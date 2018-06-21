/*
 * GPIO.cpp
 *
 * Created: 2018/5/4 上午 09:37:05
 *  Author: user
 */ 


#include "Arduino.h"



void digitalWrite(uint32_t pinName, bool level)
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


bool digitalRead(uint32_t pinName)
{
	uint32_t grp = pinName >> 5;
	
	if (grp >= PORT_GROUPS) return false;
	
	uint32_t pin = pinName & 0x1F;
	
	return PORT->Group[grp].IN.reg & (1 << pin)? true : false;
}


void pinMode(uint32_t pinName, int mode)
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
	}
}


void pinSet(uint32_t pinName)
{
	uint32_t grp = pinName >> 5;
	
	if (grp >= PORT_GROUPS) return;
	
	uint32_t pin = pinName & 0x1F;
	
	PORT->Group[grp].OUTSET.reg = 1 << pin;
}


void pinClr(uint32_t pinName)
{
	uint32_t grp = pinName >> 5;
	
	if (grp >= PORT_GROUPS) return;
	
	uint32_t pin = pinName & 0x1F;
	
	PORT->Group[grp].OUTCLR.reg = 1 << pin;
}


void pinToggle(uint32_t pinName)
{
	uint32_t grp = pinName >> 5;
	
	if (grp >= PORT_GROUPS) return;
	
	uint32_t pin = pinName & 0x1F;
	
	PORT->Group[grp].OUTTGL.reg = 1 << pin;
}


void pinMultiplex(uint32_t pinName, int pinMux)
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


uint16_t analogRead(uint32_t pin_name)
{
	if (pin_name >= MAX_PIN_NUMBER) return 65535;
	
	AdcChannel channel = AdcChannelMapping[pin_name];
	if (channel == No_ADC_Channel) return 65535;
	
	pinMultiplex(pin_name, PIO_ANALOG);
	
	//select ADC channel
	while (ADC->STATUS.bit.SYNCBUSY);
	ADC->INPUTCTRL.bit.MUXPOS = (int)channel;
	
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
