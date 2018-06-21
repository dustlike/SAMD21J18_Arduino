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

#pragma once

#include <stdint.h>
#include "sam.h"


#define MAX_PIN_NUMBER	64


//ADC Channels Definition
enum AdcChannel
{
	No_ADC_Channel = -1,
	ADC_Channel0 = 0,
	ADC_Channel1,
	ADC_Channel2,
	ADC_Channel3,
	ADC_Channel4,
	ADC_Channel5,
	ADC_Channel6,
	ADC_Channel7,
	ADC_Channel8,
	ADC_Channel9,
	ADC_Channel10,
	ADC_Channel11,
	ADC_Channel12,
	ADC_Channel13,
	ADC_Channel14,
	ADC_Channel15,
	ADC_Channel16,
	ADC_Channel17,
	ADC_Channel18,
	ADC_Channel19,
};

extern const enum AdcChannel AdcChannelMapping[];


// Definitions for TC channels
typedef enum _ETCChannel
{
  NOT_ON_TIMER=-1,
  TCC0_CH0 = (0<<8)|(0),
  TCC0_CH1 = (0<<8)|(1),
  TCC0_CH2 = (0<<8)|(2),
  TCC0_CH3 = (0<<8)|(3),
  TCC0_CH4 = (0<<8)|(0), // Channel 4 is 0!
  TCC0_CH5 = (0<<8)|(1), // Channel 5 is 1!
  TCC0_CH6 = (0<<8)|(2), // Channel 6 is 2!
  TCC0_CH7 = (0<<8)|(3), // Channel 7 is 3!
  TCC1_CH0 = (1<<8)|(0),
  TCC1_CH1 = (1<<8)|(1),
  TCC1_CH2 = (1<<8)|(0), // Channel 2 is 0!
  TCC1_CH3 = (1<<8)|(1), // Channel 3 is 1!
  TCC2_CH0 = (2<<8)|(0),
  TCC2_CH1 = (2<<8)|(1),
  TCC2_CH2 = (2<<8)|(0), // Channel 2 is 0!
  TCC2_CH3 = (2<<8)|(1), // Channel 3 is 1!
  TC3_CH0  = (3<<8)|(0),
  TC3_CH1  = (3<<8)|(1),
  TC4_CH0  = (4<<8)|(0),
  TC4_CH1  = (4<<8)|(1),
  TC5_CH0  = (5<<8)|(0),
  TC5_CH1  = (5<<8)|(1),
#if defined __SAMD21J18A__
  TC6_CH0  = (6<<8)|(0),
  TC6_CH1  = (6<<8)|(1),
  TC7_CH0  = (7<<8)|(0),
  TC7_CH1  = (7<<8)|(1),
#endif // __SAMD21J18A__
} ETCChannel ;

extern const void* g_apTCInstances[TCC_INST_NUM+TC_INST_NUM] ;

#define GetTCNumber( x ) ( (x) >> 8 )
#define GetTCChannelNumber( x ) ( (x) & 0xff )
#define GetTC( x ) ( g_apTCInstances[(x) >> 8] )

// Definitions for PWM channels
typedef enum _EPWMChannel
{
  NOT_ON_PWM=-1,
  PWM0_CH0=TCC0_CH0,
  PWM0_CH1=TCC0_CH1,
  PWM0_CH2=TCC0_CH2,
  PWM0_CH3=TCC0_CH3,
  PWM0_CH4=TCC0_CH4,
  PWM0_CH5=TCC0_CH5,
  PWM0_CH6=TCC0_CH6,
  PWM0_CH7=TCC0_CH7,
  PWM1_CH0=TCC1_CH0,
  PWM1_CH1=TCC1_CH1,
  PWM1_CH2=TCC1_CH2,
  PWM1_CH3=TCC1_CH3,
  PWM2_CH0=TCC2_CH0,
  PWM2_CH1=TCC2_CH1,
  PWM2_CH2=TCC2_CH2,
  PWM2_CH3=TCC2_CH3,
  PWM3_CH0=TC3_CH0,
  PWM3_CH1=TC3_CH1,
  PWM4_CH0=TC4_CH0,
  PWM4_CH1=TC4_CH1,
  PWM5_CH0=TC5_CH0,
  PWM5_CH1=TC5_CH1,
#if defined __SAMD21J18A__
  PWM6_CH0=TC6_CH0,
  PWM6_CH1=TC6_CH1,
  PWM7_CH0=TC7_CH0,
  PWM7_CH1=TC7_CH1,
#endif // __SAMD21J18A__
} EPWMChannel ;


