/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#pragma once


#include <inttypes.h>

#include "Stream.h"
#include "SERCOM.h"
#include "RingBuffer.h"

#define HARDSER_PARITY_EVEN (0x1ul)
#define HARDSER_PARITY_ODD	(0x2ul)
#define HARDSER_PARITY_NONE (0x3ul)
#define HARDSER_PARITY_MASK	(0xFul)

#define HARDSER_STOP_BIT_1		(0x10ul)
#define HARDSER_STOP_BIT_1_5	(0x20ul)
#define HARDSER_STOP_BIT_2	 	(0x30ul)
#define HARDSER_STOP_BIT_MASK	(0xF0ul)

#define HARDSER_DATA_5	 	(0x100ul)
#define HARDSER_DATA_6	 	(0x200ul)
#define HARDSER_DATA_7	 	(0x300ul)
#define HARDSER_DATA_8	 	(0x400ul)
#define HARDSER_DATA_MASK	(0xF00ul)

#define SERIAL_5N1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_NONE | HARDSER_DATA_5)
#define SERIAL_6N1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_NONE | HARDSER_DATA_6)
#define SERIAL_7N1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_NONE | HARDSER_DATA_7)
#define SERIAL_8N1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_NONE | HARDSER_DATA_8)
#define SERIAL_5N2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_NONE | HARDSER_DATA_5)
#define SERIAL_6N2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_NONE | HARDSER_DATA_6)
#define SERIAL_7N2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_NONE | HARDSER_DATA_7)
#define SERIAL_8N2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_NONE | HARDSER_DATA_8)
#define SERIAL_5E1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_EVEN | HARDSER_DATA_5)
#define SERIAL_6E1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_EVEN | HARDSER_DATA_6)
#define SERIAL_7E1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_EVEN | HARDSER_DATA_7)
#define SERIAL_8E1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_EVEN | HARDSER_DATA_8)
#define SERIAL_5E2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_EVEN | HARDSER_DATA_5)
#define SERIAL_6E2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_EVEN | HARDSER_DATA_6)
#define SERIAL_7E2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_EVEN | HARDSER_DATA_7)
#define SERIAL_8E2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_EVEN | HARDSER_DATA_8)
#define SERIAL_5O1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_ODD  | HARDSER_DATA_5)
#define SERIAL_6O1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_ODD  | HARDSER_DATA_6)
#define SERIAL_7O1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_ODD  | HARDSER_DATA_7)
#define SERIAL_8O1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_ODD  | HARDSER_DATA_8)
#define SERIAL_5O2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_ODD  | HARDSER_DATA_5)
#define SERIAL_6O2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_ODD  | HARDSER_DATA_6)
#define SERIAL_7O2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_ODD  | HARDSER_DATA_7)
#define SERIAL_8O2	(HARDSER_STOP_BIT_2 | HARDSER_PARITY_ODD  | HARDSER_DATA_8)


class HardwareSerial : public Stream, public ISercomIRQ
{
public:
	//(baud, SERCOMx, RX pin, TX pin, RX pinout, TX pinout, config)
    void begin(uint32_t, SERCOM *, uint16_t, uint16_t, SercomRXPad, SercomUartTXPad, uint16_t config =SERIAL_8N1);
    void end();
	
    virtual int available();
	virtual int availableForWrite();
    virtual int peek();
    virtual int read();
    virtual void flush();
    virtual size_t write(uint8_t);
    using Print::write; // pull in write(str) and write(buf, size) from Print
	
    operator bool() { return true; }
	
	virtual void IRQHandler();
	
private:
	SERCOM_USART *sercom;
	RingBuffer rxBuffer;
	RingBuffer txBuffer;
	
	SercomNumberStopBit extractNbStopBit(uint16_t config);
	SercomUartCharSize extractCharSize(uint16_t config);
	SercomParityMode extractParity(uint16_t config);
};

extern void serialEventRun(void) __attribute__((weak));
