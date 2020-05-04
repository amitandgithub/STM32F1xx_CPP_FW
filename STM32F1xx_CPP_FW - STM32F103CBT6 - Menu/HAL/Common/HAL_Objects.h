
#ifndef HAL_OBJECTS_H
#define HAL_OBJECTS_H


//#include"HAL_Objects.h"

#include"DMA.h"
#include"InterruptManager.h"
#include"DebugLog.h"
#include"RTC.h"
#include"I2c.h"
#include"INA219.h"
#include"Spi.h"
#include"NokiaLCD.h"
#include"Uart.h"
#include"ADC.h"


extern uint32_t SystickTimerTicks;
extern HAL::GpioOutput LED_C13;
extern HAL::DMA dma1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::InterruptManager InterruptManagerInstance;
extern HAL::Rtc rtc;
extern HAL::I2c i2c1;
extern HAL::Spi spi1;
extern HAL::Spi spi2;
extern BSP::INA219 INA219_Dev;
extern BSP::NokiaLCD NokiaLCDObj; 
extern HAL::Uart uart1;





#endif /* HAL_OBJECTS_H */