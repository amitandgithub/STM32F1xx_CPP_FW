


#include "System.h"



using namespace BSP;
using namespace HAL;
using namespace Utils;




InterruptManager InterruptManagerInstance;

DebugLog<DBG_LOG_TYPE> DebugLogInstance;

I2c i2c1(Gpio::B6, Gpio::B7,400000U);

DMA DMA1Instance(1);

INA219 INA219_Dev(&i2c1,0x94);