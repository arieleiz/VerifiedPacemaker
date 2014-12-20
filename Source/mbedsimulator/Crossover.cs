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
    public partial class Crossover : GPIOElement
    {
        private static List<Crossover> Crossovers = new List<Crossover>();

        public Crossover()
        {
            InitializeComponent();
            Crossovers.Add(this);

        }

        public override void remove()
        {
            Crossovers.Remove(this);
        }

        protected override void write(int value)
        {
            base.write(value);
            foreach(Crossover c in Crossovers)
            {
                if(c != this && c._device.name == _device.name)
                {
                    c.setValue(value);
                }
            }
        }

        public override void setDevice(Device d)
        {
            base.setDevice(d);
            _device = d;
            label2.Text = "Crossover {" + d.extra + "}";
        }

    }
}
