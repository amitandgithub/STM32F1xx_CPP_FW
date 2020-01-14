

#include "DigitalIO_Test.h"



HAL::GpioOutput LED_C13(A0);
//HAL::GpioInput LED_IN(B9);//INPUT_PULLUP

volatile uint32_t IntrCount; 

ButtonCallback ButtonCallbackObj;


/* DigitalOut context*/
//HAL::DigitalOut<A0> LedC13;
//BSP::Led<A0> LedC13;

/* DigitalIn context*/
DigitalIn<B9,INPUT_PULLDOWN,INTERRUPT_ON_RISING,reinterpret_cast <Callback *>(&ButtonCallbackObj)> Btn; 
GpioInput BtnObj(B9,INPUT_PULLUP,INTERRUPT_ON_FALLING,reinterpret_cast <Callback *>(&ButtonCallbackObj) );

/* Button context*/
uint32_t OnRiseCount,OnFallCount;
BtnOnRiseCallback BtnOnRiseCallbackObj;
BtnOnFallCallback BtnOnFallCallbackObj;
BSP::BtnInt<B9,INPUT_PULLUP,INTERRUPT_ON_RISING_FALLING,reinterpret_cast <Callback *>(&BtnOnRiseCallbackObj),reinterpret_cast <Callback *>(&BtnOnFallCallbackObj)> A11_Btn;
BSP::BtnPolling<B2,INPUT_PULLUP,5> A11_BtnPolling;

/* RotaryEncoderPoll context*/
#define ROTARY_PINS B2,B1,B0,reinterpret_cast <Callback *>(&OnCwCallbackObj),reinterpret_cast <Callback *>(&OnCCwCallbackObj),reinterpret_cast <Callback *>(&BtnOnRiseCallbackObj),reinterpret_cast <Callback *>(&BtnOnFallCallbackObj)

OnCwCallback OnCwCallbackObj;
OnCCwCallback OnCCwCallbackObj;
BSP::RotaryEncoderPoll<ROTARY_PINS> RotaryEncoderPollObj;

uint32_t Cw,CCw;
void DigitalIO_tests()
{
  static bool InitDone;
  
  if(InitDone == false)
  {
    LED_C13.HwInit();
    //LedC13.HwInit();
     //Btn.HwInit();
    //LedC13.HwInit();
    //A11_Btn.HwInit();
    //RotaryEncoderPollObj.HwInit();
    InitDone = true;
  }
  
  while(1)
  {      
    LED_C13.High<A0>(); // 8 cycles
    LED_C13.Low<A0>(); // 8 cycles
    
//    LedC13.High(); // 8 cycles
//    LedC13.Low(); // 8 cycles
    
//    LED_C13.High(); // 11 cycles
//    LED_C13.Low(); // 11 cycles
    
   // BtnPollTest();
    //RotaryEncoderPollTest();    
    //return;
  }  
  
}

void BtnPollTest()
{
  typename BSP::BtnPolling<B2,INPUT_PULLUP,5>::BtnState_t BtnState;
  
  BtnState = A11_BtnPolling.Run();
  
    if(BtnState == BSP::BtnPolling<B2,INPUT_PULLUP,5>::BTN_FALL)
    {
      OnFallCount++;
    }
    else if(BtnState == BSP::BtnPolling<B2,INPUT_PULLUP,5>::BTN_RISE)
    {
      OnRiseCount++;
    }  
}

void RotaryEncoderPollTest()
{
  typename BSP::RotaryEncoderPoll<ROTARY_PINS>::RotaryState_t RotaryState;
  typename BSP::RotaryEncoderPoll<ROTARY_PINS>::RotaryState_t BtnState;
  
    RotaryEncoderPollObj.Run();
    
    RotaryState = RotaryEncoderPollObj.getRotaryState();
    BtnState    = RotaryEncoderPollObj.getButtonState();
    
    if(RotaryState == BSP::RotaryEncoderPoll<ROTARY_PINS>::ROTATING_CLOCKWISE)
    {
      Cw++;
    }
    else if(RotaryState == BSP::RotaryEncoderPoll<ROTARY_PINS>::ROTATING_ANTI_CLOCKWISE)
    {
      CCw++;
    }
    
    if(BtnState == BSP::RotaryEncoderPoll<ROTARY_PINS>::BTN_FALL)
    {
      OnFallCount++;
    }
    else if(BtnState == BSP::RotaryEncoderPoll<ROTARY_PINS>::BTN_RISE)
    {
      OnRiseCount++;
    } 
}














