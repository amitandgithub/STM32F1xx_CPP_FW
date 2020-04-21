


#include"CPP_HAL.h"
#include"Timer.h"
#include"Gpt.h"
#include"PulseIn.h"
#include"PulseOut.h"
#include"IrDecoder.h" 
#include"IrEncoder.h" 

#include "rc5_decode.h"
#include"DigitalOut.h"



extern HAL::GpioOutput LED_C13;
//GpioOutput TglPin(B5);

HAL::DigitalOut<B5> TglPin;
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
void IrDecoder_Test();
void IrEncoder_Test();

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
  PulseOut_Test();
  //IROut_Test();
  //IrDecoder_Test();
  //IrEncoder_Test();
}


void GPT_Test()
{  
  static Gpt<(uint32_t)TIM4,__LL_TIM_CALC_PSC(72000000, 1000000),0xffff,true> gpt;
  gpt.HwInit();  
  while(1)
  {
#if 0
    TglPin.Low<PORTB,5>();
    gpt.DelayTicks(10);
    TglPin.High<PORTB,5>(); 
#endif

    //if ( (gpt.GetTicks() % 500000) == 0)
    {
      //intToStr(gpt.GetTicks(),tim_buf,10);
      HAL::DBG_PRINT((uint8_t*)tim_buf,13);
    }    
  }
}

 static uint16_t PulseInBufA[72];
static uint16_t PulseInBufB[100];
 static PulseBuf_t PulseBuf = {PulseInBufA,sizeof(PulseInBufA)/sizeof(PulseInBufA[0]),0,nullptr};
#define EDGES 68
void PulseIn_Test()
{ 
 
  static HAL::PulseIn<(uint32_t)TIM4,TIM4_CH1_PIN_B6, Timer::FALLING_EDGE,Timer::RISING_EDGE> PulseInn; 
  PulseInn.HwInit(__LL_TIM_CALC_PSC(72000000, 1000000),0xffff,true);
  while(1)
  {
//    if(PulseBuf.Idx >= EDGES)
//    {
////      for( uint32_t i = 0; i<= EDGES; i++ )
////      {
////        PulseBuf.Buf[i] = (PulseBuf.Buf[i+1] >= PulseBuf.Buf[i]) ? (PulseBuf.Buf[i+1] - PulseBuf.Buf[i]) : (0xffff - PulseBuf.Buf[i] + PulseBuf.Buf[i+1]);         
////      }
//      PulseBuf.Idx = 0;
//      PulseInn.ICValue1 = 0;
//      PulseInn.ICValue2 = 0;
//    }
   // intToStr(PulseInn.GetTimerCounter(),tim_buf,10);
   // HAL::DBG_PRINT((uint8_t*)tim_buf,13);
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
  static bool InitDone = false;
  static uint8_t DutyCycle = 50;  
  static HAL::PulseOut<(uint32_t)TIM3,TIM3_CH3_PIN_B0,10000> Dimmer; //TIM3_CH1_PIN_A6 
  if(InitDone == false)
  {
    Dimmer.HwInit();
    Dimmer.SetDutyCycle(50);
  }
  
  while(1)
  {
   Dimmer.SetDutyCycle(DutyCycle%99);
   DutyCycle++;
   LL_mDelay(30); 
   return;
  }
}

extern void sendSAMSUNG(unsigned long data,  int nbits);
void IROut_Test()
{   
  static uint32_t IrCount =0;
  //sendSAMSUNG(0xAAAAAAAA,32);
  while(1)
  {
   sendSAMSUNG(IrCount++,32);
   LL_mDelay(10); 
  }
}

#define SAMSUNG_BITS          32
#define SAMSUNG_HDR_MARK    4500
#define SAMSUNG_HDR_SPACE   4500
#define SAMSUNG_BIT_MARK     560
#define SAMSUNG_ONE_SPACE   1600
#define SAMSUNG_ZERO_SPACE   560
#define SAMSUNG_RPT_SPACE   2250

uint32_t IR_Rx = 0;
void IrDecoder_Test()
{     
  static HAL::IrDecoder< 
  HAL::PulseIn<(uint32_t)TIM4,TIM4_CH1_PIN_B6, Timer::FALLING_EDGE,Timer::RISING_EDGE>,
  68,
  SAMSUNG_BITS,
  SAMSUNG_HDR_MARK,
  SAMSUNG_HDR_SPACE,
  SAMSUNG_BIT_MARK,
  SAMSUNG_ONE_SPACE,
  SAMSUNG_ZERO_SPACE,
  SAMSUNG_RPT_SPACE > IrInn;
  
  IrInn.HwInit();
  IrInn.Start();
  while(1)
  {
    if( IrInn.Decode(&IR_Rx) )
    {
      //intToStr(IR_Rx,tim_buf,10);
      //Hex32ToStr(IR_Rx,tim_buf);
      //HAL::DBG_PRINT((uint8_t*)tim_buf,13);
      printf("%d\n\r",IR_Rx);      
    }
  }
}

void IrEncoder_Test()
{   
  static uint32_t IrCnt = 0;
  static HAL::IrEncoder< 
  HAL::PulseOut<(uint32_t)TIM3,TIM3_CH3_PIN_B0,38000>,
  SAMSUNG_BITS,
  SAMSUNG_HDR_MARK,
  SAMSUNG_HDR_SPACE,
  SAMSUNG_BIT_MARK,
  SAMSUNG_ONE_SPACE,
  SAMSUNG_ZERO_SPACE,
  SAMSUNG_RPT_SPACE > IrOut;
  
  IrOut.HwInit();
  
  IrOut.Encode(0x55555555);
  LL_mDelay(100); 
  
  IrOut.Encode(0xAAAAAAAA);
  LL_mDelay(100);

  while(1)
  {
    IrOut.Encode(IrCnt++);
    LL_mDelay(20);
  }
}


