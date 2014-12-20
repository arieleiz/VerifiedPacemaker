/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

// This is the main DLL file.

#include "stdafx.h"
#include "mbedsimulatorctr.h"
using namespace System::Runtime::InteropServices;
#include <string>

namespace mbedsimulatorctr {

	MbedEmulator::MbedEmulator(mbedsimulatortypes::MbedProgramInfo^ info, mbedsimulatortypes::IMbedEmulatorCB^ cb)
	{
		_info = info;
		_cb = cb;
		_imp = NULL;
	}

	MbedEmulator::~MbedEmulator()
	{
		try
		{
			stop();
		}
		catch(...)
		{
		}

	}

	void MbedEmulator::start()
	{
		if (_imp != NULL)
			return;

		_imp = new MbedEmulatorImp(this);
		if (!_imp->load(_info))
		{
			delete _imp;
			_imp = NULL;
			throw gcnew mbedsimulatortypes::LoadException(_info);
		}
	}

	void MbedEmulator::stop()
	{
		if (_imp != NULL)
		{
			_imp->stop();
			delete _imp;
			_imp = NULL;
		}
	}

	mbedsimulatortypes::serial_event^ MbedEmulator::get_serial_event()
	{
		return gcnew mbedsimulatortypes::serial_event(this, &mbedsimulatorctr::MbedEmulator::_serial_event);
	}

	void MbedEmulator::_serial_event(int mode)
	{
		if (_imp == NULL)
			return;

		_imp->_serial_event(mode);
	}

	////////////////////////////////////////////////////////////

	AutoDLL::AutoDLL(const wchar_t* path)
	{
		_module = NULL;
		open(path);
	}

	AutoDLL::~AutoDLL()
	{
		close();
		if (!DeleteFileW(_temp_file) && GetFileAttributes(_temp_file) != INVALID_FILE_ATTRIBUTES)
		{
			MoveFileEx(_temp_file, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
		}
	}

	bool AutoDLL::open(const wchar_t* path)
	{
		wchar_t temp_dir[MAX_PATH];
		GetTempPath(MAX_PATH, temp_dir);
		GetTempFileName(temp_dir, L"MBD", 0, _temp_file);
		wcscat_s(_temp_file, L".dll");
		CopyFile(path, _temp_file, FALSE);

		_module = LoadLibrary(_temp_file);
		return (_module != NULL);
	}

	FARPROC AutoDLL::get_proc_addr(const char* name)
	{
		if (_module == NULL)
			return NULL;

		return GetProcAddress(_module, name);
	}

	void AutoDLL::close()
	{
		if (_module != NULL)
		{
			FreeLibrary(_module);
			_module = NULL;
		}
	}

	void AutoDLL::steal(AutoDLL& other)
	{
		_module = other._module;
		other._module = NULL;
	}

	MbedEmulatorImp::MbedEmulatorImp(MbedEmulator^ emulator)
	{
		_emu = emulator;
		memset(&_ucb, 0, sizeof(_ucb));

		_api.myptr = this;

		_api.terminate_sim = terminate_sim;
		_api.trigger_reset = trigger_reset;
		_api.serial_getc = serial_getc;
		_api.serial_putc = serial_putc;
		_api.serial_readable = serial_readable;
		_api.serial_attach = serial_attach;
		_api.serial_detach = serial_detach;
		_api.serial_send_break = serial_send_break;
		
		_api.textlcd_init = textlcd_init;
		_api.textlcd_locate = textlcd_locate;
		_api.textlcd_cls = textlcd_cls;
		_api.textlcd_putc = textlcd_putc;

		_api.gpio_read = gpio_read;
		_api.gpio_write = gpio_write;
		_api.gpio_irq_connect = gpio_irq_connect;
		_api.gpio_irq_disconnect = gpio_irq_disconnect;
		_api.gpio_irq_enable = gpio_irq_enable;
		_api.gpio_irq_set = gpio_irq_set;
	}

	bool MbedEmulatorImp::load(mbedsimulatortypes::MbedProgramInfo^ info)
	{
		IntPtr p = Marshal::StringToHGlobalUni(info->dllname);
		std::wstring dllname(reinterpret_cast<wchar_t*>(p.ToPointer()));
		Marshal::FreeHGlobal(p);

		// we need to pass paramters to the DLL before DllMain returns,
		// and acutally even before the CRT's DLLMain calls ours.
		// ugly but works
		DLLInitStackCanary canary;
		canary.magic1 = DLLINITSTACKCANARY_MAGIC;
		canary.magic_xor_address1 = (DLLINITSTACKCANARY_MAGIC ^ (DWORD)&canary.magic_xor_address1);
		canary.magic2 = DLLINITSTACKCANARY_MAGIC;
		canary.magic_xor_address2 = (DLLINITSTACKCANARY_MAGIC ^ (DWORD)&canary.magic_xor_address2);
		canary.api = &_api;
		canary.cb = &_ucb;

		AutoDLL module(dllname.c_str());
		if (!module.ok())
			return false;
	
		memset(&canary, 0, sizeof(canary));

		_module.steal(module);

		return true;
	}

	MbedEmulatorImp::~MbedEmulatorImp()
	{
		try
		{
			if (_module.ok())
			{
				stop();
			}
		}
		catch (...)
		{
		}

	}

	void MbedEmulatorImp::stop()
	{
		if (_ucb.stop== NULL)
			throw gcnew System::Exception(gcnew String(L"Invalid component!"));
		_ucb.stop();
	}

	// CBs
	void __cdecl MbedEmulatorImp::terminate_sim(struct ComponentAPI* c)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		return ci->myptr->_emu->get_cb()->terminate_sim();
	}

