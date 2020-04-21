

#include"Callback.h"
#include"DigitalOut.h"
#include"DigitalIn.h"
#include"BtnInt_t.h" 
#include"BtnPolling_t.h"
#include"BtnInt.h" 
#include"BtnPoll.h"
//#include"Led.h"
//#include"RotaryEncoderPoll.h"
#include"GpioOutput.h"
#include"GpioInput.h"
#include"HwButton.h"

using namespace HAL;

void Btn_Poll_Test();
void Btn_Intr_Test();
void HwButton_Intr_Test();
extern volatile uint32_t IntrCount; 
extern HAL::GpioOutput LED;

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
      LED.Low();
    }
};

class BtnOnFallCallback : public Callback
{
    void CallbackFunction()
    {
      OnFallCount++; 
      LED.High();
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