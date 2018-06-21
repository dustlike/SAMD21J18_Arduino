/*
 * TWI/I2C library for Arduino Zero
 * Copyright (c) 2015 Arduino LLC. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


#pragma once

#include "Stream.h"
#include "SERCOM.h"
#include "RingBuffer.h"


class TwoWire : public Stream, public ISercomIRQ
{
public:
	TwoWire();
	
	//begin as master
	void begin(SERCOM *s, uint16_t pinSDA, uint16_t pinSCL);
	
	//begin as slave
	void begin(uint8_t slaveAddr, SERCOM *s, uint16_t pinSDA, uint16_t pinSCL);
	
	//disable I2C
	void end();
	
	//should call this before begin()
	void changeClock(uint32_t);
	
	void beginTransmission(uint8_t);
	uint8_t endTransmission(bool stopBit = true);
	
	uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit);
	uint8_t requestFrom(uint8_t address, size_t quantity) { return requestFrom(address, quantity, true); }

	size_t write(uint8_t data);
	size_t write(const uint8_t * data, size_t quantity);

	virtual int available(void);
	virtual int read(void);
	virtual int peek(void);
	virtual void flush(void);
	void onReceive(void(*)(int));
	void onRequest(void(*)(void));

	inline size_t write(unsigned long n) { return write((uint8_t)n); }
	inline size_t write(long n) { return write((uint8_t)n); }
	inline size_t write(unsigned int n) { return write((uint8_t)n); }
	inline size_t write(int n) { return write((uint8_t)n); }
	using Print::write;

	virtual void IRQHandler();

private:
	SERCOM_I2C *sercom;

	bool transmissionBegun;

	// RX Buffer
	RingBuffer rxBuffer;

	//TX buffer
	RingBuffer txBuffer;
	uint8_t txAddress;

	// Callback user functions
	void (*onRequestCallback)(void);
	void (*onReceiveCallback)(int);
	
	//i2c clock is 100K by default
	enum { DEFAULT_TWI_CLOCK = 100000 };
	
	uint32_t master_mode_clock;
};


extern TwoWire Wire;