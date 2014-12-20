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
    public partial class LED : GPIOElement
    {
        public LED()
        {
            InitializeComponent();

        }

        protected override void write(int value)
        {
            base.write(value);
            setImage();
        }

        public override void setDevice(Device d)
        {
            base.setDevice(d);
            _device = d;
            switch(d.extra.ToLower())
            {
                case "red":
                    _on = Properties.Resources.redon; 
                    _off = Properties.Resources.redoff;
                    break;
                case "blue":
                    _on = Properties.Resources.blueon;
                    _off = Properties.Resources.blueoff;
                    break;
                case "green":
                    _on = Properties.Resources.greenon;
                    _off = Properties.Resources.greenoff;
                    break;
                case "yellow":
                    _on = Properties.Resources.yellowon;
                    _off = Properties.Resources.yellowoff;
                    break;
                default:
                    _on = Properties.Resources.blueon;
                    _off = Properties.Resources.blueoff;
                    break;
            }
            setImage();
        }


        protected override int read()
        {
            return 0;
        }

        private void LED_Load(object sender, EventArgs e)
        {
            setImage();
        }

        void setImage()
        {
            pictureBox1.Image = _lastValue > 0 ? _on : _off;
        }

        Bitmap _on, _off;

    }
}
