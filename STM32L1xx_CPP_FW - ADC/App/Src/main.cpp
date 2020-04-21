
/* Includes */
#include <stddef.h>
#include"Test.h"
#include "CPP_HAL.h"
#include "ClockManager.h"


#if USB_DEVICE

#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

uint8_t Buf[20] = "Amit Chaudhary\n\r\r"; // len - 16
void usb_init();

static void MX_GPIO_Init(void);
uint32_t Debug_Write(uint8_t* buf, uint16_t len);
void Parse_Command(uint8_t *buf, uint32_t len);
void CDC_ReceiveCallback(uint8_t *buf, uint32_t len);

#endif

void SystemClock_Config(void);
void SetSystickTimerInterrupt();
extern Uart uart1;

void putc ( void* p, char c)
{
  uart1.TxPoll(c);
}

struct s {
  int i;
  unsigned u;
  char end[];
};

//asm("bkpt 255");
//Micros = Millis*1000 + 1000 – SysTick->VAL/72
//if you are free to use 32 bit timer you can even do it without IRQ, just simply time= TIMx->CNT.  
//  uint8_t day = STR_TO_BCD(__DATE__[4],__DATE__[5]);
//  uint8_t yr  = STR_TO_BCD(__DATE__[9],__DATE__[8]);

#define NEW_BOARD 1
#define RTC_ON_LSE 0

int main(void)
{
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_COMP);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);  
  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
#if USB_DEVICE
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
#else
  uart1.HwInit(9600/2);
  init_printf(NULL,putc);
#endif
  LED.HwInit();
  DWTHwInit();
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
    //Nokia_Lcd_Test();
    // w25qxx_Test();
    //st7735_Test();
   // HAL::DBG_PRINT((uint8_t*)"Amit\n\r",6);
   // Power_Monitor_Test();
    //Uart_Test();
    Adc_Test();
    //Timer_Test();
    //Ir_Test();
    // Template_Tests();
  }    
}

void SystemClock_Config(void)
{
  LL_FLASH_Enable64bitAccess();

  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
  {
  //Error_Handler();  
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  
  HAL::ClockManager::SetSystemClock(HAL::ClockManager::CLOCK_HSE,LL_RCC_PLL_MUL_4,LL_RCC_PLL_DIV_3);
  
  // Enable HSI clock for ADC peripheral
  HAL::ClockManager::SetSystemClock(HAL::ClockManager::CLOCK_HSI,LL_RCC_PLL_MUL_4,LL_RCC_PLL_DIV_3);
  
#if 0
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    
  }
  LL_PWR_EnableBkUpAccess();
  LL_RCC_ForceBackupDomainReset();
  LL_RCC_ReleaseBackupDomainReset();
  
  /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_Enable();
  LL_RCC_HSI_SetCalibTrimming(16);
  
  
#if RTC_ON_LSE
  LL_RCC_LSE_Disable();
  LL_RCC_LSE_Enable();

   /* Wait till LSE is ready */
  while(LL_RCC_LSE_IsReady() != 1)
  {
    
  }
  LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
#else
  LL_RCC_LSI_Enable();
  
  /* Wait till LSE is ready */
  while(LL_RCC_LSI_IsReady() != 1)
  {
    
  }
  LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
#endif
  
  LL_RCC_EnableRTC();
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLL_MUL_4, LL_RCC_PLL_DIV_3);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
#endif //0
  LL_SetSystemCoreClock(32000000);
  LL_Init1msTick(32000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  
  SetSystickTimerInterrupt();
#if USB_DEVICE
  LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL_DIV_1_5);
#endif

}

#if USB_DEVICE


static void MX_GPIO_Init(void)
{
  
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
}

/* USER CODE BEGIN 4 */

void CDC_ReceiveCallback(uint8_t *buf, uint32_t len)
{
  Parse_Command(buf,len);
}


void Parse_Command(uint8_t *buf, uint32_t len)
{
  if(buf == NULL || len ==0 ) return;
  
  switch(buf[0])
  {
  case 'A': Debug_Write((uint8_t*)"Amit\n\r",6); break;
  case 'a': Debug_Write((uint8_t*)"Avni\n\r",6); break;
  case 's': Debug_Write((uint8_t*)"Shiwani\n\r",9); break;
  default:  Debug_Write((uint8_t*)"??\n\r",4); break;
  }  
}

uint32_t Debug_Write(uint8_t* buf, uint16_t len)
{
  CDC_Transmit_FS(buf,len);
  return 0;
}

#endif

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
  InterruptManagerInstance.RegisterDeviceInterrupt(SysTick_IRQn,1,&SysTimerObj);
}
