

#include "Callback.h"
#include"DigitalOut.h"
#include"DigitalIn.h"

using namespace HAL;


extern volatile uint32_t IntrCount; 

class ButtonCallback : public Callback
{
    void CallbackFunction()
    {
      IntrCount++; 
    }
};


void DigitalIO_tests();