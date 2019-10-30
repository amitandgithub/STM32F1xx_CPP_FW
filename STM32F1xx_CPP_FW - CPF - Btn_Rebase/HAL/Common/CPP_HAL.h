#ifndef __CPP_HAL_H__
#define __CPP_HAL_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx.h"
#include "stm32f1xx_ll_gpio.h"
#include "Callback.h"
//#include "DebugLog.h"
//#include "printf.h"


#define DBG_LOG_TYPE uint32_t,5,7,20,100
#define DBG_LOG_CREATE_ID(__MODULE_ID,__X) Utils::DebugLog<DBG_LOG_TYPE>::Create(Utils::DebugLog<DBG_LOG_TYPE>::__MODULE_ID,__X)
namespace HAL
{

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

