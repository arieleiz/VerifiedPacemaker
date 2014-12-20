/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using mbedsimulatortypes;

namespace mbedsimulator
{
    public partial class PushButton : GPIOElement
    {
        public PushButton()
        {
            InitializeComponent();

        }

        protected override void write(int value)
        {
        }

        public override void setDevice(Device d)
        {
            base.setDevice(d);
            _device = d;
            setImage();
        }


        protected override int read()
        {
            return _lastValue;
        }

        private void LED_Load(object sender, EventArgs e)
        {
            _on = Properties.Resources.pushbuttonclose; 
            _off = Properties.Resources.pushbuttonopen;
            setImage();
        }

        void setImage()
        {
            pictureBox1.Image = _lastValue > 0 ? _on : _off;
        }

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            setValue(1);
            setImage();
        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            setValue(0);
            setImage();
        }

        Bitmap _on, _off;

    }
}
