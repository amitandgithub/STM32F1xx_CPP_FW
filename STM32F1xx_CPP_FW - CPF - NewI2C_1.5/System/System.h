
#ifndef SYSTEM_H
#define SYSTEM_H


#include "stm32f1xx.h"
#include "I2c.h"
#include "DebugLog.h"
#include"CPP_HAL.h"

extern HAL::I2c i2c1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;



























#endif /* SYSTEM_H */