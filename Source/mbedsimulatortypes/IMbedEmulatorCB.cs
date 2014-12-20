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
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace mbedsimulatortypes
{


    public delegate void serial_event(int mode);
    public delegate void gpio_event(uint id, int evt);

    public interface IMbedEmulatorCB
    {
        void trigger_reset();
        void terminate_sim();

        int serial_getc();
        void serial_putc(int ch);
        bool serial_readable();
        void serial_attach(serial_event handler, int mode);
        void serial_detach();
        void serial_send_break();

        void textlcd_init(int cols, int rows);
        void textlcd_locate(int col, int row);
        void textlcd_cls();
        void textlcd_putc(int ch);

        void gpio_write(PinName pin, bool is_out, int value);
        int gpio_read(PinName pin, bool is_out);
        void gpio_irq_connect(uint id, PinName pin, gpio_event handler);
        void gpio_irq_disconnect(uint id);
        void gpio_irq_set(uint id, int evt, bool enable);
        void gpio_irq_enable(uint id, bool enable);
    }
}

