/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct ComponentCB
	{
		void(*stop)();
		void (*serial_event)(int mode);

	} ComponentCB;


	typedef struct ComponentAPI
	{
		void(*terminate_sim)(struct ComponentAPI* c);
		void(*trigger_reset)(struct ComponentAPI* c);
		int (*serial_getc)(struct ComponentAPI* c);
		void(*serial_putc)(struct ComponentAPI* c, int ch);
		int(*serial_readable)(struct ComponentAPI* c);
		void(*serial_attach)(struct ComponentAPI* c, int mode);
		void(*serial_detach)(struct ComponentAPI* s);
		void(*serial_send_break)(struct ComponentAPI* c);

		void(*textlcd_init)(struct ComponentAPI* c, int cols, int rows);
		void(*textlcd_locate)(struct ComponentAPI* c, int col, int row);
		void(*textlcd_cls)(struct ComponentAPI* c);
		void(*textlcd_putc)(struct ComponentAPI* c, int ch);

		void(*gpio_write)(struct ComponentAPI* c, int pin, bool is_out, int value);
		int(*gpio_read)(struct ComponentAPI* c, int pin, bool is_out);
		void(*gpio_irq_connect)(struct ComponentAPI* c, uint32_t id, int pin,  void(*handler)(uint32_t, int));
		void(*gpio_irq_disconnect)(struct ComponentAPI* c, uint32_t id);
		void(*gpio_irq_set)(struct ComponentAPI* c, uint32_t id, int evt, bool enable);
		void(*gpio_irq_enable)(struct ComponentAPI* c, uint32_t id, bool enable);

	} ComponentAPI;

#define DLLINITSTACKCANARY_MAGIC 0x56566fea

	typedef struct DLLInitStackCanary
	{
		DWORD magic1;
		DWORD magic_xor_address1;
		ComponentAPI* api;
		ComponentCB* cb;
		DWORD magic_xor_address2;
		DWORD magic2;
	} DLLInitStackCanary;

#ifdef __cplusplus
}
#endif

