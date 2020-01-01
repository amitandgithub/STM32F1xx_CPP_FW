
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
#include "Spi.h"
#include "Nokia5110LCD.h"
#include "NokiaLCD.h"
#include "ST7735.h"




using namespace BSP;
using namespace HAL;
using namespace Utils;

uint32_t SystickTimerTicks; // Systick Timer millisecond Ticks counter

HAL::Callback* g_GpioCallbacks[16];

InterruptManager InterruptManagerInstance;

DebugLog<DBG_LOG_TYPE> DebugLogInstance;

//I2c i2c1(I2C1_B6_B7,400000U);

DMA dma1(1);

INA219 INA219_Dev(0x94);

Rtc rtc;

Spi spi1(SPI1_A5_A6_A7);  // CLK - A5, MISO - A6, MOSI - A7

Spi spi2(SPI2_B13_B14_B15); // CLK - B13, MISO - B14, MOSI - B15

Nokia5110LCD<&spi1,B1,B0,B10,A0> LCD; // CS - B1, DC - B0, Rst - B10, Bkl - A0, 

NokiaLCD NokiaLcdObj(&spi1,B1,B0,B10,A0);  // CS - B1, DC - B0, Rst - B10, Bkl - A0, 

ST7735 TFT_1_8(&spi2,A11,A9,A10,A0); // CS - A11, DC - A9, Rst - A10, Bkl - A0, 

ST7735 TFT_Small(&spi2,B0,A9,A10,A0); // CS - A11, DC - A9, Rst - A10, Bkl - A0, 


namespace HAL
{
  
  
  bool WaitOnFlag(volatile uint32_t* reg, uint32_t bitmask, uint32_t status, uint32_t timeout) // 35(0x25)bytes // stm32 0x28
  {
    while( ((*reg & bitmask) == status) && timeout-- );    
    return (bool)((*reg & bitmask) == status);
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
}

