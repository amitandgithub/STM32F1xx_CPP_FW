/******************
** FILE: ClockManager.h
**
** DESCRIPTION:
**  ClockManager Class
**
** CREATED: 113-Apr-2020, by Amit Chaudhary - Happy Baisakhi in Corona Lockdown
******************/



#ifndef ClockManager_h
#define ClockManager_h

#include "CPP_HAL.h"
#include"chip_peripheral.h"
#include"Utils.h"
#include"InterruptManager.h"
#include"Callback.h"


namespace HAL
{  
  
  class ClockManager : public InterruptSource
  {
  public:
    
    typedef enum
    {
      CLOCK_HSE,
      CLOCK_HSI,
      CLOCK_PLL    
    }SystemClock_t;
    
    typedef enum
    {
      CLOCK_LSE,
      CLOCK_LSI,
      CLOCK_HSI_DIV_128    
    }RTCClock_t;
    
    typedef enum
    {
      /* AHB peripherals*/
      CLOCK_TYPE_AHB    = 0x00000000,
      CLOCK_DMA1        = CLOCK_TYPE_AHB |  0,
      CLOCK_DMA2        = CLOCK_TYPE_AHB |  1,
      CLOCK_SRAM        = CLOCK_TYPE_AHB |  2,
      CLOCK_FLITF       = CLOCK_TYPE_AHB |  4,
      CLOCK_CRC         = CLOCK_TYPE_AHB |  6,
      CLOCK_FSM         = CLOCK_TYPE_AHB |  8,
      CLOCK_SDIO        = CLOCK_TYPE_AHB |  10,
      
      /* APB1 peripherals*/
      CLOCK_TYPE_APB1   = 0x10000000,
      CLOCK_TIM2        = CLOCK_TYPE_APB1 | 0,
      CLOCK_TIM3        = CLOCK_TYPE_APB1 | 1,
      CLOCK_TIM4        = CLOCK_TYPE_APB1 | 2,
      CLOCK_TIM5        = CLOCK_TYPE_APB1 | 3,
      CLOCK_TIM6        = CLOCK_TYPE_APB1 | 4,
      CLOCK_TIM7        = CLOCK_TYPE_APB1 | 5,
      CLOCK_TIM12       = CLOCK_TYPE_APB1 | 6,
      CLOCK_TIM13       = CLOCK_TYPE_APB1 | 7,
      CLOCK_TIM14       = CLOCK_TYPE_APB1 | 8,
      CLOCK_WWDG        = CLOCK_TYPE_APB1 | 11,
      CLOCK_SPI2        = CLOCK_TYPE_APB1 | 14,
      CLOCK_SPI3        = CLOCK_TYPE_APB1 | 15,
      CLOCK_USART2       = CLOCK_TYPE_APB1 | 17,
      CLOCK_USART3       = CLOCK_TYPE_APB1 | 18,
      CLOCK_USART4       = CLOCK_TYPE_APB1 | 19,
      CLOCK_USART5       = CLOCK_TYPE_APB1 | 20,
      CLOCK_I2C1        = CLOCK_TYPE_APB1 | 21,
      CLOCK_I2C2        = CLOCK_TYPE_APB1 | 22,
      CLOCK_USB         = CLOCK_TYPE_APB1 | 23,
      CLOCK_CAN         = CLOCK_TYPE_APB1 | 25,
      CLOCK_BKP         = CLOCK_TYPE_APB1 | 27,
      CLOCK_PWR         = CLOCK_TYPE_APB1 | 28,
      CLOCK_DAC         = CLOCK_TYPE_APB1 | 29,        
      
      
      /* APB2 peripherals*/
      CLOCK_TYPE_APB2   = 0x20000000,
      CLOCK_AFIO        = CLOCK_TYPE_APB2 | 0,
      CLOCK_GPIOA       = CLOCK_TYPE_APB2 | 2,
      CLOCK_GPIOB       = CLOCK_TYPE_APB2 | 3,
      CLOCK_GPIOC       = CLOCK_TYPE_APB2 | 4,
      CLOCK_GPIOD       = CLOCK_TYPE_APB2 | 5,
      CLOCK_GPIOE       = CLOCK_TYPE_APB2 | 6,
      CLOCK_GPIOF       = CLOCK_TYPE_APB2 | 7,
      CLOCK_GPIOG       = CLOCK_TYPE_APB2 | 8,      
      CLOCK_ADC1        = CLOCK_TYPE_APB2 | 9,
      CLOCK_ADC2        = CLOCK_TYPE_APB2 | 10,
      CLOCK_TIM1        = CLOCK_TYPE_APB2 | 11,
      CLOCK_SPI1        = CLOCK_TYPE_APB2 | 12,
      CLOCK_TIM8        = CLOCK_TYPE_APB2 | 13,
      CLOCK_USART1       = CLOCK_TYPE_APB2 | 14,
      CLOCK_ADC3        = CLOCK_TYPE_APB2 | 15,
      CLOCK_TIM9        = CLOCK_TYPE_APB2 | 19,
      CLOCK_TIM10       = CLOCK_TYPE_APB2 | 20,
      CLOCK_TIM11       = CLOCK_TYPE_APB2 | 21,
     
      
    }PeripheralID_t;
    
