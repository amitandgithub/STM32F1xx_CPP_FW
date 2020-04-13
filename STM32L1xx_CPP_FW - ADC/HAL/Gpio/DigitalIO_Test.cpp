

#include "DigitalIO_Test.h"

void DigitalIn_Tests();
void DigitalOut_Tests();

extern HAL::GpioOutput LED;
//HAL::GpioInput LED_IN(B9);//INPUT_PULLUP

volatile uint32_t IntrCount; 

ButtonCallback ButtonCallbackObj;


/* DigitalOut context*/
//HAL::DigitalOut<A0> LedC13;
//BSP::Led<A0> LedC13;


//GpioInput BtnObj(B9);

/* Button context*/
uint32_t OnRiseCount,OnFallCount;
BtnOnRiseCallback BtnOnRiseCallbackObj;
BtnOnFallCallback BtnOnFallCallbackObj;
BSP::BtnInt_t<B9,INPUT_PULLUP,INTERRUPT_ON_RISING_FALLING,reinterpret_cast <Callback *>(&BtnOnRiseCallbackObj),reinterpret_cast <Callback *>(&BtnOnFallCallbackObj)> A11_Btn;
BSP::BtnPolling_t<B2,INPUT_PULLUP,5> A11_BtnPolling;




/* RotaryEncoderPoll context*/
#define ROTARY_PINS B2,B1,B0,reinterpret_cast <Callback *>(&OnCwCallbackObj),reinterpret_cast <Callback *>(&OnCCwCallbackObj),reinterpret_cast <Callback *>(&BtnOnRiseCallbackObj),reinterpret_cast <Callback *>(&BtnOnFallCallbackObj)

OnCwCallback OnCwCallbackObj;
OnCCwCallback OnCCwCallbackObj;
//BSP::RotaryEncoderPoll<ROTARY_PINS> RotaryEncoderPollObj;

uint32_t Cw,CCw;
void DigitalIO_tests()
{
  
  DigitalOut_Tests();
  //DigitalIn_Tests(); 
  
}

void Btn_Poll_Test()
{
  BSP::BtnPoll B9_BtnPolling(B9) ;
  B9_BtnPolling.HwInit(INPUT_PULLUP);
  
  while(1)
  {    
    if( B9_BtnPolling.Read())
      LED.Low();
    else
      LED.High();
  }  
}

void Btn_Intr_Test()
{
  BSP::BtnInt B9_BtnInt(B9);
  B9_BtnInt.HwInit(INPUT_PULLUP,INTERRUPT_ON_RISING,reinterpret_cast <Callback *>(&BtnOnRiseCallbackObj),reinterpret_cast <Callback *>(&BtnOnFallCallbackObj));
  
  while(1)
  { 

  }  
}


void HwButton_Intr_Test()
{
   
//  static BSP::HwButton B9_HwBtnInt(B9);
//  B9_HwBtnInt.HwInit(INPUT_PULLDOWN,NO_INTERRUPT,reinterpret_cast <Callback *>(&BtnOnRiseCallbackObj),reinterpret_cast <Callback *>(&BtnOnFallCallbackObj));
//  
//  while(1)
//  { 
//       B9_HwBtnInt.Run();
//  }  
}




void BtnPollTest()
{
  typename BSP::BtnPolling_t<B2,INPUT_PULLUP,5>::BtnState_t BtnState;
  
  BtnState = A11_BtnPolling.Run();
  
    if(BtnState == BSP::BtnPolling_t<B2,INPUT_PULLUP,5>::BTN_FALL)
    {
      OnFallCount++;
    }
    else if(BtnState == BSP::BtnPolling_t<B2,INPUT_PULLUP,5>::BTN_RISE)
    {
      OnRiseCount++;
    }  
}

//void RotaryEncoderPollTest()
//{
//  typename BSP::RotaryEncoderPoll<ROTARY_PINS>::RotaryState_t RotaryState;
//  typename BSP::RotaryEncoderPoll<ROTARY_PINS>::RotaryState_t BtnState;
//  
//    RotaryEncoderPollObj.Run();
//    
//    RotaryState = RotaryEncoderPollObj.getRotaryState();
//    BtnState    = RotaryEncoderPollObj.getButtonState();
//    
//    if(RotaryState == BSP::RotaryEncoderPoll<ROTARY_PINS>::ROTATING_CLOCKWISE)
//    {
//      Cw++;
//    }
//    else if(RotaryState == BSP::RotaryEncoderPoll<ROTARY_PINS>::ROTATING_ANTI_CLOCKWISE)
//    {
//      CCw++;
//    }
//    
//    if(BtnState == BSP::RotaryEncoderPoll<ROTARY_PINS>::BTN_FALL)
//    {
//      OnFallCount++;
//    }
//    else if(BtnState == BSP::RotaryEncoderPoll<ROTARY_PINS>::BTN_RISE)
//    {
//      OnRiseCount++;
//    } 
//}


    //LED.High<A0>(); // 8 cycles
    //LED.Low<A0>(); // 8 cycles
    
//    LedC13.High(); // 8 cycles
//    LedC13.Low(); // 8 cycles
    
//    LED.High(); // 11 cycles
//    LED.Low(); // 11 cycles


/* DigitalIn context*/
DigitalIn<B0,INPUT_PULLDOWN,INTERRUPT_ON_RISING,reinterpret_cast <Callback *>(&ButtonCallbackObj)> Btn1; // 196 bytes -> 42 bytes, 4 RAM
DigitalIn<B1,INPUT_PULLDOWN,INTERRUPT_ON_RISING,reinterpret_cast <Callback *>(&ButtonCallbackObj)> Btn2; 
DigitalIn<B2,INPUT_PULLDOWN,INTERRUPT_ON_RISING,reinterpret_cast <Callback *>(&ButtonCallbackObj)> Btn3; 

void DigitalIn_Tests()
{
  uint32_t High;  
  Btn1.HwInit();
  Btn2.HwInit();
  Btn3.HwInit();
  
  while(1)
  {
    if(Btn1.Read() == true)
    {
      High++;
    }
    
    if(Btn2.Read() == true)
    {
      High++;
    }
    
    if(Btn3.Read() == true)
    {
      High++;
    }
  }
  
}

void DigitalOut_Tests()
{
  HAL::DigitalOut<A2> LedC13;
  LedC13.HwInit();
  while(1)
  {
    LedC13.High();
    LL_mDelay(300);
    LedC13.Low();
    LL_mDelay(300);
  }
  
}







