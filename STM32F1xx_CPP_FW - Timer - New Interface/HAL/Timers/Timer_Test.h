


#include"CPP_HAL.h"
#include"Timer.h"
#include"Gpt.h"
#include"PulseIn.h"
#include"PulseOut.h"

#include "rc5_decode.h"



extern HAL::GpioOutput LED_C13;
GpioOutput TglPin(B5);
volatile uint32_t Tim4_Count;

class TimerCallback : public Callback
{
  virtual void CallbackFunction()
  {
    TglPin.Toggle();
    Tim4_Count++;
  }
};

TimerCallback TimerCb;

void GPT_Test();
void PulseIn_Test();
void IR_Test();
void IR_Russia_Test();
void PulseOut_Test();
void IROut_Test();
volatile uint32_t CpuLoad;
RC5Frame_TypeDef RC5_FRAME;
char tim_buf[20];  


void Timer_Test()
{  
  tim_buf[11] = '\n';
  tim_buf[12] = '\r';
  TglPin.HwInit();

  //GPT_Test();
  //PulseIn_Test();
  //IR_Test();
  //IR_Russia_Test();
  //PulseOut_Test();
   IROut_Test();
}


void GPT_Test()
{  
  static Gpt<(uint32_t)TIM4,71,0xffff,true> gpt;
  gpt.HwInit();  
  while(1)
  {
#if 0
    TglPin.Low<PORTB,5>();
    gpt.DelayTicks(10);
    TglPin.High<PORTB,5>(); 
#endif

    if ( (gpt.GetTicks() % 500000) == 0)
    {
      intToStr(gpt.GetTicks(),tim_buf,10);
      HAL::DBG_PRINT((uint8_t*)tim_buf,13);
    }    
  }
}
  
void PulseIn_Test()
{  
  static HAL::PulseIn<(uint32_t)TIM2,TIM2_CH1_PIN_A0, Timer::FALLING_EDGE,Timer::RISING_EDGE,72,nullptr> PulseInn; 
  PulseInn.HwInit(__LL_TIM_CALC_PSC(72000000, 1000000),65000,true);  //__LL_TIM_CALC_PSC(72000000, 1000000)
  while(1)
  {
    intToStr(PulseInn.GetTimerCounter(),tim_buf,10);
    HAL::DBG_PRINT((uint8_t*)tim_buf,13);
  }
}
  

void IR_Test()
{   
  IR_RC5_Init();
  while(1)
  {
    IR_RC5_Decode(&RC5_FRAME);    
  }
}


#include "IRremote.h"

extern decode_results results;

void IR_Russia_Test()
{   
  char trans_str[96] = {0,};
  static const char *decode_str[] = {"UNUSED", "UNKNOWN", "RC5", "RC6", "NEC", "SONY", "PANASONIC", "JVC", "SAMSUNG", "WHYNTER", "AIWA_RC_T501", "LG", "SANYO", "MITSUBISHI", "DISH", "SHARP", "DENON", "PRONTO"};  
  
  my_enableIRIn();  
 
  while(1)
  {
    if(my_decode(&results))
    {

      snprintf(trans_str, 96, "Cod: %p | Type: %s | Bits: %d\n", (void*)results.value, decode_str[results.decode_type + 1], results.bits);
      
      HAL::DBG_PRINT( (uint8_t*)trans_str, strlen(trans_str));
      HAL::DBG_PRINT_NL();
      LL_mDelay(20); 
      
      my_resume();                
    }
  }
}


void PulseOut_Test()
{   
  uint8_t DutyCycle = 50;
  static HAL::PulseOut<(uint32_t)TIM3,TIM3_CH4_PIN_B1,10000> Dimmer; 
  Dimmer.HwInit();
  Dimmer.SetDutyCycle(50);
  while(1)
  {
   Dimmer.SetDutyCycle(DutyCycle%99);
   DutyCycle++;
   LL_mDelay(30); 
  }
}

extern void sendSAMSUNG(unsigned long data,  int nbits);
void IROut_Test()
{   
  uint32_t var = 0xFFFF0000; 
  while(1)
  {
   sendSAMSUNG(var++,32);
   LL_mDelay(100); 
  }
}

