

#include "DigitalIO_Test.h"


volatile uint32_t IntrCount; 

ButtonCallback ButtonCallbackObj;


/* DigitalOut context*/
BSP::Led<C13> LedC13;

/* DigitalIn context*/
DigitalIn<A11,INPUT_PULLUP,INTERRUPT_ON_FALLING,reinterpret_cast <Callback *>(&ButtonCallbackObj)> Btn;

/* Button context*/
uint32_t OnRiseCount,OnFallCount;
BtnOnRiseCallback BtnOnRiseCallbackObj;
BtnOnFallCallback BtnOnFallCallbackObj;
BSP::BtnInt<A11,INPUT_PULLUP,INTERRUPT_ON_RISING_FALLING,reinterpret_cast <Callback *>(&BtnOnRiseCallbackObj),reinterpret_cast <Callback *>(&BtnOnFallCallbackObj)> A11_Btn;
BSP::BtnPolling<A11,INPUT_PULLUP,5,reinterpret_cast <Callback *>(&BtnOnRiseCallbackObj),reinterpret_cast <Callback *>(&BtnOnFallCallbackObj)> A11_BtnPolling;

/* RotaryEncoderPoll context*/
BSP::RotaryEncoderPoll<A1,A2,A3> RotaryEncoderPollObj;



void DigitalIO_tests()
{
  static bool InitDone;
  
  if(InitDone == false)
  {
   // Btn.HwInit();
    LedC13.HwInit();
    A11_Btn.HwInit();
    RotaryEncoderPollObj.HwInit();
    InitDone = true;
  }
  
  while(1)
  {
    LedC13.Blink();
    return;
  }  
  
}


