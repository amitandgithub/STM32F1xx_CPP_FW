


#include"CPP_HAL.h"
#include"Timer.h"
#include"Gpt.h"
#include"PulseIn.h"

#include "rc5_decode.h"



extern HAL::GpioOutput LED_C13;
GpioOutput TglPin(B5);
volatile uint32_t Tim4_Count;

class TimerCallback : public Callback
{
  virtual void CallbackFunction()
  {
    TglPin.Toggle();
    Tim4_Count++;
  }
};

TimerCallback TimerCb;

void GPT_Test();
void PulseIn_Test();
void IR_Test();

volatile uint32_t CpuLoad;
RC5Frame_TypeDef RC5_FRAME;
char tim_buf[20];  


void Timer_Test()
{  
  tim_buf[11] = '\n';
  tim_buf[12] = '\r';
  TglPin.HwInit();

  //GPT_Test();
  PulseIn_Test();
  //IR_Test();
}


void GPT_Test()
{  
  static Gpt<(uint32_t)TIM4,8000000UL> gpt;
  gpt.HwInit();  
  while(1)
  {
#if 0
    TglPin.Low<PORTB,5>();
    gpt.DelayTicks(10);
    TglPin.High<PORTB,5>(); 
#endif

    if ( (gpt.GetTicks() % 1000) == 0)
    {
      intToStr(gpt.GetTicks(),tim_buf,10);
      HAL::DBG_PRINT((uint8_t*)tim_buf,13);
    }    
  }
}
  
void PulseIn_Test()
{  
  static HAL::PulseIn<(uint32_t)TIM3,TIM3_CH1_PIN_A6,8000000UL,nullptr> PulseInn;
  PulseInn.HwInit();  
  while(1)
  {
  
  }
}
  

void IR_Test()
{   
  IR_RC5_Init();
  while(1)
  {
    IR_RC5_Decode(&RC5_FRAME);    
  }
}
