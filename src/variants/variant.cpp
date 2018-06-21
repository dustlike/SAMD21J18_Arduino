#include "variant.h"



const AdcChannel AdcChannelMapping[MAX_PIN_NUMBER] = 
{
	//PA00
	No_ADC_Channel,
	No_ADC_Channel,
	ADC_Channel0,
	ADC_Channel1,
	ADC_Channel4,
	ADC_Channel5,
	ADC_Channel6,
	ADC_Channel7,
	
	//PA08
	ADC_Channel16,
	ADC_Channel17,
	ADC_Channel18,
	ADC_Channel19,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	
	//PA16
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	
	//PA24
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	
	//PB00
	ADC_Channel8,
	ADC_Channel9,
	ADC_Channel10,
	ADC_Channel11,
	ADC_Channel12,
	ADC_Channel13,
	ADC_Channel14,
	ADC_Channel15,
	
	//PB08
	ADC_Channel2,
	ADC_Channel3,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	
	//PB16
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	
	//PB24
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
	No_ADC_Channel,
};


const void* g_apTCInstances[TCC_INST_NUM+TC_INST_NUM]={ TCC0, TCC1, TCC2, TC3, TC4, TC5 } ;


SERCOM sercom0 = SERCOM(SERCOM0, SERCOM0_IRQn, GCM_SERCOM0_CORE);
void SERCOM0_Handler()
{
	sercom0.handleIRQ();
}


SERCOM sercom1 = SERCOM(SERCOM1, SERCOM1_IRQn, GCM_SERCOM1_CORE);
void SERCOM1_Handler()
{
	sercom1.handleIRQ();
}


SERCOM sercom2 = SERCOM(SERCOM2, SERCOM2_IRQn, GCM_SERCOM2_CORE);
void SERCOM2_Handler()
{
	sercom2.handleIRQ();
}


SERCOM sercom3 = SERCOM(SERCOM3, SERCOM3_IRQn, GCM_SERCOM3_CORE);
void SERCOM3_Handler()
{
	sercom3.handleIRQ();
}


SERCOM sercom4 = SERCOM(SERCOM4, SERCOM4_IRQn, GCM_SERCOM4_CORE);
void SERCOM4_Handler()
{
	sercom4.handleIRQ();
}


SERCOM sercom5 = SERCOM(SERCOM5, SERCOM5_IRQn, GCM_SERCOM5_CORE);
void SERCOM5_Handler()
{
	sercom5.handleIRQ();
}



//////////////////////////////////////////////////////////////////////////
// Arduino PIN ¹ï·Óªí

