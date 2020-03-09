#include "IRremote.h"


#define DWT_CONTROL *(volatile unsigned long *)0xE0001000
#define SCB_DEMCR *(volatile unsigned long *)0xE000EDFC

static HAL::PulseOut<(uint32_t)TIM3,TIM3_CH3_PIN_B0,38000> IR_OUT; 

void DWT_Init()
{
  SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; 
  DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk;   
}



void sendRaw(unsigned int buf[], unsigned int len, uint8_t hz)
{
  enableIROut(hz);
  
  for(uint16_t i = 0; i < len; i++)
  {
    if(i & 1) space(buf[i]);
    else mark(buf[i]);
  }
  
  space(0);
}

void mark(unsigned int time)
{
  IR_OUT.Start();
  if (time > 0) custom_delay_usec(time);
}

void space(unsigned int time)
{
  IR_OUT.Stop();
  if(time > 0) custom_delay_usec(time);
}

void enableIROut(uint8_t khz)
{
  uint16_t pwm_freq = 0;
  uint16_t pwm_pulse = 0;
  pwm_freq = MYSYSCLOCK / (khz * 1000) - 1;
  pwm_pulse = pwm_freq / 3;

  DWT_Init();
  IR_OUT.HwInit();
  IR_OUT.SetDutyCycle(33);

}

void custom_delay_usec(unsigned long us)
{
  uint32_t us_count_tic =  us * (SystemCoreClock / 1000000);
  //uint32_t us_count_tic =  us * (HAL::GetSystemClock() / 1000000);
  DWT->CYCCNT = 0U; // обнуляем счётчик
  while(DWT->CYCCNT < us_count_tic);
}

