


#include "System.h"



using namespace BSP;
using namespace HAL;
using namespace Utils;




InterruptManager InterruptManagerInstance;

DebugLog<DBG_LOG_TYPE> DebugLogInstance;

I2c i2c1(I2C1_B6_B7,100000U);

DMA DMA1Instance(1);

INA219 INA219_Dev(&i2c1,0x94);