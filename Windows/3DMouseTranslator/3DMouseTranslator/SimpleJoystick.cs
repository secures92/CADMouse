using System;
using SlimDX.DirectInput;

namespace _3DMouseTranslator
{
    class SimpleJoystick
    {
        /// 
        /// Joystick handle
        /// 
        private Joystick Joystick;

        /// 
        /// Get the state of the joystick
        /// 
        public JoystickState State
        {
            get
            {

                if (Joystick.Acquire().IsFailure)
                    throw new Exception("Joystick failure");

                if (Joystick.Poll().IsFailure)
                    throw new Exception("Joystick failure");

                return Joystick.GetCurrentState();
            }
        }

        /// 
        /// Construct, attach the joystick
        /// 
        public SimpleJoystick()
        {
            DirectInput dinput = new DirectInput();

            // Search for device
            foreach (DeviceInstance device in dinput.GetDevices(DeviceClass.GameController, DeviceEnumerationFlags.AttachedOnly))
            {
                // Create device
                try
                {
                    Joystick = new Joystick(dinput, device.InstanceGuid);
                    break;
                }
                catch (DirectInputException)
                {
                }
            }

            if (Joystick == null)
                throw new Exception("No joystick found");

            foreach (DeviceObjectInstance deviceObject in Joystick.GetObjects())
            {
                if ((deviceObject.ObjectType & ObjectDeviceType.Axis) != 0)
                    Joystick.GetObjectPropertiesById((int)deviceObject.ObjectType).SetRange(-100, 100);
            }

            // Acquire sdevice
            Joystick.Acquire();
        }

        /// 
        /// Release joystick
        /// 
        public void Release()
        {
            if (Joystick != null)
            {
                Joystick.Unacquire();
                Joystick.Dispose();
            }

            Joystick = null;
        }
    }
}