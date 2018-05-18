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


#pragma once


#include "sam.h"


typedef enum
{
	SPI_SLAVE_OPERATION = 0x2u,
	SPI_MASTER_OPERATION = 0x3u
} SercomSpiMode;

typedef enum
{
	I2C_SLAVE_OPERATION = 0x4u,
	I2C_MASTER_OPERATION = 0x5u
} SercomI2CMode;

typedef enum
{
	SERCOM_EVEN_PARITY = 0,
	SERCOM_ODD_PARITY,
	SERCOM_NO_PARITY
} SercomParityMode;

typedef enum
{
	SERCOM_STOP_BIT_1 = 0,
	SERCOM_STOP_BITS_2
} SercomNumberStopBit;

typedef enum
{
	MSB_FIRST = 0,
	LSB_FIRST
} SercomDataOrder;

typedef enum
{
	UART_CHAR_SIZE_8_BITS = 0,
	UART_CHAR_SIZE_9_BITS,
	UART_CHAR_SIZE_5_BITS = 0x5u,
	UART_CHAR_SIZE_6_BITS,
	UART_CHAR_SIZE_7_BITS
} SercomUartCharSize;

typedef enum
{
	SERCOM_RX_PAD_0 = 0,
	SERCOM_RX_PAD_1,
	SERCOM_RX_PAD_2,
	SERCOM_RX_PAD_3
} SercomRXPad;

typedef enum
{
	UART_TX_PAD_0 = 0x0ul,	// Only for UART
	UART_TX_PAD_2 = 0x1ul,  // Only for UART
	UART_TX_RTS_CTS_PAD_0_2_3 = 0x2ul,  // Only for UART with TX on PAD0, RTS on PAD2 and CTS on PAD3
} SercomUartTXPad;

typedef enum
{
	SAMPLE_RATE_x16 = 0x1,	//Fractional
	SAMPLE_RATE_x8 = 0x3,	//Fractional
} SercomUartSampleRate;

typedef enum
{
	SPI_MODE_0 = 0,	// CPOL : 0  | CPHA : 0
	SPI_MODE_1,		// CPOL : 0  | CPHA : 1
	SPI_MODE_2,		// CPOL : 1  | CPHA : 0
	SPI_MODE_3		// CPOL : 1  | CPHA : 1
} SercomSpiClockMode;

typedef enum
{
	SPI_PAD_0_SCK_1 = 0,
	SPI_PAD_2_SCK_3,
	SPI_PAD_3_SCK_1,
	SPI_PAD_0_SCK_3
} SercomSpiTXPad;

typedef enum
{
	SPI_CHAR_SIZE_8_BITS = 0x0ul,
	SPI_CHAR_SIZE_9_BITS
} SercomSpiCharSize;

typedef enum
{
	WIRE_UNKNOWN_STATE = 0x0ul,
	WIRE_IDLE_STATE,
	WIRE_OWNER_STATE,
	WIRE_BUSY_STATE
} SercomWireBusState;

typedef enum
{
	WIRE_WRITE_FLAG = 0x0ul,
	WIRE_READ_FLAG
} SercomWireReadWriteFlag;

typedef enum
{
	WIRE_MASTER_ACT_NO_ACTION = 0,
	WIRE_MASTER_ACT_REPEAT_START,
	WIRE_MASTER_ACT_READ,
	WIRE_MASTER_ACT_STOP
} SercomMasterCommandWire;

typedef enum
{
	WIRE_MASTER_ACK_ACTION = 0,
	WIRE_MASTER_NACK_ACTION
} SercomMasterAckActionWire;



class ISercomIRQ
{
public:
	virtual void IRQHandler() = 0;
};


struct SERCOM
{
	SERCOM(Sercom* s, IRQn_Type nvic_id, uint8_t gclk_id)
		: base(s), idNVIC(nvic_id), idGCLK(gclk_id), bindObj(nullptr)
	{
	}
	
	void handleIRQ()
	{
		if (bindObj) bindObj->IRQHandler();
	}
	
	//invoke bindIRQ() will unbind
	void bindIRQ(ISercomIRQ *obj = nullptr)
	{
		bindObj = obj;
	}
	
	void initClockNVIC();
	
	Sercom *base;
	ISercomIRQ *bindObj;
	IRQn_Type idNVIC;
	uint8_t idGCLK;
};



class SERCOM_USART : public SercomUsart
{
	public:
	
	void initUART(SercomUartSampleRate sampleRate, uint32_t baudrate=0) ;
	void initFrame(SercomUartCharSize charSize, SercomDataOrder dataOrder, SercomParityMode parityMode, SercomNumberStopBit nbStopBits) ;
	void initPads(SercomUartTXPad txPad, SercomRXPad rxPad) ;

