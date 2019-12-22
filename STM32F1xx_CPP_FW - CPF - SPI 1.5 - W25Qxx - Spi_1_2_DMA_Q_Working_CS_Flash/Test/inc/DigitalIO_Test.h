

#include "Callback.h"
#include"DigitalOut.h"
#include"DigitalIn.h"
#include"BtnInt.h" 
#include"BtnPolling.h"
#include"Led.h"
#include"RotaryEncoderPoll.h"


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
 
extern uint32_t Cw,CCw;
class OnCwCallback : public Callback
{
    void CallbackFunction()
    {
      Cw++; 
    }
};

class OnCCwCallback : public Callback
{
    void CallbackFunction()
    {
      CCw++; 
    }
};

void DigitalIO_tests();
void BtnPollTest();
void RotaryEncoderPollTest();