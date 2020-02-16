


#include"CPP_HAL.h"
#include"Timer.h"



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
HAL::Timer gpt(TIM3);
volatile uint32_t CpuLoad;

void Timer_Test()
{
  uint32_t Loc;
  char tim_buf[20];  
  tim_buf[11] = '\n';
  tim_buf[12] = '\r';
  TglPin.HwInit();
  gpt.HwInit();
   
  gpt.PulseIn_HwInit(HAL::Timer::TIM3_CH1_PIN_B4,nullptr,1);
  gpt.StartTicking(10);
  while(1)
  {
//    gpt.Start(10);
//    
//    TglPin.Low();
//    gpt.Wait();
//    TglPin.High();
    
//    TglPin.Low();
//    gpt.usDelay(10);
//    TglPin.High();
    
//    TglPin.Low();
//    gpt.usDelay_T<8>();    
//    TglPin.High(); 

//    Loc = Tim4_Count;
//    gpt.usDelay_T<3,reinterpret_cast <Callback *>(&TimerCb)>(); 
//    while(Loc == Tim4_Count);
    
//        TglPin.Low();
//        while( (gpt.GetTicks() % 10000) == 0);    
//        TglPin.High(); 
//        while( (gpt.GetTicks() % 10000) == 0);   
   // CpuLoad++; 
    if ( (gpt.GetTicks() % 1000) == 0)
    {
      intToStr(gpt.GetTicks(),tim_buf,10);
      HAL::DBG_PRINT((uint8_t*)tim_buf,13);
    }
  }
}
  
  