
#include "main.h"
#include "mouse.h"
#include "avr/io.h"

void mouseInit()
{
	adcInit(); // Init ADC
	//getADCValue(0); // Test Measurement
}

void adcInit()
{
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz

	ADMUX |= (1 << REFS0); // Set ADC reference to AVCC
	ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading


	//ADCSRB &= ~((1 << ADTS0)|(1 << ADTS1)|(1 << ADTS2));  // Set ADC to Free-Running Mode
	//ADCSRA |= (1<<ADATE); // Enable Auto Trigger
	ADCSRA |= (1 << ADEN);  // Enable ADC
	ADCSRA |= (1 << ADSC);  // Start A2D Conversions
	while (ADCSRA&(1<<ADSC)); // Wait for Result
}

int getADCValue(int channel)
{
	ADMUX &= 0xF0;
	ADMUX |= channel;
	ADCSRA |= (1 << ADSC);  // Start A2D Conversions
	while (ADCSRA&(1<<ADSC)); // Wait for Result
	return ADCH;
}

int getXAxis()
{
	return (255-getADCValue(XAxis));
}
int getYAxis()
{
	return (255-getADCValue(YAxis));
}
int getZAxis()
{
	return (255-getADCValue(ZAxis));
}