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

        private double factor = 0.0012;
        private int calY = 0, calX = 0, calZ = 0;
        private int threshold = 5;

        private void Form1_Load(object sender, EventArgs e)
        {

            swType = Type.GetTypeFromProgID("SldWorks.Application");    //SldWorks.Application.21 pour le 2013
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
            calZ = jState.Z;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            jState = sJoy.State;
            double angX = 0;
            double angY = 0;
            double angZ = 0;
            bool[] Jbuttons = jState.GetButtons();

            if (Math.Abs(jState.X-calX) > threshold)
            {
                angX = ((int)(jState.X - calX - (Math.Sign(jState.X)*5))) * (Math.PI / 6) * factor;
            }
            if (Math.Abs(jState.Y-calY) > threshold) 
            {
                angY = ((int)(jState.Y - calY - (Math.Sign(jState.X)*5))) * (Math.PI / 6) * factor;
            }
            if (Math.Abs(jState.Z - calZ) > threshold)
            {
                angZ = ((int)(jState.Z - calZ - (Math.Sign(jState.Z) * 5))) * (Math.PI / 6) * factor;
            }
            if ((Math.Abs(jState.X-calX) > threshold) || (Math.Abs(jState.Y-calY) > threshold) || (Math.Abs(jState.Z-calZ) > threshold))
            {
                label1.Text = "X=" + Math.Round(180 * angX / Math.PI, 3).ToString() + "°";
                label1.Text += " Y=" + Math.Round(180 * angY / Math.PI, 3).ToString() + "°";
                label1.Text += " Z=" + Math.Round(1 - angZ/4 ,3).ToString();
                if ((angX != 0) || (angY != 0) || (angZ != 0))
                {
                    if (Jbuttons[1])    //translate
                    {
                        modelView.TranslateBy(angX / 40, -angY / 40);
                    }
                    else if (Jbuttons[0])    //zoom
                    {
                        modelView.ZoomByFactor(1 - angY / 4);
                    }
                    else
                    {
                        modelView.RotateAboutCenter(angY, angX);
                        modelView.RollBy(angZ);
                    }
                }
            }

        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            swModel = ((ModelDoc2)(app.ActiveDoc));
            modelView = (ModelView)swModel.ActiveView;
        }

        private void cb_threshold_SelectedIndexChanged(object sender, EventArgs e)
        {
            threshold = int.Parse(cb_threshold.Text);
        }

        private void tb_sensivity_Scroll(object sender, EventArgs e)
        {
            factor = tb_sensivity.Value;
            factor = factor / 10000;
        }
    }
}
