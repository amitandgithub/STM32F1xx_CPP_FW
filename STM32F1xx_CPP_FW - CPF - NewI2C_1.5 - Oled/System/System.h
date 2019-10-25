
#ifndef SYSTEM_H
#define SYSTEM_H


#include "stm32f1xx.h"
#include "I2c.h"
#include "DebugLog.h"
#include "CPP_HAL.h"
#include "InterruptManager.h"

extern HAL::InterruptManager InterruptManagerInstance;
extern HAL::I2c i2c1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::DMA DMA1Instance;



























#endif /* SYSTEM_H */