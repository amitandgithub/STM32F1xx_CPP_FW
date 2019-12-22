
#ifndef SYSTEM_H
#define SYSTEM_H


#include "stm32f1xx.h"
#include "I2c.h"
#include "DebugLog.h"
#include "CPP_HAL.h"
#include "InterruptManager.h"
#include "INA219.h"
#include "RTC.h"
#include "DMA.h"

extern uint32_t SystickTimerTicks;

extern HAL::InterruptManager InterruptManagerInstance;
extern HAL::I2c i2c1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::DMA dma1;
extern BSP::INA219 INA219_Dev;
extern HAL::Rtc rtc;

























#endif /* SYSTEM_H */