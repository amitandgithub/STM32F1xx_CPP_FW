

#include "Callback.h"
#include"DigitalOut.h"
#include"DigitalIn.h"
#include"BtnInt.h"

using namespace HAL;


extern volatile uint32_t IntrCount; 

class ButtonCallback : public Callback
{
    void CallbackFunction()
    {
      IntrCount++; 
    }
};

extern uint32_t OnRiseCount,OnFallCount;
      
class BtnOnRiseCallback : public Callback
{
    void CallbackFunction()
    {
      OnRiseCount++; 
    }
};

class BtnOnFallCallback : public Callback
{
    void CallbackFunction()
    {
      OnFallCount++; 
    }
};
  

void DigitalIO_tests();