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


#include <Arduino.h>


TwoWire Wire;



TwoWire::TwoWire()
{
	master_mode_clock = DEFAULT_TWI_CLOCK;
	transmissionBegun = false;
}


void TwoWire::begin(SERCOM *scm, uint16_t pinSDA, uint16_t pinSCL)
{
	//Master Mode
	
	sercom = reinterpret_cast<SERCOM_I2C *>(scm->base);
	scm->bindIRQ(this);
	scm->initClockNVIC();
	
	pinMultiplex(pinSDA & 0xFF, pinSDA >> 8);
	pinMultiplex(pinSCL & 0xFF, pinSCL >> 8);
	
	sercom->initMasterWIRE(master_mode_clock);
	sercom->enableWIRE();
}


void TwoWire::begin(uint8_t address, SERCOM *scm, uint16_t pinSDA, uint16_t pinSCL)
{
	//Slave mode
	
	sercom = reinterpret_cast<SERCOM_I2C *>(scm->base);
	scm->bindIRQ(this);
	scm->initClockNVIC();
	
	pinMultiplex(pinSDA & 0xFF, pinSDA >> 8);
	pinMultiplex(pinSCL & 0xFF, pinSCL >> 8);
	
	sercom->initSlaveWIRE(address);
	sercom->enableWIRE();
}


void TwoWire::changeClock(uint32_t baudrate)
{
	master_mode_clock = baudrate;
}


void TwoWire::end()
{
	sercom->disableWIRE();
}


uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
	if(quantity == 0)
	{
		return 0;
	}

	size_t byteRead = 0;

	rxBuffer.clear();

	if(sercom->startTransmissionWIRE(address, WIRE_READ_FLAG))
	{
		// Read first data
		rxBuffer.store_char(sercom->readDataWIRE());

		// Connected to slave
		for (byteRead = 1; byteRead < quantity; ++byteRead)
		{
			sercom->prepareAckBitWIRE();                          // Prepare Acknowledge
			sercom->prepareCommandBitsWire(WIRE_MASTER_ACT_READ); // Prepare the ACK command for the slave
			rxBuffer.store_char(sercom->readDataWIRE());          // Read data and send the ACK
		}
		sercom->prepareNackBitWIRE();	 // Prepare NACK to stop slave transmission

		if (stopBit)
		{
			sercom->prepareCommandBitsWire(WIRE_MASTER_ACT_STOP);   // Send Stop
		}
	}

	return byteRead;
}


void TwoWire::beginTransmission(uint8_t address)
{
	// save address of target and clear buffer
	txAddress = address;
	txBuffer.clear();

	transmissionBegun = true;
}


// Errors:
//  0 : Success
//  1 : Data too long
//  2 : NACK on transmit of address
//  3 : NACK on transmit of data
//  4 : Other error
uint8_t TwoWire::endTransmission(bool stopBit)
{
	transmissionBegun = false ;

	// Start I2C transmission
	if ( !sercom->startTransmissionWIRE( txAddress, WIRE_WRITE_FLAG ) )
	{
		sercom->prepareCommandBitsWire(WIRE_MASTER_ACT_STOP);
		return 2 ;  // Address error
	}

	// Send all buffer
	while( txBuffer.available() )
	{
		// Trying to send data
		if ( !sercom->sendDataMasterWIRE( txBuffer.read_char() ) )
		{
			sercom->prepareCommandBitsWire(WIRE_MASTER_ACT_STOP);
			return 3 ;  // Nack or error
		}
	}
  
	if (stopBit)
	{
		sercom->prepareCommandBitsWire(WIRE_MASTER_ACT_STOP);
	}   

	return 0;
}


size_t TwoWire::write(uint8_t ucData)
{
	// No writing, without begun transmission or a full buffer
	if (!transmissionBegun || txBuffer.isFull())
	{
		return 0 ;
	}

	txBuffer.store_char(ucData) ;

	return 1 ;
}


size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
	//Try to store all data
	for(size_t i = 0; i < quantity; ++i)
	{
		//Return the number of data stored, when the buffer is full (if write return 0)
		if(!write(data[i]))
			return i;
	}
	
	//All data stored
	return quantity;
}


int TwoWire::available()
{
	return rxBuffer.available();
}


int TwoWire::read()
{
	return rxBuffer.read_char();
}


int TwoWire::peek()
{
	return rxBuffer.peek();
}


void TwoWire::flush()
{
	// Do nothing, you should use endTransmission(..) to force data transfer.
}


void TwoWire::onReceive(void(*function)(int))
{
	onReceiveCallback = function;
}


void TwoWire::onRequest(void(*function)(void))
{
	onRequestCallback = function;
}


void TwoWire::IRQHandler()
{
	if ( sercom->isSlaveWIRE() )
	{
		if(sercom->isStopDetectedWIRE() ||
			//Stop or Restart detected
			(sercom->isAddressMatch() && sercom->isRestartDetectedWIRE() && !sercom->isMasterReadOperationWIRE()))
		{
			sercom->prepareAckBitWIRE();
			sercom->prepareCommandBitsWire(0x03);

			//Calling onReceiveCallback, if exists
			if(onReceiveCallback)
			{
				onReceiveCallback(available());
			}
			
			rxBuffer.clear();
		}
		else if(sercom->isAddressMatch())	//Address Match
		{
			sercom->prepareAckBitWIRE();
			sercom->prepareCommandBitsWire(0x03);

			if(sercom->isMasterReadOperationWIRE())	//Is a request ?
			{
				txBuffer.clear();

				transmissionBegun = true;

				//Calling onRequestCallback, if exists
				if(onRequestCallback)
				{
					onRequestCallback();
				}
			}
		}
		else if(sercom->isDataReadyWIRE())
		{
			if (sercom->isMasterReadOperationWIRE())
			{
				uint8_t c = 0xff;
				if(txBuffer.available()) c = txBuffer.read_char();
				transmissionBegun = sercom->sendDataSlaveWIRE(c);
			}
			else
			{
				if (rxBuffer.isFull())
				{
					sercom->prepareNackBitWIRE();
				}
				else
				{
					rxBuffer.store_char(sercom->readDataWIRE());
					sercom->prepareAckBitWIRE();
				}
				
				sercom->prepareCommandBitsWire(0x03);
			}
		}
	}
}
