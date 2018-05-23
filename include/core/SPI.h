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

#include "Stream.h"
#include "SERCOM.h"
#include "RingBuffer.h"


class SerialPerIface
{
public:
	
	// sercom物件, SPI時鐘頻率, 位元順序, CPHA與CPOL設定
	// MISO腳位, SCLK腳位, MOSI腳位, 接收腳位選擇, SPI腳位選擇
	// 腳位格式： (bit0~7)PIN_Pxyy   (bit8~15)PIO_SERCOM / PIO_SERCOM_ALT
	void begin(SERCOM *, uint32_t, BitOrder, SercomSpiClockMode, uint16_t, uint16_t, uint16_t, SercomRXPad, SercomSpiTXPad);
	
	void end();
	uint8_t transfer(uint8_t data) { return sercom->transferDataSPI(data); }
	uint16_t transfer16(uint16_t data);
	void transfer(void *buf, size_t count);
	
private:
	SERCOM_SPI *sercom;
};
