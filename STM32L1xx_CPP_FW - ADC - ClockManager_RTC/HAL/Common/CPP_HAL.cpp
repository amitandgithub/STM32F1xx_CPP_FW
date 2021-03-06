
/******************
** FILE: CPP_HAL.cpp
**
** DESCRIPTION:
**  HAL common functions and typedefs
**
** CREATED: 1/16/2019, by Amit Chaudhary
******************/

#include "CPP_HAL.h" 

#include "stm32l1xx.h"
#include "I2c.h"
#include "DebugLog.h"
#include "CPP_HAL.h"
#include "InterruptManager.h"
#include "INA219.h"
#include "RTC.h"
#include "DMA.h"
#include "Spi.h"
#include "Uart.h"
#include "AnalogIn.h"
#include "Nokia5110LCD.h"
#include "NokiaLCD.h"
#include "ST7735.h"




using namespace BSP;
using namespace HAL;
using namespace Utils;

uint32_t SystickTimerTicks; // Systick Timer millisecond Ticks counter

InterruptManager InterruptManagerInstance;

DebugLog<DBG_LOG_TYPE> DebugLogInstance;

GpioOutput LED(A2); 

I2c i2c1(I2C2_B10_B11);

DMA dma1(1);

INA219 INA219_Dev(0x94);

Rtc rtc;

Spi spi1(SPI1_A5_A6_A7);                        // CLK - A5, MISO - A6, MOSI - A7

Spi spi2(SPI2_B13_B14_B15);                     // CLK - B13, MISO - B14, MOSI - B15

NokiaLCD NokiaLCDObj(&spi1,B0,B4,B5,B8);       // CS - C5, DC - B0, Rst - C4, Bkl - C3, 

//Nokia5110LCD<&spi2,B0,B4,B5,B8> NokiaLCDObj;     // CS - C5, DC - B0, Rst - C4, Bkl - C3, 

//ST7735 TFT_1_8(ST7735::ST7735_160_x_128,&spi2,A11,A9,A10,A0);            // CS - A11, DC - A9, Rst - A10, Bkl - A0, 

//ST7735 TFT_Small(ST7735::ST7735_160_x_80,&spi2,B1,B2,B0,A0);           // CS - B1, DC - B2, Rst - B0, Bkl - A0, 

ST7735 TFT_135_240(ST7735::ST7789_135_x_240,&spi2,B1,B2,B0,A0);           // CS - B1, DC - B2, Rst - B0, Bkl - A0, 

ST7735 TFT_240_240(ST7735::ST7789_240_x_240,&spi1,A0,C13,A3,A0);           // CS - B1, DC - B2, Rst - B0, Bkl - A0, 

Uart uart1(UART1_A9_A10,9600);

AnalogIn adc0(LL_ADC_CHANNEL_0);

AnalogIn ChipTemperature(LL_ADC_CHANNEL_16);


namespace HAL
{
  
  void DWTHwInit()
  {
    *(volatile unsigned long *)0xE0001000 |= CoreDebug_DEMCR_TRCENA_Msk; // DWT_CONTROL
    *(volatile unsigned long *)0xE000EDFC |= DWT_CTRL_CYCCNTENA_Msk;   // SCB_DEMCR
  }


  uint32_t GetSystemClock()
  { 
    return SystemCoreClock; 
  }
  
  bool WaitOnFlag(volatile uint32_t* reg, uint32_t bitmask, uint32_t status, uint32_t timeout) // 35(0x25)bytes // stm32 0x28
  {
    while( ((*reg & bitmask) == status) && timeout-- );    
    return (bool)((*reg & bitmask) == status);
  }
  
  uint32_t GetTicksSince(uint32_t LastTick)
  {
    uint32_t CurrentTicks = HAL::GetTick();
    
    return ( (CurrentTicks >= LastTick)? (CurrentTicks - LastTick ) :   (CurrentTicks - LastTick + 0x00FFFFFFUL) );
  }
  
  void usDelay(uint32_t us)
  {
    uint32_t us_count_tic =  us * (SystemCoreClock / 1000000);
    DWT->CYCCNT = 0U; 
    while(DWT->CYCCNT < us_count_tic);
  }  
  
  void delayUS_DWT(uint32_t us) 
  {
    uint32_t cycles = (SystemCoreClock/1000000L)*us;
    uint32_t start = DWT->CYCCNT;
    while(DWT->CYCCNT - start < cycles);
  }
  
  
  void DBG_PRINT(uint8_t* Buf, uint16_t Len)
  {
#if USB_DEVICE
    while( CDC_Transmit_FS(Buf,Len) != USBD_OK);
#else
    while( uart1.TxPoll(Buf,Len) != HAL::Uart::UART_OK);
#endif   
  }

  void DBG_PRINT_NL()
  {
    uint8_t arr[3] = "\r\n";
#if USB_DEVICE
    while( CDC_Transmit_FS(arr,2) != USBD_OK);
#else
    while( uart1.TxPoll(arr,2) != HAL::Uart::UART_OK);
#endif   
  }  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
}