	void resetUART();
	void enableUART();
	void flushUART();
	int writeDataUART(uint8_t data);
	
	void clearStatusUART() { STATUS.reg = SERCOM_USART_STATUS_RESETVALUE; }
	bool availableDataUART() { return INTFLAG.bit.RXC; }
	bool isBufferOverflowErrorUART() { return STATUS.bit.BUFOVF; }
	bool isFrameErrorUART() { return STATUS.bit.FERR; }
	bool isParityErrorUART() { return STATUS.bit.PERR; }
	bool isDataRegisterEmptyUART() { return INTFLAG.bit.DRE; }
	uint8_t readDataUART() { return DATA.reg; }
	bool isUARTError() { return INTFLAG.bit.ERROR; }
	void acknowledgeUARTError() { INTFLAG.bit.ERROR = 1; }
	void enableDataRegisterEmptyInterruptUART() { INTENSET.bit.DRE = 1; }
	void disableDataRegisterEmptyInterruptUART() { INTENCLR.bit.DRE = 1; }
};


class SERCOM_I2C
{
public:
	void initSlaveWIRE(uint8_t address);
	void initMasterWIRE(uint32_t baudrate);
	void resetWIRE();
	void enableWIRE();
	void disableWIRE();
	bool startTransmissionWIRE(uint8_t address, SercomWireReadWriteFlag flag);
	bool sendDataMasterWIRE(uint8_t data);
	bool sendDataSlaveWIRE(uint8_t data);
	uint8_t readDataWIRE();
	void prepareCommandBitsWire(uint8_t cmd);
	
	void prepareNackBitWIRE() {
		if(isMasterWIRE()) ((Sercom *)this)->I2CM.CTRLB.bit.ACKACT = 1;
		else ((Sercom *)this)->I2CS.CTRLB.bit.ACKACT = 1;
	}
	
	void prepareAckBitWIRE() {
		if(isMasterWIRE()) ((Sercom *)this)->I2CM.CTRLB.bit.ACKACT = 0;
		else ((Sercom *)this)->I2CS.CTRLB.bit.ACKACT = 0;
	}
	
	int availableWIRE() {
		if(isMasterWIRE()) return ((Sercom *)this)->I2CM.INTFLAG.bit.SB;
		else return ((Sercom *)this)->I2CS.INTFLAG.bit.DRDY;
	}
	
	bool isMasterWIRE() { return ((Sercom *)this)->I2CS.CTRLA.bit.MODE == I2C_MASTER_OPERATION; }
	bool isSlaveWIRE() { return ((Sercom *)this)->I2CS.CTRLA.bit.MODE == I2C_SLAVE_OPERATION; }
	bool isBusIdleWIRE() { return ((Sercom *)this)->I2CM.STATUS.bit.BUSSTATE == WIRE_IDLE_STATE; }
	bool isBusOwnerWIRE() { return ((Sercom *)this)->I2CM.STATUS.bit.BUSSTATE == WIRE_OWNER_STATE; }
	bool isDataReadyWIRE() { return ((Sercom *)this)->I2CS.INTFLAG.bit.DRDY; }
	bool isStopDetectedWIRE() { return ((Sercom *)this)->I2CS.INTFLAG.bit.PREC; }
	bool isRestartDetectedWIRE() { return ((Sercom *)this)->I2CS.STATUS.bit.SR; }
	bool isAddressMatch() { return ((Sercom *)this)->I2CS.INTFLAG.bit.AMATCH; }
	bool isMasterReadOperationWIRE() { return ((Sercom *)this)->I2CS.STATUS.bit.DIR; }
	bool isRXNackReceivedWIRE() { return ((Sercom *)this)->I2CM.STATUS.bit.RXNACK; }
};



class SERCOM_SPI: public SercomSpi
{
public:
	void initSPI(SercomSpiTXPad mosi, SercomRXPad miso, SercomSpiCharSize charSize, SercomDataOrder dataOrder) ;
	void initSPIClock(SercomSpiClockMode clockMode, uint32_t baudrate) ;
	void resetSPI();
	void enableSPI();
	void disableSPI();
	uint8_t transferDataSPI(uint8_t data);
	
	SercomDataOrder getDataOrderSPI() { return CTRLA.bit.DORD ? LSB_FIRST : MSB_FIRST; }
	bool isBufferOverflowErrorSPI() { return STATUS.bit.BUFOVF; }
	bool isDataRegisterEmptySPI() { return INTFLAG.bit.DRE; }
	bool isTransmitCompleteSPI() { return INTFLAG.bit.TXC; }
	bool isReceiveCompleteSPI() { return INTFLAG.bit.RXC; }
	
private:
	static uint8_t calculateBaudrateSynchronous(uint32_t baudrate);
};