//A    B                 C       D          E      F   G   H
//EIC REF ADC AC PTC DAC SERCOM SERCOM_ALT TC/TCC TCC COM AC/GCLK

typedef enum _EPioType
{
  PIO_NOT_A_PIN=-1,     /* Not under control of a peripheral. */
  PIO_EXTINT=0,         /* The pin is controlled by the associated signal of peripheral A. */
  PIO_ANALOG,           /* The pin is controlled by the associated signal of peripheral B. */
  PIO_SERCOM,           /* The pin is controlled by the associated signal of peripheral C. */
  PIO_SERCOM_ALT,       /* The pin is controlled by the associated signal of peripheral D. */
  PIO_TIMER,            /* The pin is controlled by the associated signal of peripheral E. */
  PIO_TIMER_ALT,        /* The pin is controlled by the associated signal of peripheral F. */
  PIO_COM,              /* The pin is controlled by the associated signal of peripheral G. */
  PIO_AC_CLK,           /* The pin is controlled by the associated signal of peripheral H. */

  PIO_PWM=PIO_TIMER,
  PIO_PWM_ALT=PIO_TIMER_ALT,
} EPioType ;


/* Generic Clock Multiplexer IDs */
#define GCM_DFLL48M_REF           (0x00U)
#define GCM_FDPLL96M_INPUT        (0x01U)
#define GCM_FDPLL96M_32K          (0x02U)
#define GCM_WDT                   (0x03U)
#define GCM_RTC                   (0x04U)
#define GCM_EIC                   (0x05U)
#define GCM_USB                   (0x06U)
#define GCM_EVSYS_CHANNEL_0       (0x07U)
#define GCM_EVSYS_CHANNEL_1       (0x08U)
#define GCM_EVSYS_CHANNEL_2       (0x09U)
#define GCM_EVSYS_CHANNEL_3       (0x0AU)
#define GCM_EVSYS_CHANNEL_4       (0x0BU)
#define GCM_EVSYS_CHANNEL_5       (0x0CU)
#define GCM_EVSYS_CHANNEL_6       (0x0DU)
#define GCM_EVSYS_CHANNEL_7       (0x0EU)
#define GCM_EVSYS_CHANNEL_8       (0x0FU)
#define GCM_EVSYS_CHANNEL_9       (0x10U)
#define GCM_EVSYS_CHANNEL_10      (0x11U)
#define GCM_EVSYS_CHANNEL_11      (0x12U)
#define GCM_SERCOMx_SLOW          (0x13U)
#define GCM_SERCOM0_CORE          (0x14U)
#define GCM_SERCOM1_CORE          (0x15U)
#define GCM_SERCOM2_CORE          (0x16U)
#define GCM_SERCOM3_CORE          (0x17U)
#define GCM_SERCOM4_CORE          (0x18U)
#define GCM_SERCOM5_CORE          (0x19U)
#define GCM_TCC0_TCC1             (0x1AU)
#define GCM_TCC2_TC3              (0x1BU)
#define GCM_TC4_TC5               (0x1CU)
#define GCM_TC6_TC7               (0x1DU)
#define GCM_ADC                   (0x1EU)
#define GCM_AC_DIG                (0x1FU)
#define GCM_AC_ANA                (0x20U)
#define GCM_DAC                   (0x21U)
#define GCM_PTC                   (0x22U)
#define GCM_I2S_0                 (0x23U)
#define GCM_I2S_1                 (0x24U)
