

#include "DigitalIO_Test.h"


volatile uint32_t IntrCount; 

ButtonCallback ButtonCallbackObj;


/* DigitalOut context*/
DigitalOut<C13> Led;

/* DigitalIn context*/
DigitalIn<A11,INPUT_PULLUP,INTERRUPT_ON_FALLING,reinterpret_cast <Callback *>(&ButtonCallbackObj)> Btn;

/* Button context*/
uint32_t OnRiseCount,OnFallCount;
BtnOnRiseCallback BtnOnRiseCallbackObj;
BtnOnFallCallback BtnOnFallCallbackObj;
BSP::BtnInt<A11,INPUT_PULLUP,INTERRUPT_ON_RISING_FALLING,reinterpret_cast <Callback *>(&BtnOnRiseCallbackObj),reinterpret_cast <Callback *>(&BtnOnFallCallbackObj)> A8_Btn;

void DigitalIO_tests()
{
  static bool InitDone;
  
  if(InitDone == false)
  {
   // Btn.HwInit();
   // Led.HwInit();
    A8_Btn.HwInit();
    InitDone = true;
  }
  
  while(1)
  {
    
    return;
  }  
  
}


