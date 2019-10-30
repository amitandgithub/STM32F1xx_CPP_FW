

#include "DigitalIO_Test.h"


volatile uint32_t IntrCount; 

ButtonCallback ButtonCallbackObj;

DigitalOut<C13> Led;

DigitalIn<A11,INPUT_PULLUP,INTERRUPT_ON_FALLING,reinterpret_cast <Callback *>(&ButtonCallbackObj)> Btn;

void DigitalIO_tests()
{
  
   Btn.HwInit();
   Led.HwInit();
   
  while(1)
  {
    
  }
  
  
}