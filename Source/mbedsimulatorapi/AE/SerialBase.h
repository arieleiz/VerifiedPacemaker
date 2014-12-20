/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_SERIALBASE_H
#define MBED_SERIALBASE_H

#include "platform.h"
#include "simhw.hpp"

#if DEVICE_SERIAL

#include "Stream.h"
#include "FunctionPointer.h"
#include "serial_api.h"

namespace mbed {

/** A base class for serial port implementations
 * Can't be instantiated directly (use Serial or RawSerial)
 */
class SerialBase {

public:
    /** Set the baud rate of the serial port
     *
     *  @param baudrate The baudrate of the serial port (default = 9600).
     */
	void baud(int baudrate) { UNREFERENCED_PARAMETER(baudrate);  }

    enum Parity {
        None = 0,
        Odd,
        Even,
        Forced1,
        Forced0
    };

    enum IrqType {
        RxIrq = 0,
        TxIrq
    };

    enum Flow {
        Disabled = 0,
        RTS,
        CTS,
        RTSCTS
    };

    /** Set the transmission format used by the serial port
     *
     *  @param bits The number of bits in a word (5-8; default = 8)
     *  @param parity The parity used (SerialBase::None, SerialBase::Odd, SerialBase::Even, SerialBase::Forced1, SerialBase::Forced0; default = SerialBase::None)
     *  @param stop The number of stop bits (1 or 2; default = 1)
     */
	void format(int bits = 8, Parity parity = SerialBase::None, int stop_bits = 1) 
	{ 
		UNREFERENCED_PARAMETER(bits);
		UNREFERENCED_PARAMETER(parity);
		UNREFERENCED_PARAMETER(stop_bits);
	}

    /** Determine if there is a character available to read
     *
     *  @returns
     *    1 if there is a character available to read,
     *    0 otherwise
     */
	int readable()
	{
		return AESIM::serial_readable(this);
	}

    /** Determine if there is space available to write a character
     *
     *  @returns
     *    1 if there is space to write a character,
     *    0 otherwise
     */
    int writeable()
	{
		return 1;
	}

    /** Attach a function to call whenever a serial interrupt is generated
     *
     *  @param fptr A pointer to a void function, or 0 to set as none
     *  @param type Which serial interrupt to attach the member function to (Seriall::RxIrq for receive, TxIrq for transmit buffer empty)
     */
	void attach(void(*fptr)(void), IrqType type = RxIrq)
	{
		_irq[type].attach(fptr);
		AESIM::serial_attach(this, type);
	}

    /** Attach a member function to call whenever a serial interrupt is generated
     *
     *  @param tptr pointer to the object to call the member function on
     *  @param mptr pointer to the member function to be called
     *  @param type Which serial interrupt to attach the member function to (Seriall::RxIrq for receive, TxIrq for transmit buffer empty)
     */
    template<typename T>
    void attach(T* tptr, void (T::*mptr)(void), IrqType type=RxIrq) {
        if((mptr != NULL) && (tptr != NULL)) {
            _irq[type].attach(tptr, mptr);
			AESIM::serial_attach(this, type);
        }
    }

	void on_event(IrqType type)
	{
		_irq[type]();
	}

    /** Generate a break condition on the serial line
     */
	void send_break()
	{
		AESIM::serial_send_break(this); 
	}

#if DEVICE_SERIAL_FC
    /** Set the flow control type on the serial port
     *
     *  @param type the flow control type (Disabled, RTS, CTS, RTSCTS)
     *  @param flow1 the first flow control pin (RTS for RTS or RTSCTS, CTS for CTS)
     *  @param flow2 the second flow control pin (CTS for RTSCTS)
     */
	void set_flow_control(Flow type, PinName flow1 = NC, PinName flow2 = NC)
	{
		UNREFERENCED_PARAMETER(type);
		UNREFERENCED_PARAMETER(flow2);
		UNREFERENCED_PARAMETER(flow1);
	}
#endif

	int get_rx() { return _rx; } 
	int get_tx() { return _tx; }

protected:
	SerialBase(PinName tx, PinName rx) {
		_tx = tx;
		_rx = rx;

	}

    virtual ~SerialBase() {
		AESIM::serial_detach(this);
    }

	int _base_getc()
	{
		return AESIM::serial_getc(this);
	}

	int _base_putc(int c)
	{
		AESIM::serial_putc(this, c);
		return 0;
	}

	PinName _tx, _rx;
	FunctionPointer _irq[2];
};

} // namespace mbed

#endif

#endif
