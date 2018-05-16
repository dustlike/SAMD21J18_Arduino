/*
 * GPIO.cpp
 *
 * Created: 2018/5/4 上午 09:37:05
 *  Author: user
 */ 


#include "Arduino.h"



void PortWrite(uint32_t pinName, bool level)
{
	uint32_t grp = pinName >> 5;
	
	if (grp >= PORT_GROUPS) return;
	
	uint32_t pin = pinName & 0x1F;
	
	if (level)
	{
		PORT->Group[grp].OUTSET.reg = 1 << pin;
	}
	else
	{
		PORT->Group[grp].OUTCLR.reg = 1 << pin;
	}
}


bool PortRead(uint32_t pinName)
{
	uint32_t grp = pinName >> 5;
	
	if (grp >= PORT_GROUPS) return false;
	
	uint32_t pin = pinName & 0x1F;
	
	return PORT->Group[grp].IN.reg & (1 << pin)? true : false;
}


void PortMode(uint32_t pinName, int mode)
{
	uint32_t grp = pinName >> 5;
	
	if (grp >= PORT_GROUPS) return;
	
	uint32_t pin = pinName & 0x1F;
	
	switch(mode)
	{
		case OUTPUT:
		PORT->Group[grp].PINCFG[pin].reg = 0;
		PORT->Group[grp].DIRSET.reg = 1 << pin;
		break;
		
		case INPUT:
		PORT->Group[grp].PINCFG[pin].reg = PORT_PINCFG_INEN;
		PORT->Group[grp].DIRCLR.reg = 1 << pin;
		break;
		
		case INPUT_PULLDOWN:
		PORT->Group[grp].PINCFG[pin].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
		PORT->Group[grp].DIRCLR.reg = 1 << pin;
		PORT->Group[grp].OUTCLR.reg = 1 << pin;
		break;
		
		case INPUT_PULLUP:
		PORT->Group[grp].PINCFG[pin].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
		PORT->Group[grp].DIRCLR.reg = 1 << pin;
		PORT->Group[grp].OUTSET.reg = 1 << pin;
		break;
		
		case DISABLE:
		PORT->Group[grp].PINCFG[pin].reg = 0;
		PORT->Group[grp].DIRCLR.reg = 1 << pin;
		break;
		
		case DISABLE_PULLDN:
		PORT->Group[grp].PINCFG[pin].reg = PORT_PINCFG_PULLEN;
		PORT->Group[grp].DIRCLR.reg = 1 << pin;
		PORT->Group[grp].OUTCLR.reg = 1 << pin;
		break;
		
		case DISABLE_PULLUP:
		PORT->Group[grp].PINCFG[pin].reg = PORT_PINCFG_PULLEN;
		PORT->Group[grp].DIRCLR.reg = 1 << pin;
		PORT->Group[grp].OUTSET.reg = 1 << pin;
		break;
		
		default:
		return;
	}
}


void PortSet(uint32_t pinName)
{
	uint32_t grp = pinName >> 5;
	
	if (grp >= PORT_GROUPS) return;
	
	uint32_t pin = pinName & 0x1F;
	
	PORT->Group[grp].OUTSET.reg = 1 << pin;
}


void PortClear(uint32_t pinName)
{
	uint32_t grp = pinName >> 5;
	
	if (grp >= PORT_GROUPS) return;
	
	uint32_t pin = pinName & 0x1F;
	
	PORT->Group[grp].OUTCLR.reg = 1 << pin;
}


void PortToggle(uint32_t pinName)
{
	uint32_t grp = pinName >> 5;
	
	if (grp >= PORT_GROUPS) return;
	
	uint32_t pin = pinName & 0x1F;
	
	PORT->Group[grp].OUTTGL.reg = 1 << pin;
}


void PortMultiplex(uint32_t pinName, int pinMux)
{
	uint32_t grp = pinName >> 5;
	
	if (grp >= PORT_GROUPS) return;
	
	uint32_t pin = pinName & 0x1F;
	
	//workaround for Errata 12368
	if (pinName == 24 || pinName == 25)
	{
		//PULL won't be disabled automatically when PA24 and PA25 is configured as alternative function
		PORT->Group[0].PINCFG[pin].bit.INEN = 0;
	}
	
	if (pinName & 1)	//pin is odd number
	{
		PORT->Group[grp].PMUX[pin >> 1].bit.PMUXO = pinMux;
	}
	else
	{
		PORT->Group[grp].PMUX[pin >> 1].bit.PMUXE = pinMux;
	}
	
	PORT->Group[grp].PINCFG[pin].bit.PMUXEN = 1;
}


uint16_t ReadADC(uint32_t pin_name)
{
	PortMultiplex(pin_name & 0xFF, PIO_ANALOG);
	
	//select ADC channel
	while (ADC->STATUS.bit.SYNCBUSY);
	ADC->INPUTCTRL.bit.MUXPOS = pin_name >> 8;
	
	//enable ADC
	while (ADC->STATUS.bit.SYNCBUSY);
	ADC->CTRLA.bit.ENABLE = 1;
	
	//start conversion
	while (ADC->STATUS.bit.SYNCBUSY);
	ADC->INTFLAG.bit.RESRDY = 1;	//ensure the ready flag is clear
	ADC->SWTRIG.bit.START = 1;
	
	//wait conversion...
	while (!ADC->INTFLAG.bit.RESRDY);
	
	uint16_t result = ADC->RESULT.reg;
	
	//disable ADC
	ADC->CTRLA.bit.ENABLE = 0;
	while (ADC->STATUS.bit.SYNCBUSY);
	
	return result;
}
