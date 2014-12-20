/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

// mbedsimulatorctr.h

#pragma once
#include <Windows.h>
#include "CTRAPI.h"
#include <vcclr.h>

using namespace System;

namespace mbedsimulatorctr {

	class MbedEmulatorImp;
		
	public ref class MbedEmulator
	{
	public:
		MbedEmulator(mbedsimulatortypes::MbedProgramInfo^ info, mbedsimulatortypes::IMbedEmulatorCB^ cb);
		~MbedEmulator();

		void start();
		void stop();

		inline mbedsimulatortypes::MbedProgramInfo^ get_info() {
			return _info;
		}
		inline mbedsimulatortypes::IMbedEmulatorCB^ get_cb(){
			return _cb;
		}

		mbedsimulatortypes::serial_event^ get_serial_event();

	private:
		void _serial_event(int mode);

	private:
		mbedsimulatortypes::MbedProgramInfo^ _info;
		mbedsimulatortypes::IMbedEmulatorCB^ _cb;
		MbedEmulatorImp* _imp;
	};

	struct ComponentAPIImp : public ComponentAPI
	{
		MbedEmulatorImp* myptr;
	};

	class AutoDLL
	{
	public:
		AutoDLL() {
			_module = NULL;
		}
		AutoDLL(const wchar_t* path);
		~AutoDLL();

		bool open(const wchar_t* path);
		void steal(AutoDLL& other);

		bool ok() {
			return _module != NULL;
		}

		FARPROC get_proc_addr(const char* name);

		void close();

	private:
		wchar_t _temp_file[MAX_PATH + 8];
		HMODULE _module;
	};

	class MbedEmulatorImp
	{
	public:
		MbedEmulatorImp(MbedEmulator^ emulator);
		~MbedEmulatorImp();

		void start();
		void stop();
		bool load(mbedsimulatortypes::MbedProgramInfo^ info);

		// CBs
		static void __cdecl terminate_sim(struct ComponentAPI* c);
		static void __cdecl trigger_reset(struct ComponentAPI* c);

		static int	__cdecl  serial_getc(struct ComponentAPI* c);
		static void __cdecl serial_putc(struct ComponentAPI* c, int ch);
		static int  __cdecl serial_readable(struct ComponentAPI* c);
		static void __cdecl serial_attach(struct ComponentAPI* c, int mode);
		static void __cdecl serial_detach(struct ComponentAPI* s);
		static void __cdecl serial_send_break(struct ComponentAPI* c);

		static void __cdecl textlcd_init(struct ComponentAPI* c, int cols, int rows);
		static void __cdecl textlcd_locate(struct ComponentAPI* c, int col, int row);
		static void __cdecl textlcd_cls(struct ComponentAPI* c);
		static void __cdecl textlcd_putc(struct ComponentAPI* c, int ch);

		static void gpio_write(struct ComponentAPI* c, int pin, bool is_out, int value);
		static int gpio_read(struct ComponentAPI* c, int pin, bool is_out);
		static void gpio_irq_connect(struct ComponentAPI* c, uint32_t id, int pin, void(*handler)(uint32_t, int));
		static void gpio_irq_disconnect(struct ComponentAPI* c, uint32_t id);
		static void gpio_irq_set(struct ComponentAPI* c, uint32_t id, int evt, bool enable);
		static void gpio_irq_enable(struct ComponentAPI* c, uint32_t id, bool enable);

		void _serial_event(int mode);

	private:
		ComponentAPIImp _api;
		ComponentCB _ucb;
		AutoDLL _module;
		gcroot <MbedEmulator^> _emu;
	};
}
