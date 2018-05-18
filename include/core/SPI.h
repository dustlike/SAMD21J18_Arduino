/*
 * SPI Master library for Arduino Zero.
 * Copyright (c) 2015 Arduino LLC
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

#include <Arduino.h>


struct SPISettings
{
	SPISettings(uint32_t clock = 4000000, BitOrder _bitOrder = MSBFIRST, SercomSpiClockMode _dataMode = SPI_MODE_0)
		: clockFreq(clock), dataMode(_dataMode)
	{
		//transform between BitOrder and SercomDataOrder
		if (_bitOrder == MSBFIRST)
		{
			bitOrder = MSB_FIRST;
		}
		else
		{
			bitOrder = LSB_FIRST;
		}
	}
	
	uint32_t clockFreq;
	SercomSpiClockMode dataMode;
	SercomDataOrder bitOrder;
};


class SerialPerIface
{
public:
	void begin(SPISettings, SERCOM *scm, uint16_t pinMISO, uint16_t pinSCK, uint16_t pinMOSI, SercomSpiTXPad, SercomRXPad);
	void end();
	uint8_t transfer(uint8_t data) { return sercom->transferDataSPI(data); }
	uint16_t transfer16(uint16_t data);
	void transfer(void *buf, size_t count);
	
private:
	void config(SPISettings settings);

	SERCOM_SPI *sercom;
};