	void __cdecl MbedEmulatorImp::trigger_reset(struct ComponentAPI* c)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		return ci->myptr->_emu->get_cb()->trigger_reset();
	}

	int	__cdecl  MbedEmulatorImp::serial_getc(struct ComponentAPI* c)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		return ci->myptr->_emu->get_cb()->serial_getc();
	}

	void __cdecl MbedEmulatorImp::serial_putc(struct ComponentAPI* c, int ch)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		MbedEmulator^ emu = ci->myptr->_emu;
		emu->get_cb()->serial_putc(ch);
	}

	int  __cdecl MbedEmulatorImp::serial_readable(struct ComponentAPI* c)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		return ci->myptr->_emu->get_cb()->serial_readable();
	}


	void __cdecl MbedEmulatorImp::serial_attach(struct ComponentAPI* c, int mode)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->serial_attach(ci->myptr->_emu->get_serial_event(), mode);
	}

	void MbedEmulatorImp::_serial_event(int mode)
	{
		_ucb.serial_event(mode);
	}

	void __cdecl MbedEmulatorImp::serial_detach(struct ComponentAPI* c)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->serial_detach();
	}

	void __cdecl MbedEmulatorImp::serial_send_break(struct ComponentAPI* c)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->serial_send_break();
	}

	void __cdecl MbedEmulatorImp::textlcd_init(struct ComponentAPI* c, int cols, int rows)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->textlcd_init(cols, rows);
	}

	void __cdecl MbedEmulatorImp::textlcd_locate(struct ComponentAPI* c, int col, int row)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->textlcd_locate(col, row);
	}

	void __cdecl MbedEmulatorImp::textlcd_cls(struct ComponentAPI* c)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->textlcd_cls();
	}

	void __cdecl MbedEmulatorImp::textlcd_putc(struct ComponentAPI* c, int ch)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->textlcd_putc(ch);
	}

	void __cdecl MbedEmulatorImp::gpio_write(struct ComponentAPI* c, int pin, bool is_out, int value)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->gpio_write(static_cast<mbedsimulatortypes::PinName>(pin), is_out, value);
	}

	int  __cdecl MbedEmulatorImp::gpio_read(struct ComponentAPI* c, int pin, bool is_out)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		return ci->myptr->_emu->get_cb()->gpio_read(static_cast<mbedsimulatortypes::PinName>(pin), is_out);
	}

	ref class GPIOHandlerCBWrapper
	{
	public:
		GPIOHandlerCBWrapper(void(*handler)(uint32_t, int))
		{
			_handler = handler;
		}

	public:
		void evtfunc(unsigned int id, int evt)
		{
			_handler(id, evt);
		}

		void(*_handler)(uint32_t, int);
	};

	void __cdecl MbedEmulatorImp::gpio_irq_connect(struct ComponentAPI* c, uint32_t id, int pin, void(*handler)(uint32_t, int))
	{
		GPIOHandlerCBWrapper^ wrapper = gcnew GPIOHandlerCBWrapper(handler);
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->gpio_irq_connect(id, static_cast<mbedsimulatortypes::PinName>(pin), 
					gcnew mbedsimulatortypes::gpio_event(wrapper, &GPIOHandlerCBWrapper::evtfunc));
	}

	void __cdecl MbedEmulatorImp::gpio_irq_disconnect(struct ComponentAPI* c, uint32_t id)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->gpio_irq_disconnect(id);
	}

	void __cdecl MbedEmulatorImp::gpio_irq_set(struct ComponentAPI* c, uint32_t id, int evt, bool enable)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->gpio_irq_set(id, evt, enable);
	}

	void __cdecl MbedEmulatorImp::gpio_irq_enable(struct ComponentAPI* c, uint32_t id, bool enable)
	{
		ComponentAPIImp* ci = (ComponentAPIImp*)c;
		ci->myptr->_emu->get_cb()->gpio_irq_enable(id, enable);
	}

	
}