/*
 *
 * + Pin number +  ZERO Board pin  |  PIN   | Label/Name      | Comments (* is for default peripheral in use)
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | Digital Low      |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 0          | 0 -> RX          |  PA11  |                 | EIC/EXTINT[11] ADC/AIN[19] PTC/X[3] *SERCOM0/PAD[3] SERCOM2/PAD[3] TCC1/WO[1] TCC0/WO[3]
 * | 1          | 1 <- TX          |  PA10  |                 | EIC/EXTINT[10] ADC/AIN[18] PTC/X[2] *SERCOM0/PAD[2] TCC1/WO[0] TCC0/WO[2]  
 * | 2          | ~2               |  PA08  |                 | EIC/NMI ADC/AIN[16] PTC/X[0] SERCOM0/PAD[0] SERCOM2/PAD[0] *TCC0/WO[0] TCC1/WO[2]
 * | 3          | ~3               |  PA09  |                 | EIC/EXTINT[9] ADC/AIN[17] PTC/X[1] SERCOM0/PAD[1] SERCOM2/PAD[1] *TCC0/WO[1] TCC1/WO[3]
 * | 4          | ~4               |  PA14  |                 | EIC/EXTINT[14] SERCOM2/PAD[2] SERCOM4/PAD[2] TC3/WO[0] *TCC0/WO[4]
 * | 5          | ~5               |  PA15  |                 | EIC/EXTINT[15] SERCOM2/PAD[3] SERCOM4/PAD[3] TC3/WO[1] *TCC0/WO[5]
 * | 6          | ~6               |  PA20  |                 | EIC/EXTINT[4] PTC/X[8] SERCOM5/PAD[2] SERCOM3/PAD[2] TC7/WO[0] *TCC0/WO[6]
 * | 7          | ~7               |  PA21  |                 | EIC/EXTINT[5] PTC/X[9] SERCOM5/PAD[3] SERCOM3/PAD[3] TC7/WO[1] *TCC0/WO[7]
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | Digital High     |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 8          | ~8               |  PA06  |                 | EIC/EXTINT[6] PTC/Y[4] ADC/AIN[6] AC/AIN[2] SERCOM0/PAD[2] *TCC1/WO[0]
 * | 9          | ~9               |  PA07  |                 | EIC/EXTINT[7] PTC/Y[5] DC/AIN[7] AC/AIN[3] SERCOM0/PAD[3] *TCC1/WO[1]
 * | 10         | ~10              |  PA18  |                 | EIC/EXTINT[2] PTC/X[6] SERCOM1/PAD[2] SERCOM3/PAD[2] *TC3/WO[0] TCC0/WO[2]
 * | 11         | ~11              |  PA16  |                 | EIC/EXTINT[0] PTC/X[4] SERCOM1/PAD[0] SERCOM3/PAD[0] *TCC2/WO[0] TCC0/WO[6]
 * | 12         | ~12              |  PA19  |                 | EIC/EXTINT[3] PTC/X[7] SERCOM1/PAD[3] SERCOM3/PAD[3] *TC3/WO[1] TCC0/WO[3]
 * | 13         | ~13              |  PA17  | LED             | EIC/EXTINT[1] PTC/X[5] SERCOM1/PAD[1] SERCOM3/PAD[1] *TCC2/WO[1] TCC0/WO[7]
 * | 14         | GND              |        |                 |
 * | 15         | AREF             |  PA03  |                 | *DAC/VREFP PTC/Y[1]
 * | 16         | SDA              |  PA22  |                 | EIC/EXTINT[6] PTC/X[10] *SERCOM3/PAD[0] SERCOM5/PAD[0] TC4/WO[0] TCC0/WO[4]
 * | 17         | SCL              |  PA23  |                 | EIC/EXTINT[7] PTC/X[11] *SERCOM3/PAD[1] SERCOM5/PAD[1] TC4/WO[1] TCC0/WO[5]
 * +------------+------------------+--------+-----------------+------------------------------
 * |            |SPI (Legacy ICSP) |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 18         | 1                |  PA12  | MISO            | EIC/EXTINT[12] SERCOM2/PAD[0] *SERCOM4/PAD[0] TCC2/WO[0] TCC0/WO[6]
 * | 19         | 2                |        | 5V0             |
 * | 20         | 3                |  PB11  | SCK             | EIC/EXTINT[11]                *SERCOM4/PAD[3] TC5/WO[1] TCC0/WO[5]
 * | 21         | 4                |  PB10  | MOSI            | EIC/EXTINT[10]                *SERCOM4/PAD[2] TC5/WO[0] TCC0/WO[4]
 * | 22         | 5                |        | RESET           |
 * | 23         | 6                |        | GND             |
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | Analog Connector |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 24         | A0               |  PA02  |                 | EIC/EXTINT[2] *ADC/AIN[0] PTC/Y[0] DAC/VOUT
 * | 25         | A1               |  PB08  |                 | EIC/EXTINT[8] *ADC/AIN[2] PTC/Y[14] SERCOM4/PAD[0] TC4/WO[0]
 * | 26         | A2               |  PB09  |                 | EIC/EXTINT[9] *ADC/AIN[3] PTC/Y[15] SERCOM4/PAD[1] TC4/WO[1]
 * | 27         | A3               |  PA04  |                 | EIC/EXTINT[4] *ADC/AIN[4] AC/AIN[0] PTC/Y[2] SERCOM0/PAD[0] TCC0/WO[0]
 * | 28         | A4               |  PA05  |                 | EIC/EXTINT[5] *ADC/AIN[5] AC/AIN[1] PTC/Y[5] SERCOM0/PAD[1] TCC0/WO[1]
 * | 29         | A5               |  PB02  |                 | EIC/EXTINT[2] *ADC/AIN[10] PTC/Y[8] SERCOM5/PAD[0] TC6/WO[0]
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | LEDs             |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 30         |                  |  PB03  | RX              |
 * | 31         |                  |  PA27  | TX              |
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | USB              |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 32         |                  |  PA28  | USB HOST ENABLE |
 * | 33         |                  |  PA24  | USB_NEGATIVE    | USB/DM
 * | 34         |                  |  PA25  | USB_POSITIVE    | USB/DP
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | EDBG             |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 35         |                  |  PB22  | EDBG_UART TX    | SERCOM5/PAD[2]
 * | 36         |                  |  PB23  | EDBG_UART RX    | SERCOM5/PAD[3]
 * +------------+------------------+--------+-----------------+------------------------------
 * | 37         |                  |  PA22  | EDBG_SDA        | SERCOM3/PAD[0]
 * | 38         |                  |  PA23  | EDBG_SCL        | SERCOM3/PAD[1]
 * +------------+------------------+--------+-----------------+------------------------------
 * | 39         |                  |  PA19  | EDBG_MISO       | SERCOM1/PAD[3]
 * | 40         |                  |  PA16  | EDBG_MOSI       | SERCOM1/PAD[0]
 * | 41         |                  |  PA18  | EDBG_SS         | SERCOM1/PAD[2]
 * | 42         |                  |  PA17  | EDBG_SCK        | SERCOM1/PAD[1]
 * +------------+------------------+--------+-----------------+------------------------------
 * | 43         |                  |  PA13  | EDBG_GPIO0      | EIC/EXTINT[13] *TCC2/WO[1] TCC0/WO[7]
 * | 44         |                  |  PA21  | EDBG_GPIO1      | Pin 7
 * | 45         |                  |  PA06  | EDBG_GPIO2      | Pin 8
 * | 46         |                  |  PA07  | EDBG_GPIO3      | Pin 9
 * +------------+------------------+--------+-----------------+------------------------------
 * |            |32.768KHz Crystal |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * |            |                  |  PA00  | XIN32           | EXTINT[0] SERCOM1/PAD[0] TCC2/WO[0]
 * |            |                  |  PA01  | XOUT32          | EXTINT[1] SERCOM1/PAD[1] TCC2/WO[1]
 * +------------+------------------+--------+-----------------+------------------------------
 */
