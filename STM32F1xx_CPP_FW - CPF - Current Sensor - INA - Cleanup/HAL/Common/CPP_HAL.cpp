
/******************
** FILE: CPP_HAL.cpp
**
** DESCRIPTION:
**  HAL common functions and typedefs
**
** CREATED: 1/16/2019, by Amit Chaudhary
******************/

#include "CPP_HAL.h" 

#include "stm32f1xx.h"
#include "I2c.h"
#include "DebugLog.h"
#include "CPP_HAL.h"
#include "InterruptManager.h"
#include "INA219.h"
#include "RTC.h"
#include "DMA.h"


using namespace BSP;
using namespace HAL;
using namespace Utils;

uint32_t SystickTimerTicks; // Systick Timer millisecond Ticks counter


InterruptManager InterruptManagerInstance;

DebugLog<DBG_LOG_TYPE> DebugLogInstance;

I2c i2c1(I2C1_B6_B7,400000U);

DMA dma1(1);

INA219 INA219_Dev(0x94);

Rtc rtc;