/*
  Copyright (c) 2014 Arduino.  All right reserved.

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

#include "Arduino.h"



void SERCOM::initClockNVIC()
{
	//initialize IRQ
	NVIC_SetPriority(idNVIC, (1 << __NVIC_PRIO_BITS) - 1);	//lowest priority level
	NVIC_EnableIRQ(idNVIC);
	
	//use Generator 0 (same as CPU clock) as clock source
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(idGCLK) | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN;
	while(GCLK->STATUS.bit.SYNCBUSY);
}



//////////////////////////////////////////////////////////////////////////
//		Sercom USART
//////////////////////////////////////////////////////////////////////////


void SERCOM_USART::initUART(SercomUartSampleRate sampleRate, uint32_t baudrate)
{
	resetUART();
	CTRLA.reg = SERCOM_USART_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_CTRLA_SAMPR(sampleRate);
	
	//enable "Receive Complete" and "Error" interrupt
	INTENSET.reg = SERCOM_USART_INTENSET_RXC | SERCOM_USART_INTENSET_ERROR;
	
	uint16_t sampleRateValue = (sampleRate == SAMPLE_RATE_x16)? 16 : 8;
	
	// Asynchronous fractional mode (Table 24-2 in datasheet)
	//   BAUD = fref / (sampleRateValue * fbaud)
	// (multiply by 8, to calculate fractional piece)
	uint32_t baudTimes8 = (SystemCoreClock * 8) / (sampleRateValue * baudrate);
	
	BAUD.FRAC.FP = baudTimes8 & 7;
	BAUD.FRAC.BAUD = baudTimes8 >> 3;
}


void SERCOM_USART::initFrame(SercomUartCharSize charSize, SercomDataOrder dataOrder, SercomParityMode parityMode, SercomNumberStopBit nbStopBits)
{
	//frame format(auto-baud and parity), bit order
	CTRLA.reg |= SERCOM_USART_CTRLA_FORM( (parityMode == SERCOM_NO_PARITY ? 0 : 1) ) | dataOrder << SERCOM_USART_CTRLA_DORD_Pos;
	
	//character size, stop bit length, parity mode
	CTRLB.reg |= SERCOM_USART_CTRLB_CHSIZE(charSize) | 
		nbStopBits << SERCOM_USART_CTRLB_SBMODE_Pos |
		(parityMode == SERCOM_NO_PARITY ? 0 : parityMode) << SERCOM_USART_CTRLB_PMODE_Pos;
}


void SERCOM_USART::initPads(SercomUartTXPad txPad, SercomRXPad rxPad)
{
	//set Transmit Data Pinout & Receive Data Pinout
	CTRLA.reg |= SERCOM_USART_CTRLA_TXPO(txPad) | SERCOM_USART_CTRLA_RXPO(rxPad);
	
	//enable TX & RX
	CTRLB.reg |= SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_RXEN;
}


void SERCOM_USART::resetUART()
{
	CTRLA.bit.SWRST = 1;
	while (SYNCBUSY.bit.SWRST);		//wait for software reset finished
}


void SERCOM_USART::enableUART()
{
	CTRLA.bit.ENABLE = 1;
	while (SYNCBUSY.bit.ENABLE);	//wait for enable finished
}


void SERCOM_USART::flushUART()
{
	// Skip checking transmission completion if data register is empty
	if (isDataRegisterEmptyUART()) return;
	
	// Wait for transmission to complete
	while (!INTFLAG.bit.TXC);
}


int SERCOM_USART::writeDataUART(uint8_t data)
{
	// Wait for data register to be empty
	while(!isDataRegisterEmptyUART());

	//Put data into DATA register
	DATA.reg = (uint16_t)data;
	return 1;
}



//////////////////////////////////////////////////////////////////////////
//		Sercom I2C
//////////////////////////////////////////////////////////////////////////

void SERCOM_I2C::resetWIRE()
{
	//Master and Slave mode have the same SWRST bit
	
	((Sercom *)this)->I2CM.CTRLA.bit.SWRST = 1;
	while (((Sercom *)this)->I2CM.SYNCBUSY.bit.SWRST);
}


void SERCOM_I2C::enableWIRE()
{
	//Master and Slave mode have the same ENABLE bit
	
	((Sercom *)this)->I2CM.CTRLA.bit.ENABLE = 1;
	while (((Sercom *)this)->I2CM.SYNCBUSY.bit.ENABLE);
	
	// Setting bus idle mode
	((Sercom *)this)->I2CM.STATUS.bit.BUSSTATE = 1;
	while (((Sercom *)this)->I2CM.SYNCBUSY.bit.SYSOP);
}


void SERCOM_I2C::disableWIRE()
{
	//Master and Slave mode have the same ENABLE bit
	
	((Sercom *)this)->I2CM.CTRLA.bit.ENABLE = 0;
	while (((Sercom *)this)->I2CM.SYNCBUSY.bit.ENABLE);
}


void SERCOM_I2C::initSlaveWIRE(uint8_t ucAddress)
{
	resetWIRE();
	
	//assigned as slave mode
	((Sercom *)this)->I2CS.CTRLA.bit.MODE = I2C_SLAVE_OPERATION;
	
	//set slave address and no address mask
	((Sercom *)this)->I2CS.ADDR.reg = SERCOM_I2CS_ADDR_ADDR(ucAddress & 0x7F) | SERCOM_I2CS_ADDR_ADDRMASK(0);
	
	//enable "Stop Received" "Address Match" "Data Ready" interrupt
	((Sercom *)this)->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_PREC | SERCOM_I2CS_INTENSET_AMATCH | SERCOM_I2CS_INTENSET_DRDY;
	
	while (((Sercom *)this)->I2CM.SYNCBUSY.bit.SYSOP);
}


void SERCOM_I2C::initMasterWIRE(uint32_t baudrate)
{
	resetWIRE();
	
	//assigned as master mode
	((Sercom *)this)->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_MODE( I2C_MASTER_OPERATION );
	
	((Sercom *)this)->I2CM.BAUD.bit.BAUD = SystemCoreClock / (2 * baudrate) - 1;
}


bool SERCOM_I2C::startTransmissionWIRE(uint8_t address, SercomWireReadWriteFlag flag)
{
	uint32_t start = millis();
	
	// 7-bits address + 1-bits R/W
	address = (address << 1) | flag;
	
	// Wait idle or owner bus mode
	while ( !isBusIdleWIRE() && !isBusOwnerWIRE() )
	{
		if (millis() - start > 1000)
		{
			return false;
		}
	}
	
	// Send start and address
	((Sercom *)this)->I2CM.ADDR.bit.ADDR = address;

	// Address Transmitted
	if ( flag == WIRE_WRITE_FLAG ) // Write mode
	{
		while(!((Sercom *)this)->I2CM.INTFLAG.bit.MB)
		{
			if (millis() - start > 1000) return false;
		}
	}
	else  // Read mode
	{
		while( !((Sercom *)this)->I2CM.INTFLAG.bit.SB )
		{
			// If the slave NACKS the address, the MB bit will be set.
			// In that case, send a stop condition and return false.
			if (((Sercom *)this)->I2CM.INTFLAG.bit.MB)
			{
				((Sercom *)this)->I2CM.CTRLB.bit.CMD = 3; // Stop condition
				return false;
			}
			
			if (millis() - start > 1000) return false;
		}
	}
	
	//ACK received (0: ACK, 1: NACK)
	if (((Sercom *)this)->I2CM.STATUS.bit.RXNACK)
	{
		return false;
	}
	else
	{
		return true;
	}
}


bool SERCOM_I2C::sendDataMasterWIRE(uint8_t data)
{
	uint32_t start = millis();
	
	//Send data
	((Sercom *)this)->I2CM.DATA.bit.DATA = data;

	//Wait transmission successful
	while(!((Sercom *)this)->I2CM.INTFLAG.bit.MB)
	{
		// If a bus error occurs, the MB bit may never be set.
		// Check the bus error bit and bail if it's set.
		if (((Sercom *)this)->I2CM.STATUS.bit.BUSERR) return false;
		
		if (millis() - start > 1000) return false;
	}

	//Problems on line? nack received?
	if(((Sercom *)this)->I2CM.STATUS.bit.RXNACK)
	{
		return false;
	}
	else
	{
		return true;
	}
}


bool SERCOM_I2C::sendDataSlaveWIRE(uint8_t data)
{
	//Send data
	((Sercom *)this)->I2CS.DATA.bit.DATA = data;

	//Problems on line? nack received?
	if(!((Sercom *)this)->I2CS.INTFLAG.bit.DRDY || ((Sercom *)this)->I2CS.STATUS.bit.RXNACK)
	{
		return false;
	}
	else
	{
		return true;
	}
}


uint8_t SERCOM_I2C::readDataWIRE( void )
{
	uint32_t start = millis();
	
	if(isMasterWIRE())
	{
		while(!((Sercom *)this)->I2CM.INTFLAG.bit.SB)
		{
			if (millis() - start > 1000) return false;
		}
		
		return ((Sercom *)this)->I2CM.DATA.reg;
	}
	else
	{
		return ((Sercom *)this)->I2CS.DATA.reg;
	}
}


void SERCOM_I2C::prepareCommandBitsWire(uint8_t cmd)
{
	if(isMasterWIRE())
	{
		((Sercom *)this)->I2CM.CTRLB.bit.CMD = cmd;
		while(((Sercom *)this)->I2CM.SYNCBUSY.bit.SYSOP);
	}
	else
	{
		((Sercom *)this)->I2CS.CTRLB.bit.CMD = cmd;
	}
}



//////////////////////////////////////////////////////////////////////////
//		Sercom SPI
//////////////////////////////////////////////////////////////////////////


void SERCOM_SPI::initSPI(SercomSpiTXPad mosi, SercomRXPad miso, SercomSpiCharSize charSize, SercomDataOrder dataOrder)
{
	resetSPI();
	
	//SPI master, MOSI pinout, MISO pinout, Data bit order
	CTRLA.reg = SERCOM_SPI_CTRLA_MODE_SPI_MASTER | SERCOM_SPI_CTRLA_DOPO(mosi) | SERCOM_SPI_CTRLA_DIPO(miso) | dataOrder << SERCOM_SPI_CTRLA_DORD_Pos;
	
	//character size, enable receive
	CTRLB.reg = SERCOM_SPI_CTRLB_CHSIZE(charSize) | SERCOM_SPI_CTRLB_RXEN;
}


void SERCOM_SPI::initSPIClock(SercomSpiClockMode clockMode, uint32_t baudrate)
{
	//setting PHASE
	if (clockMode & 1) CTRLA.reg |= SERCOM_SPI_CTRLA_CPHA;
	
	//setting POLARITY
	if (clockMode & 2) CTRLA.reg |= SERCOM_SPI_CTRLA_CPOL;
	
	BAUD.reg = calculateBaudrateSynchronous(baudrate);
}


void SERCOM_SPI::resetSPI()
{
	CTRLA.bit.SWRST = 1;
	while (SYNCBUSY.bit.SWRST);
}


void SERCOM_SPI::enableSPI()
{
	CTRLA.bit.ENABLE = 1;
	while (SYNCBUSY.bit.ENABLE);
}


void SERCOM_SPI::disableSPI()
{
	CTRLA.bit.ENABLE = 0;
	while (SYNCBUSY.bit.ENABLE);
}


uint8_t SERCOM_SPI::transferDataSPI(uint8_t data)
{
	DATA.reg = data;	//transmit data
	while (!INTFLAG.bit.RXC);
	
	return DATA.reg;	//return received data
}


uint8_t SERCOM_SPI::calculateBaudrateSynchronous(uint32_t baudrate)
{
	return SystemCoreClock / (2 * baudrate) - 1;
}
