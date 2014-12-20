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
    public partial class GPIOElement : UserControl
    {
        public GPIOElement()
        {
            InitializeComponent();
        
        }

        public virtual void remove()
        {
        }

        public uint Id {  get { return _id; } }

        public PinName Pin = PinName.NC;
        public bool IsOut = false;
        public virtual void setDevice(Device d)
        {
            _device = d;
            Pin = d.pin;
            label1.Text = String.Format("{0} ({1})", d.name, d.pin);
        }

        public Device GetDevice() { return _device; }

        public void connect(uint id, gpio_event e)
        {
            _id = id;
            _irq = e;
            _irq_on_fall = _irq_on_rise = false;
        }

        public void disconnect()
        {
            _id = uint.MaxValue;
            _irq = null;
            _irq_on_fall = _irq_on_rise = false;
        }

        public void setirq(int evt, bool enable)
        {
            switch(evt)
            {
                case 1: _irq_on_rise = enable; break;
                case 2: _irq_on_fall = enable; break;
            }
        }

        public int Value
        {
            get
            {
                if (IsOut)
                    return _lastValue;
                else
                    return read();
            }
            set
            {
                if (IsOut && value != _lastValue)
                    write(value);
            }
        }

        protected virtual void write(int value)
        {
            _lastValue = value;
        }

        protected virtual int read()
        {
            return _lastValue;
        }

        public void setValue(int value)
        {
            if(_lastValue == value)
                return;
            _lastValue = value;
            if (_lastValue == 1 && _irq != null && _irq_on_rise)
                _irq(_id, 1);
            else if (_lastValue == 0 && _irq != null && _irq_on_fall)
                _irq(_id, 2);
        }

        protected int _lastValue = 0;
        protected Device _device;
        protected gpio_event _irq = null;
        private bool _irq_on_rise = false;
        private bool _irq_on_fall = false;
        private uint _id = uint.MaxValue;
    }
}
