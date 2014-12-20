/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

#include "simhw.hpp"
#include "mbed.h"
#include "rtos.h"
#include "gpio_api.h"

uint32_t gpio_set(PinName pin)
{
	UNREFERENCED_PARAMETER(pin);
	return 1;
}

void gpio_init(gpio_t *obj, PinName pin)
{
	obj->pin = pin;
	obj->is_out = false;
}


void gpio_mode(gpio_t *obj, PinMode mode)
{
	UNREFERENCED_PARAMETER(obj);
	UNREFERENCED_PARAMETER(mode);
}

void gpio_dir(gpio_t *obj, PinDirection direction)
{
	UNREFERENCED_PARAMETER(obj);
	UNREFERENCED_PARAMETER(direction);
	obj->is_out = direction == PIN_OUTPUT;
}

void gpio_write(gpio_t *obj, int value)
{
	AESIM::gpio_write(obj->pin, obj->is_out, value);
}

int  gpio_read(gpio_t *obj)
{
	return AESIM::gpio_read(obj->pin, obj->is_out);
}

void gpio_init_in(gpio_t* gpio, PinName pin)
{
	gpio->pin = pin;
	gpio->is_out = false;
	gpio_read(gpio);
}

void gpio_init_in_ex(gpio_t* gpio, PinName pin, PinMode mode)
{
	gpio->pin = pin;
	gpio->is_out = false;
	gpio_read(gpio);
	UNREFERENCED_PARAMETER(mode);
}

void gpio_init_out(gpio_t* gpio, PinName pin)
{
	gpio->pin = pin;
	gpio->is_out = true;
	gpio_write(gpio, 1);
}

void gpio_init_out_ex(gpio_t* gpio, PinName pin, int value)
{
	gpio->pin = pin;
	gpio->is_out = true;
	AESIM::gpio_write(pin, gpio->is_out, value);
}

void gpio_init_inout(gpio_t* gpio, PinName pin, PinDirection direction, PinMode mode, int value)
{
	gpio->pin = pin;
	gpio->is_out = direction == PIN_OUTPUT;
	if(gpio->is_out)
		AESIM::gpio_write(pin, gpio->is_out, value);
	UNREFERENCED_PARAMETER(direction);
	UNREFERENCED_PARAMETER(mode);
}

int  gpio_irq_init(gpio_irq_t *obj, PinName pin, gpio_irq_handler handler, uint32_t id)
{
	obj->pin = pin;
	obj->handler = handler;
	obj->id = id;
	AESIM::gpio_irq_connect(id, pin, (AESIM::gpio_irq_handler)handler);
	AESIM::gpio_read(pin, false);
	return osOK;
}

void gpio_irq_free(gpio_irq_t *obj)
{
	AESIM::gpio_irq_disconnect(obj->id);
}

void gpio_irq_set(gpio_irq_t *obj, gpio_irq_event event, uint32_t enable)
{
	AESIM::gpio_irq_set(obj->id, (int)event, enable != 0);
}

void gpio_irq_enable(gpio_irq_t *obj)
{
	AESIM::gpio_irq_enable(obj->id, true);
}

void gpio_irq_disable(gpio_irq_t *obj)
{
	AESIM::gpio_irq_enable(obj->id, false);
}

