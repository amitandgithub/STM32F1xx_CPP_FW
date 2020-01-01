
/* Includes */
#include <stddef.h>
#include <stdio.h>
#include"Test.h"

#define NEW_BOARD 0

void putc ( void* p, char c);
void SystemClock_Config_RTC_LSI(void);
void SystemClock_Config_RTC_LSE(void);
void SetSystickTimerInterrupt();

struct s {
  int i;
  unsigned u;
  char end[];
};

//Micros = Millis*1000 + 1000 – SysTick->VAL/72
//if you are free to use 32 bit timer you can even do it without IRQ, just simply time= TIMx->CNT.  

//HAL::GpioOutput LED_C13(A0);
//HAL::GpioInput LED_IN(B9);//INPUT_PULLUP
int main(void)
{	
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
     
  /* Configure the system clock */
  SystemClock_Config_RTC_LSE();    
  DWT->CTRL |= 1 ; // enable the counter  
  //LED_C13.HwInit();
  //LED_IN.HwInit();
  Rtc_test();
  while(1)
  {
    //BMP280_Test();
    //I2c_Tests_AT24C128();
    //I2c_Slave_Tests();
    //I2c_Full_EEPROM_POLL();
    //DigitalIO_tests();
    //LED_IN.Read() ? LED_C13.High() : LED_C13.Low() ;
    //if(MillisElapsed<100>()) ssd1306_test();
    //RUN_EVERY_MILLIS(100,fxx(1,2));
   // RUN_EVERY_MILLIS(100,ssd1306_test());
    //Rtc_test();
    //LL_mDelay(100);
    //ssd1306_test();
   Nokia_Lcd_Test();
    //w25qxx_Test();
    //st7735_Test();
  }    
}

void putc ( void* p, char c)
{
    //ITM_SendChar(c);
}

#ifdef __cplusplus
extern "C" {
#endif
  void _Error_Handler(const char *, int);
  
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif


void SystemClock_Config_RTC_LSI(void)
{
 LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

   if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
    Error_Handler();  
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    Error_Handler();  
  }
  
  LL_RCC_LSI_Enable();

   /* Wait till LSI is ready */
  while(LL_RCC_LSI_IsReady() != 1)
  {
    Error_Handler();  
  }
  
  LL_PWR_EnableBkUpAccess();
  LL_RCC_ForceBackupDomainReset();
  LL_RCC_ReleaseBackupDomainReset();
  LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);


  LL_RCC_EnableRTC();
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    Error_Handler();  
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
    Error_Handler();  
  }
  LL_Init1msTick(72000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(72000000);
}

void SystemClock_Config_RTC_LSE(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

   if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
    Error_Handler();  
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    Error_Handler();  
  }
  LL_PWR_EnableBkUpAccess();
  LL_RCC_ForceBackupDomainReset();
  LL_RCC_ReleaseBackupDomainReset();
  LL_RCC_LSE_Enable();

   /* Wait till LSE is ready */
  while(LL_RCC_LSE_IsReady() != 1)
  {
    Error_Handler();  
  }
  LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
  LL_RCC_EnableRTC();
#if NEW_BOARD
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_2, LL_RCC_PLL_MUL_9);
#else  
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
#endif
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    Error_Handler();  
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
    Error_Handler();  
  }
  
  LL_Init1msTick(72000000);
  SetSystickTimerInterrupt();
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(72000000);  
}

extern uint32_t SystickTimerTicks;
class SysTimerCallback : public InterruptSource
{
public:
  virtual void ISR(){SystickTimerTicks++;}
};

extern HAL::InterruptManager InterruptManagerInstance;
void SetSystickTimerInterrupt()
{
  static SysTimerCallback SysTimerObj;
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
  InterruptManagerInstance.RegisterDeviceInterrupt(SysTick_IRQn,0,&SysTimerObj);
}


/**
* @brief  This function is executed in case of error occurrence.
* @param  file: The file name as string.
* @param  line: The line in file as a number.
* @retval None
*/
void _Error_Handler(const char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
//  while(1)
//  {
//  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
  tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
