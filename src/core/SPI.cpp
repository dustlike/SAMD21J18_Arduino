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


#include <Arduino.h>



void SerialPerIface::begin(SERCOM *scm, uint32_t freqSCK, BitOrder bitOrder, SercomSpiClockMode clockMode, uint16_t pinMISO, uint16_t pinSCK, uint16_t pinMOSI, SercomRXPad padRX, SercomSpiTXPad padTX)
{
	SercomDataOrder sdo = ((bitOrder == MSBFIRST)?  SERCOM_MSBFIRST : SERCOM_LSBFIRST);
	
	sercom = reinterpret_cast<SERCOM_SPI *>(scm->base);
	scm->bindIRQ();		//SerialPerIface目前還暫時不依賴IRQ處理來接收資料.
	scm->initClockNVIC();
	
	pinMultiplex(pinMISO & 0xFF, pinMISO >> 8);
	pinMultiplex(pinSCK  & 0xFF, pinSCK  >> 8);
	pinMultiplex(pinMOSI & 0xFF, pinMOSI >> 8);
	
	sercom->initSPI(padTX, padRX, SPI_CHAR_SIZE_8_BITS, sdo);
	sercom->initSPIClock(clockMode, freqSCK);
	sercom->enableSPI();
}


void SerialPerIface::end()
{
	sercom->resetSPI();
}


uint16_t SerialPerIface::transfer16(uint16_t data)
{
	union { uint16_t val;  struct { uint8_t lsb; uint8_t msb; }; } t;

	t.val = data;

	if (sercom->getDataOrderSPI() == SERCOM_LSBFIRST)
	{
		t.lsb = transfer(t.lsb);
		t.msb = transfer(t.msb);
	}
	else
	{
		t.msb = transfer(t.msb);
		t.lsb = transfer(t.lsb);
	}
	
	return t.val;
}


void SerialPerIface::transfer(void *buf, size_t count)
{
	uint8_t *buffer = reinterpret_cast<uint8_t *>(buf);
	
	for (size_t i = 0; i < count; i++)
	{
		*buffer = transfer(*buffer);
		buffer++;
	}
}
