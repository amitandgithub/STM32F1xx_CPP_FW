
#include "IRremote.h"
#include "GpioInput.h"
#include"Gpt.h"

volatile irparams_t irparams;
decode_results results;

HAL::GpioInput IrReadPin(A6);

void HAL_TIM_PeriodElapsedCallback()
{
  uint8_t irdata = IrReadPin.Read(); //RECIV_PIN; // пин для приёма
  uint16_t irparams_timer;
  
  irparams.timer++;  // One more 50uS tick
  if(irparams.rawlen >= (RAWBUF-1)) irparams.rcvstate = STATE_OVERFLOW ;  // Buffer overflow
  
  switch(irparams.rcvstate)
  {
  case STATE_IDLE: // In the middle of a gap
    if(irdata == MARK)
    {
      if(irparams.timer < GAP_TICKS)  // Not big enough to be a gap.
      {
        irparams.timer = 0;
      }
      else
      {
        // Gap just ended; Record duration; Start recording transmission
        irparams.overflow = false;
        irparams.rawlen  = 0;
        irparams_timer = irparams.timer;
        irparams.rawbuf[irparams.rawlen++] = irparams_timer;
        irparams.timer = 0;
        irparams.rcvstate = STATE_MARK;
      }
    }
    
    break;
    
  case STATE_MARK:  // Timing Mark
    if(irdata == SPACE) // Mark ended; Record time
    {
      irparams_timer = irparams.timer;
      irparams.rawbuf[irparams.rawlen++] = irparams_timer;
      irparams.timer = 0;
      irparams.rcvstate = STATE_SPACE;
    }
    
    break;
    
  case STATE_SPACE:  // Timing Space
    if(irdata == MARK) // Space just ended; Record time
    {
      irparams_timer = irparams.timer;
      irparams.rawbuf[irparams.rawlen++] = irparams_timer;
      irparams.timer = 0;
      irparams.rcvstate = STATE_MARK;
      
    }
    else if(irparams.timer > GAP_TICKS) // Space
    {
      irparams.rcvstate = STATE_STOP;
    }
    
    break;
    
  case STATE_STOP:  // Waiting; Measuring Gap
    if(irdata == MARK) irparams.timer = 0 ;  // Reset gap timer
    break;
    
  case STATE_OVERFLOW:  // Flag up a read overflow; Stop the State Machine
    irparams.overflow = true;
    irparams.rcvstate = STATE_STOP;
    break;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
int MATCH(int measured, int desired)
{
  return ((measured >= TICKS_LOW(desired)) && (measured <= TICKS_HIGH(desired)));
}

int MATCH_MARK(int measured_ticks, int desired_us)
{
  return ((measured_ticks >= TICKS_LOW(desired_us + MARK_EXCESS)) && (measured_ticks <= TICKS_HIGH(desired_us + MARK_EXCESS)));
}

int MATCH_SPACE(int measured_ticks, int desired_us)
{
  return ((measured_ticks >= TICKS_LOW(desired_us - MARK_EXCESS)) && (measured_ticks <= TICKS_HIGH(desired_us - MARK_EXCESS)));
}

//////////////////////////////////////////irRecv.c//////////////////////////////////////////////////

int16_t my_decode(decode_results *results)
{
  results->rawbuf = irparams.rawbuf;
  results->rawlen = irparams.rawlen;
  results->overflow = irparams.overflow;
  
  if(irparams.rcvstate != STATE_STOP) return false;
  
#if DECODE_NEC
  if(decodeNEC(results)) return true;
#endif
  
#if DECODE_SONY
  if(decodeSony(results)) return true;
#endif
  
#if DECODE_SANYO
  if(decodeSanyo(results)) return true;
#endif
  
#if DECODE_MITSUBISHI
  if(decodeMitsubishi(results)) return true;
#endif
  
#if DECODE_RC5
  if(decodeRC5(results)) return true;
#endif
  
#if DECODE_RC6
  if(decodeRC6(results)) return true;
#endif
  
#if DECODE_PANASONIC
  if(decodePanasonic(results)) return true;
#endif
  
#if DECODE_LG
  if(decodeLG(results)) return true;
#endif
  
#if DECODE_JVC
  if(decodeJVC(results)) return true;
#endif
  
#if DECODE_SAMSUNG
  if(decodeSAMSUNG(results)) return true;
#endif
  
#if DECODE_WHYNTER
  if(decodeWhynter(results)) return true;
#endif
  
#if DECODE_AIWA_RC_T501
  if(decodeAiwaRCT501(results)) return true;
#endif
  
#if DECODE_DENON
  if(decodeDenon(results)) return true;
#endif
  
  if(decodeHash(results)) return true;
  my_resume();
  return false;
}

#include"Timer.h"
#include"PulseIn.h"
#include"Gpt.h"
#define TIMx TIM3
void my_enableIRIn() // initialization
{
  static HAL::Gpt<(uint32_t)TIM3,MYPRESCALER,MYPERIOD,true> IR_gpt; 
  
  DWT_Init();
#if 0
  HAL_TIM_Base_DeInit(&htim4);
  
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = MYPRESCALER;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = MYPERIOD;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim4);
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig);
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig);
#endif
  IrReadPin.HwInit(INPUT_PULLDOWN);
  IR_gpt.HwInit();
  
  irparams.rcvstate = STATE_IDLE;
  irparams.rawlen = 0;
}

uint8_t my_isIdle() // Return if receiving new IR signals
{
  return (irparams.rcvstate == STATE_IDLE || irparams.rcvstate == STATE_STOP) ? true : false;
}

void my_resume() // Restart the ISR state machine
{
  irparams.rcvstate = STATE_IDLE;
  irparams.rawlen = 0;
}

int compare(unsigned int oldval, unsigned int newval)
{
  if(newval < oldval * .8) return 0;
  else if(oldval < newval * .8) return 2;
  else return 1;
}

#define FNV_PRIME_32 16777619
#define FNV_BASIS_32 (int32_t)2166136261

int32_t decodeHash(decode_results *results)
{
  int32_t hash = FNV_BASIS_32;
  int16_t value;
  unsigned int Raw1, Raw2;
  
  // Require at least 6 samples to prevent triggering on noise
  if(results->rawlen < 6) return false;
  
  for(int16_t i = 1; (i + 2) < results->rawlen; i++)
  {
    Raw1 = results->rawbuf[i];
    Raw2 = results->rawbuf[i+2];
    value = compare(Raw1, Raw2);
    hash = (hash * FNV_PRIME_32) ^ value; // Add value into the hash
  }
  
  results->value = hash;
  results->bits = 32;
  results->decode_type = UNKNOWN;
  return true;
}





