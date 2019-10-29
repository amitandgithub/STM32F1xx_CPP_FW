



#include "Callback.h"
#include"DigitalInOut.h"
#include"DigitalOut.h"
#include"DigitalIn.h"


using namespace HAL;

volatile uint32_t IntrCount; 

class ButtonCallback : public Callback
{
    void CallbackFunction()
    {
      IntrCount++; 
    }
};


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