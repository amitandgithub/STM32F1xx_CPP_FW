#ifndef __CPP_HAL_H__
#define __CPP_HAL_H__

/* Includes ------------------------------------------------------------------*/

#include "stm32f1xx.h"
#include "Callback.h"
#include "stdio.h"
#include <stddef.h>
#include <string.h>
#include <cstdint>


#define USB_DEVICE 0

typedef enum
{
  I2C1_B6_B7,
  I2C2_B10_B11,
  I2C1_B8_B9,
}I2cPort_t;

typedef enum
{
    SPI1_A5_A6_A7,
    SPI1_A15_B3_B4_B5,
    SPI2_B13_B14_B15
}SpiPort_t;

extern uint32_t SystickTimerTicks;

#define DBG_LOG_TYPE uint32_t,5,7,20,100
#define DBG_LOG_CREATE_ID(__MODULE_ID,__X) Utils::DebugLog<DBG_LOG_TYPE>::Create(Utils::DebugLog<DBG_LOG_TYPE>::__MODULE_ID,__X)

  template <uint32_t millisec>
    bool MillisElapsed()
    {
      extern uint32_t SystickTimerTicks;
      static uint32_t previous_millis = SystickTimerTicks;
      
      if(SystickTimerTicks >= previous_millis + millisec)
      {
        previous_millis = SystickTimerTicks;
        return true;
      }
      return false;      
    }
  
#define RUN_EVERY_MILLIS(MILLIS,FX) if(MillisElapsed<MILLIS>()) FX
  
  
namespace HAL
{


void DBG_PRINT(uint8_t* Buf, uint16_t Len);

bool WaitOnFlag(volatile uint32_t* reg, uint32_t bitmask, uint32_t status, uint32_t timeout);

inline uint32_t  GetTick() {return SystickTimerTicks;}  
  
typedef enum
{
    HAL_STATUS_SUCCESS = 0U,
    HAL_STATUS_ERROR
        
}HAL_Status_t;  
    
typedef void (*HALCallback_t)();

typedef struct
{
  uint8_t* Buf;
  uint8_t  Len;
  uint8_t  Idx;     
}Buf_t;

typedef struct
{
  uint8_t* Buf;
  uint16_t  Len;
  uint16_t  Idx;     
}SpiBuf_t;


}

#if 0
void delayUS_DWT(uint32_t us) {
	volatile uint32_t cycles = (SystemCoreClock/1000000L)*us;
	volatile uint32_t start = DWT->CYCCNT;
	do  {
	} while(DWT->CYCCNT - start < cycles);
}


uint32_t cycles = 0;

/* DWT struct is defined inside the core_cm4.h file */
DWT->CTRL |= 1 ; // enable the counter
DWT->CYCCNT = 0; // reset the counter
delayUS(1);
cycles = DWT->CYCCNT;
cycles--; /* We subtract the cycle used to transfer 
             CYCCNT content to cycles variable */
			 	
#endif

/* Private define ------------------------------------------------------------*/

#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
//void _Error_Handler(const char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __CPP_HAL_H__ */

