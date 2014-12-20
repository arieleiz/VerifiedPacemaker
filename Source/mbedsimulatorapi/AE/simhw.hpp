/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

// AE
#pragma once

#include <Windows.h>
#include "PinNames.h"

void NVIC_SystemReset(void);

namespace mbed {
	class SerialBase;
}
class TextLCD_Base;

class Win32Thread;

namespace AESIM
{
	void register_thread(Win32Thread* thread);
	void unregister_thread(Win32Thread* thread);
	void register_waitable_timer(HANDLE timer);
	void unregister_waitable_timer(HANDLE timer);
	void register_mutex(HANDLE timer);
	void unregister_mutex(HANDLE timer);

	void terminate_sim();
	void trigger_reset();

	int serial_getc(mbed::SerialBase* s);
	void serial_putc(mbed::SerialBase* s, int c);
	int serial_readable(mbed::SerialBase* s);
	void serial_attach(mbed::SerialBase* s, int mode);
	void serial_detach(mbed::SerialBase* s);
	void serial_send_break(mbed::SerialBase* s);

	void textlcd_init(TextLCD_Base* text, int cols, int rows);
	void textlcd_locate(TextLCD_Base* text, int col, int row);
	void textlcd_cls(TextLCD_Base* text);
	void textlcd_putc(TextLCD_Base* text, int c);

	typedef void(*gpio_irq_handler)(uint32_t id, int evt);

	void gpio_write(PinName pin, bool is_out, int value);
	int gpio_read(PinName pin, bool is_out);
	void gpio_irq_connect(uint32_t id, PinName pin, gpio_irq_handler handler);
	void gpio_irq_disconnect(uint32_t id);
	void gpio_irq_set(uint32_t id, int evt, bool enable);
	void gpio_irq_enable(uint32_t id, bool enable);


};

