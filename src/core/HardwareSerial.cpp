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

#include "HardwareSerial.h"
#include "GPIO.h"



void HardwareSerial::begin(uint32_t baudrate, SERCOM *scm, uint16_t pinRX, uint16_t pinTX, SercomRXPad padRX, SercomUartTXPad padTX, uint16_t config /*= SERIAL_8N1*/)
{
	sercom = reinterpret_cast<SERCOM_USART *>(scm->base);
	scm->bindIRQ(this);
	scm->initClockNVIC();
	
	sercom->initUART(SAMPLE_RATE_x16, baudrate);
	sercom->initFrame(extractCharSize(config), SERCOM_LSBFIRST, extractParity(config), extractNbStopBit(config));
	sercom->initPads(padTX, padRX);
	PortMultiplex(pinRX & 0xFF, pinRX >> 8);
	PortMultiplex(pinTX & 0xFF, pinTX >> 8);
	
	sercom->enableUART();
}


void HardwareSerial::end()
{
	sercom->resetUART();
	rxBuffer.clear();
	txBuffer.clear();
}


void HardwareSerial::flush()
{
	while(txBuffer.available()); // wait until TX buffer is empty
	sercom->flushUART();
}


void HardwareSerial::IRQHandler()
{
	if (sercom->availableDataUART())
	{
		rxBuffer.store_char(sercom->readDataUART());
	}
	
	if (sercom->isDataRegisterEmptyUART())
	{
		if (txBuffer.available())
		{
			uint8_t data = txBuffer.read_char();

			sercom->writeDataUART(data);
		}
		else
		{
			sercom->disableDataRegisterEmptyInterruptUART();
		}
	}
	
	// TODO: if (sercom->isBufferOverflowErrorUART())
	// TODO: if (sercom->isFrameErrorUART())
	// TODO: if (sercom->isParityErrorUART())
	if (sercom->isUARTError())
	{
		sercom->acknowledgeUARTError();
		sercom->clearStatusUART();
	}
}


int HardwareSerial::available()
{
	return rxBuffer.available();
}


int HardwareSerial::availableForWrite()
{
	return txBuffer.availableForStore();
}


int HardwareSerial::peek()
{
	return rxBuffer.peek();
}


int HardwareSerial::read()
{
	return rxBuffer.read_char();
}


size_t HardwareSerial::write(const uint8_t data)
{
	if (sercom->isDataRegisterEmptyUART() && txBuffer.available() == 0)
	{
		sercom->writeDataUART(data);
	}
	else
	{
		while(txBuffer.isFull());	// polling until a spot opens up in the buffer
		txBuffer.store_char(data);
		sercom->enableDataRegisterEmptyInterruptUART();
	}
	
	return 1;
}


SercomNumberStopBit HardwareSerial::extractNbStopBit(uint16_t config)
{
  switch(config & HARDSER_STOP_BIT_MASK)
  {
    case HARDSER_STOP_BIT_1:
    default:
      return SERCOM_STOP_BIT_1;

    case HARDSER_STOP_BIT_2:
      return SERCOM_STOP_BITS_2;
  }
}


SercomUartCharSize HardwareSerial::extractCharSize(uint16_t config)
{
  switch(config & HARDSER_DATA_MASK)
  {
    case HARDSER_DATA_5:
      return UART_CHAR_SIZE_5_BITS;

    case HARDSER_DATA_6:
      return UART_CHAR_SIZE_6_BITS;

    case HARDSER_DATA_7:
      return UART_CHAR_SIZE_7_BITS;

    case HARDSER_DATA_8:
    default:
      return UART_CHAR_SIZE_8_BITS;

  }
}


SercomParityMode HardwareSerial::extractParity(uint16_t config)
{
  switch(config & HARDSER_PARITY_MASK)
  {
    case HARDSER_PARITY_NONE:
    default:
      return SERCOM_NO_PARITY;

    case HARDSER_PARITY_EVEN:
      return SERCOM_EVEN_PARITY;

    case HARDSER_PARITY_ODD:
      return SERCOM_ODD_PARITY;
  }
}
