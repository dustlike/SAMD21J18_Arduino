/*
 * GPIO.h
 *
 * Created: 2018/5/4 上午 09:36:51
 *  Author: user
 */ 


#ifndef GPIO_H_
#define GPIO_H_

#include "wiring_constants.h"

void digitalWrite(uint32_t pinName, bool level);
inline void digitalWrite(uint32_t pinName, int level)	//相容於LOW=0, HIGH=1的舊式定義.
{
	digitalWrite(pinName, level==LOW? false : true);
}
bool digitalRead(uint32_t pinName);
void pinMode(uint32_t pinName, int mode);
void pinSet(uint32_t pinName);
void pinClr(uint32_t pinName);
void pinToggle(uint32_t pinName);
void pinMultiplex(uint32_t pinName, int pinMux);

// 直接回傳原始值，故其解析度由使用者自行判斷
uint16_t analogRead(uint32_t pin_name);



#endif /* GPIO_H_ */