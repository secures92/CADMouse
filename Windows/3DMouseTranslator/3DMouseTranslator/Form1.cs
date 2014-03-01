using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SolidWorks.Interop.sldworks;
using SlimDX.DirectInput;

namespace _3DMouseTranslator
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        private Type swType;
        private SldWorks app;
        private ModelView modelView;
        private ModelDoc2 swModel;

        private SimpleJoystick sJoy;
        private JoystickState jState;

        private double factor = 0.01;
        private int calY = 0, calX = 0;
        private int threshold = 5;


        private void Form1_Load(object sender, EventArgs e)
        {

            swType = Type.GetTypeFromProgID("SldWorks.Application.21");
            app = (SldWorks)Activator.CreateInstance(swType);
            if (app != null)
            {
                app.Visible = true;
                swModel = ((ModelDoc2)(app.ActiveDoc));
                modelView = (ModelView)swModel.ActiveView;
            }


            sJoy = new SimpleJoystick();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            calX = jState.X;
            calY = jState.Y;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            jState = sJoy.State;
            if ((Math.Abs(jState.X-calX) > threshold) || (Math.Abs(jState.Y-calY) > threshold))
            {

                double angX = ((int)(jState.X - calX - (Math.Sign(jState.X)*5))) * (Math.PI / 6) * factor;
                double angY = ((int)(jState.Y - calY - (Math.Sign(jState.X)*5))) * (Math.PI / 6) * factor;
                label1.Text = "X=" + Math.Round(180 * angX / Math.PI, 3).ToString() + "° Y=" + Math.Round(180 * angY / Math.PI, 3).ToString() + "°";
                if ((angX != 0) || (angY != 0))
                {
                    modelView.RotateAboutCenter(angX, angY);
                }
            }
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            swModel = ((ModelDoc2)(app.ActiveDoc));
            modelView = (ModelView)swModel.ActiveView;
        }
    }
}
