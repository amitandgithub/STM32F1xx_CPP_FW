
/* Includes */
#include <stddef.h>
#include"Test.h"
#include "CPP_HAL.h"


#if USB_DEVICE

#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
void usb_init();

static void MX_GPIO_Init(void);
uint32_t Debug_Write(uint8_t* buf, uint16_t len);
void Parse_Command(uint8_t *buf, uint32_t len);
void CDC_ReceiveCallback(uint8_t *buf, uint32_t len);

#endif

void SystemClock_Config(void);
void SetSystickTimerInterrupt();
extern Uart uart1;
extern GpioOutput LED;

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
// MCP1703T input max 16V

#define NEW_BOARD 1
#define RTC_ON_LSE 0

int main(void)
{
  SystemClock_Config();
  
  /* Initialize all configured peripherals */
#if USB_DEVICE
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
#else
  uart1.HwInit(9600);
  init_printf(NULL,putc);
  printf("%f\n\r", 0.1234); 
#endif
  
  DWTHwInit();
  Rtc_test();
  LED.HwInit();
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
    //Power_Monitor_Test();
    //Uart_Test();
    //Adc_Test();
    //Timer_Test();
    //Ir_Test();
    // Template_Tests();
   // LED.Toggle();
    Menu_test();
  }    
}

void SystemClock_Config(void)
{
  LL_FLASH_EnablePrefetch();  
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  
#if NEW_BOARD
  HAL::ClockManager::SetSystemClock(HAL::ClockManager::CLOCK_HSE,LL_RCC_PLLSOURCE_HSE_DIV_2,LL_RCC_PLL_MUL_9);
#else
  HAL::ClockManager::SetSystemClock(HAL::ClockManager::CLOCK_HSE,LL_RCC_PLLSOURCE_HSE_DIV_1,LL_RCC_PLL_MUL_9);
#endif
  
  LL_Init1msTick(72000000);
  SetSystickTimerInterrupt();
#if USB_DEVICE
  LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL_DIV_1_5);
#endif
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
  InterruptManagerInstance.RegisterDeviceInterrupt(SysTick_IRQn,1,&SysTimerObj);
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


