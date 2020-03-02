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

typedef enum : uint8_t
{
    UART1_A9_A10,
    UART1_B6_B7,        // Remap
    UART2_A2_A3,
    UART2_D5_D6,        // Remap
    UART3_B10_B11,
    UART3_C10_C11,      // Partial-Remap
    UART3_D8_D9,        // Full-Remap
}UartPort_t;


typedef enum : uint8_t
{
  /* Timer 1*/
  TIM1_ETR_PIN_A12,
  TIM1_CH1_PIN_A8,
  TIM1_CH2_PIN_A9,
  TIM1_CH3_PIN_A10,
  TIM1_CH4_PIN_A11,  
  TIM1_BKIN_PIN_B12,  
  TIM1_CH1N_PIN_B13,
  TIM1_CH2N_PIN_B14,
  TIM1_CH3N_PIN_B15,
  
  TIM1_ETR_PIN_REMAP_E7,
  TIM1_CH1_PIN_REMAP_E9,
  TIM1_CH2_PIN_REMAP_E11,
  TIM1_CH3_PIN_REMAP_E13,
  TIM1_CH4_PIN_REMAP_E14,  
  TIM1_BKIN_PIN_REMAP_E15,  
  TIM1_CH1N_PIN_REMAP_E8,
  TIM1_CH2N_PIN_REMAP_E10,
  TIM1_CH3N_PIN_REMAP_E12,
  
  TIM1_ETR_PIN_PAR_REMAP_A12,
  TIM1_CH1_PIN_PAR_REMAP_A8,
  TIM1_CH2_PIN_PAR_REMAP_A9,
  TIM1_CH3_PIN_PAR_REMAP_A10,
  TIM1_CH4_PIN_PAR_REMAP_A11,  
  TIM1_BKIN_PIN_PAR_REMAP_A6,  
  TIM1_CH1N_PIN_PAR_REMAP_A7,
  TIM1_CH2N_PIN_PAR_REMAP_B0,
  TIM1_CH3N_PIN_PAR_REMAP_B1,
  
  /* Timer 2*/
  TIM2_ETR_PIN_A0,
  TIM2_CH1_PIN_A0,
  TIM2_CH2_PIN_A1,
  TIM2_CH3_PIN_A2,
  TIM2_CH4_PIN_A3,  
  
  TIM2_ETR_PIN_REMAP_A15,
  TIM2_CH1_PIN_REMAP_A15,
  TIM2_CH2_PIN_REMAP_B3,
  TIM2_CH3_PIN_REMAP_B10,
  TIM2_CH4_PIN_REMAP_B11,  
  
  TIM2_ETR_PIN_PAR_REMAP_A15,
  TIM2_CH1_PIN_PAR_REMAP_A15,
  TIM2_CH2_PIN_PAR_REMAP_B3,
  TIM2_CH3_PIN_PAR_REMAP_A2,
  TIM2_CH4_PIN_PAR_REMAP_A3,
  
  TIM2_ETR_PIN_PAR_REMAP_A0,
  TIM2_CH1_PIN_PAR_REMAP_A0,
  TIM2_CH2_PIN_PAR_REMAP_B1,
  TIM2_CH3_PIN_PAR_REMAP_B10,
  TIM2_CH4_PIN_PAR_REMAP_B11,
  
  /* Timer 3*/ 
  TIM3_CH1_PIN_A6,
  TIM3_CH2_PIN_A7,
  TIM3_CH3_PIN_B0,
  TIM3_CH4_PIN_B1,
  
  TIM3_CH1_PIN_PAR_REMAP_B4,
  TIM3_CH2_PIN_PAR_REMAP_B5,
  TIM3_CH3_PIN_PAR_REMAP_B0,
  TIM3_CH4_PIN_PAR_REMAP_B1, 
  
  TIM3_CH1_PIN_REMAP_C6,
  TIM3_CH2_PIN_REMAP_C7,
  TIM3_CH3_PIN_REMAP_C8,
  TIM3_CH4_PIN_REMAP_C9,
 
  
  /* Timer 4*/
  TIM4_CH1_PIN_B6,
  TIM4_CH2_PIN_B7,
  TIM4_CH3_PIN_B8,
  TIM4_CH4_PIN_B9,
  
  TIM4_CH1_PIN_REMAP_D12,
  TIM4_CH2_PIN_REMAP_D13,
  TIM4_CH3_PIN_REMAP_D14,
  TIM4_CH4_PIN_REMAP_D15,
  
}TimerChanel_t;
      
extern uint32_t SystickTimerTicks;

#define STR_TO_BCD(a,b) (a - '0') << 4 | (b - '0')

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
void DBG_PRINT_NL();

bool WaitOnFlag(volatile uint32_t* reg, uint32_t bitmask, uint32_t status, uint32_t timeout);

inline uint32_t  GetTick() {return SystickTimerTicks;}  

uint32_t GetTicksSince(uint32_t LastTick);

uint32_t GetSystemClock();
  
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

