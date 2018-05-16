/*
 * GPIO.h
 *
 * Created: 2018/5/4 上午 09:36:51
 *  Author: user
 */ 


#ifndef GPIO_H_
#define GPIO_H_



void PortWrite(uint32_t pinName, bool level);
bool PortRead(uint32_t pinName);
void PortMode(uint32_t pinName, int mode);
void PortSet(uint32_t pinName);
void PortClear(uint32_t pinName);
void PortToggle(uint32_t pinName);
void PortMultiplex(uint32_t pinName, int pinMux);

// pin_name結構 0~7bit: PIN_Pxyy, 8~31bit: ADC Channel number
// 直接回傳原始值，故其解析度由使用者自行判斷
// 原版有要求丟棄在變更analog reference後第一次的ADC轉換值，由於詳細還沒看datasheet使用者先自行讀一次吧
uint16_t ReadADC(uint32_t pin_name);



#endif /* GPIO_H_ */