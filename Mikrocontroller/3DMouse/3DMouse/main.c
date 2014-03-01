#include "main.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h> /* for sei() */
#include <util/delay.h> /* for _delay_ms() */

#include <avr/pgmspace.h>  /* required by usbdrv.h */
#include "usbdrv/usbdrv.h"
#include "usbdrv/oddebug.h"
#include "ppm.h"
#include "mouse.h"

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

PROGMEM char const usbHidReportDescriptor[50] = { /* USB report descriptor, size must match usbconfig.h */
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x15, 0x00, // LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,  // LOGICAL_MAXIMUM (255)
	0x75, 0x08, // REPORT_SIZE (6)
	0x09, 0x04, // USAGE (Joystick)
	0xa1, 0x01, // COLLECTION (Application)
	0x09, 0x01, //  USAGE (Pointer)
	0xa1, 0x00, //  COLLECTION (Physical)
	0x09, 0x30, // USAGE (X)
	0x09, 0x31, // USAGE (Y)
	0x95, 0x02, // REPORT_COUNT (2)
	0x81, 0x82, // INPUT (Data,Var,Abs,Vol)
	0xc0, //  END_COLLECTION
	0xa1, 0x00, //  COLLECTION (Physical)
	0x09, 0x32, // USAGE (Z)
	0x09, 0x33, // USAGE (Rx)
	0x95, 0x02, // REPORT_COUNT (2)
	0x81, 0x82, // INPUT (Data,Var,Abs,Vol)
	0xc0, //  END_COLLECTION
	0x09, 0x34, //  USAGE (Ry)
	0x09, 0x35, //  USAGE (Rz)
	0x09, 0x36, //  USAGE (Slider)
	0x09, 0x37, //  USAGE (Dial)
	0x95, 0x04, //  REPORT_COUNT (2)
	0x81, 0x82, //  INPUT (Data,Var,Abs,Vol)
	0xc0 // END_COLLECTION
};
//PROGMEM const char usbHidReportDescriptor[50] = {
	//0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	//0x09, 0x02,                    // USAGE (Mouse)
	//0xa1, 0x01,                    // COLLECTION (Application)
	//0x09, 0x01,                    //   USAGE (Pointer)
	//0xa1, 0x00,                    //   COLLECTION (Physical)
	//0x05, 0x09,                    //     USAGE_PAGE (Button)
	//0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
	//0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
	//0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	//0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
	//0x95, 0x03,                    //     REPORT_COUNT (3)
	//0x75, 0x01,                    //     REPORT_SIZE (1)
	//0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	//0x95, 0x01,                    //     REPORT_COUNT (1)
	//0x75, 0x05,                    //     REPORT_SIZE (5)
	//0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	//0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
	//0x09, 0x30,                    //     USAGE (X)
	//0x09, 0x31,                    //     USAGE (Y)
	//0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
	//0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
	//0x75, 0x08,                    //     REPORT_SIZE (8)
	//0x95, 0x02,                    //     REPORT_COUNT (2)
	//0x81, 0x06,                    //     INPUT (Data,Var,Rel)
	//0xc0,                          //   END_COLLECTION
	//0xc0                           // END_COLLECTION
//};
//

static uchar reportBuffer[8];
static uchar idleRate;  /* repeat rate for keyboards, never used for mice */


/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	usbRequest_t *rq = (void *)data;

	if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){ /* class request type */
		DBG1(0x50, &rq->bRequest, 1);  /* debug output: print our request */
		if(rq->bRequest == USBRQ_HID_GET_REPORT){ /* wValue: ReportType (highbyte), ReportID (lowbyte) */
			/* we only have one report type, so don't look at wValue */
			usbMsgPtr = (void *)&reportBuffer;
			return sizeof(reportBuffer);
			}else if(rq->bRequest == USBRQ_HID_GET_IDLE){
			usbMsgPtr = &idleRate;
			return 1;
			}else if(rq->bRequest == USBRQ_HID_SET_IDLE){
			idleRate = rq->wValue.bytes[1];
		}
		}else{
		/* no vendor specific requests implemented */
	}
	return 0;  /* default for not implemented requests: return no data back to host */
}
//
//uchar usbFunctionSetup(uchar data[8])
//{
	//usbRequest_t    *rq = (void *)data;
//
	//usbMsgPtr = reportBuffer;
	//if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
		//if(rq->bRequest == USBRQ_HID_GET_REPORT){
			//return sizeof(reportBuffer);
		//}
		//}else{
		///* no vendor specific requests implemented */
	//}
	//return 0;
//}


/* ------------------------------------------------------------------------- */

int main(void) {
	wdt_enable(WDTO_1S);

	//odDebugInit();
	//DBG1(0x00, 0, 0);  /* debug output: main starts */

	mouseInit();

	usbInit();

	usbDeviceDisconnect(); /* enforce re-enumeration, do this while interrupts are disabled! */
	uchar i = 0;
	while(--i){  /* fake USB disconnect for > 250 ms */
		wdt_reset();
		_delay_ms(1);
	}
	usbDeviceConnect();

	sei();
	//DBG1(0x01, 0, 0);  /* debug output: main loop starts */
	
	
	for(;;){ /* main event loop */
		//DBG1(0x02, 0, 0);  /* debug output: main loop iterates */
		wdt_reset();
		usbPoll();
		
		reportBuffer[0]=getXAxis();
		reportBuffer[1]=getYAxis();
		reportBuffer[2]=getZAxis();
		
		if(usbInterruptIsReady()){
		// called after every poll of the interrupt endpoint
		//DBG1(0x03, 0, 0);  // debug output: interrupt report prepared
		usbSetInterrupt((void *)&reportBuffer, sizeof(reportBuffer));
		}
	}
}
