





#include "System.h"


using namespace HAL;
using namespace Utils;

DebugLog<DBG_LOG_TYPE> DebugLogInstance;
I2c i2c1(Gpio::B6, Gpio::B7,100000U);