   static bool Enable(PeripheralID_t PeripheralID)
    {
      if(PeripheralID & CLOCK_TYPE_AHB)
      {
        RCC->AHBENR |= 1UL << (uint8_t)PeripheralID;
        return true;
      }
      else if(PeripheralID & CLOCK_TYPE_APB1)
      {
        RCC->APB1ENR |= 1UL << (uint8_t)PeripheralID;
        return true;
      }
      else if(PeripheralID & CLOCK_TYPE_APB2)
      {
        RCC->APB2ENR |= 1UL << (uint8_t)PeripheralID;
        return true;
      }
      else
      {
        return false;
      }
    }
    
    static bool Disable(PeripheralID_t PeripheralID)
    {
      if(PeripheralID & CLOCK_TYPE_AHB)
      {
        RCC->AHBENR &= ~(1UL << (uint8_t)PeripheralID);
        return true;
      }
      else if(PeripheralID & CLOCK_TYPE_APB1)
      {
        RCC->APB1ENR &= ~(1UL << (uint8_t)PeripheralID);
        return true;
      }
      else if(PeripheralID & CLOCK_TYPE_APB2)
      {
        RCC->APB2ENR &= ~(1UL << (uint8_t)PeripheralID);
        return true;
      }
      else
      {
        return false;
      }
    }
   
   static bool SetSystemClock(SystemClock_t SystemClock, uint32_t Divider, uint32_t Multiplier)
   {
     if(SystemClock == CLOCK_HSE)
     {
       LL_RCC_HSE_Enable();       
       /* Wait till HSE is ready */
       while(LL_RCC_HSE_IsReady() != 1)
       {
         
       }
       LL_RCC_PLL_ConfigDomain_SYS( Divider == LL_RCC_PLLSOURCE_HSE_DIV_2 ? LL_RCC_PLLSOURCE_HSE_DIV_2 : LL_RCC_PLLSOURCE_HSE_DIV_1, Multiplier);
       //return true;
     }
     else  if(SystemClock == CLOCK_HSI)
     {
       LL_RCC_HSI_Enable();       
       /* Wait till HSE is ready */
       while(LL_RCC_HSI_IsReady() != 1)
       {
         
       }
       LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, Multiplier);
       //return true;
     }
     else
     {
       return false;
     }
     
     LL_RCC_PLL_Enable();
     
     /* Wait till PLL is ready */
     while(LL_RCC_PLL_IsReady() != 1)
     {
       
     }
     
     LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
     LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
     LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
     
     LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
     
     /* Wait till System clock is ready */
     while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
     {
       
     }
     LL_SetSystemCoreClock(72000000);  
  
     return true;
   }
   
   
   
   
  };
  
  
  
}



#endif // ClockManager_h