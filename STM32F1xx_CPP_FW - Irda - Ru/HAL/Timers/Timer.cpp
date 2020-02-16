/******************
** FILE: Timer.cpp
**
** DESCRIPTION:
**  Timer implementation
**
** CREATED: 8/11/2019, by Amit Chaudhary
******************/

#include "Timer.h"

namespace HAL
{
  GpioOutput TglPin(B5);
  
  Timer::Timer(TIMx timerx) : m_TIMx(timerx), m_TimerCb(nullptr),m_TimerCBCC1(nullptr),m_TimerCBCC2(nullptr)
  {
    
  }
  
  Timer::TimerStatus_t Timer::HwInit()
  {
    LL_TIM_InitTypeDef TIM_InitStruct;
    
    TIM_InitStruct.Prescaler         = __LL_TIM_CALC_PSC (72000000, 8000000);
    TIM_InitStruct.CounterMode       = LL_TIM_COUNTERMODE_DOWN;
    TIM_InitStruct.Autoreload        = 0xFFFFFFFFU;
    TIM_InitStruct.ClockDivision     = LL_TIM_CLOCKDIVISION_DIV1;
    TIM_InitStruct.RepetitionCounter = (uint8_t)0x00;
    
    if (m_TIMx == TIM2)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM2_IRQn,0,this);  
    }
#if defined(TIM1)
    else if (m_TIMx == TIM1)
    {
      LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
      // All Timer 1 interrupts share one ISR, can use 
      // different ISR for each IRQ if required
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM1_BRK_IRQn,0,this); 
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM1_UP_IRQn,0,this);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM1_TRG_COM_IRQn,0,this);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM1_CC_IRQn,0,this);
    }
#endif
#if defined(TIM3)
    else if (m_TIMx == TIM3)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM3_IRQn,0,this);  
    }
#endif
#if defined(TIM4)
    else if (m_TIMx == TIM4)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM4_IRQn,0,this);  
    }
#endif
#if defined(TIM5)
    else if (m_TIMx == TIM5)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM5_IRQn,0,this);  
    }
#endif
#if defined(TIM6)
    else if (m_TIMx == TIM6)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM6_IRQn,0,this);  
    }
#endif
#if defined (TIM7)
    else if (m_TIMx == TIM7)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM7_IRQn,0,this);  
    }
#endif
#if defined(TIM8)
    else if (m_TIMx == TIM8)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM8);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM8_IRQn,0,this);  
    }
#endif
#if defined(TIM9)
    else if (m_TIMx == TIM9)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM9);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM9_IRQn,0,this);  
    }
#endif
#if defined(TIM10)
    else if (m_TIMx == TIM10)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM10);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM10_IRQn,0,this);  
    }
#endif
#if defined(TIM11)
    else if (m_TIMx == TIM11)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM11);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM11_IRQn,0,this);  
    }
#endif
#if defined(TIM12)
    else if (m_TIMx == TIM12)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM12);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM12_IRQn,0,this);  
    }
#endif
#if defined(TIM13)
    else if (m_TIMx == TIM13)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM13);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM13_IRQn,0,this);  
    }
#endif
#if defined(TIM14)
    else if (m_TIMx == TIM14)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM14_IRQn,0,this);  
    }
#endif
#if defined(TIM15)
    else if (m_TIMx == TIM15)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM15);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM15_IRQn,0,this);  
    }
#endif
#if defined(TIM16)
    else if (m_TIMx == TIM16)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM16);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM16_IRQn,0,this);  
    }
#endif
#if defined(TIM17)
    else if (m_TIMx == TIM17)
    {
      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM17);
      InterruptManagerInstance.RegisterDeviceInterrupt(TIM17_IRQn,0,this);  
    }
#endif
    else
    {
      m_TimerStatus = TIMER_INVALID_PARAMS;
      return TIMER_INVALID_PARAMS;
    }  
    
    LL_TIM_Init(m_TIMx, &TIM_InitStruct);
    
    //LL_TIM_SetOnePulseMode(m_TIMx,LL_TIM_ONEPULSEMODE_SINGLE);
    
    //LL_TIM_ClearFlag_UPDATE(m_TIMx);      
    
    return TIMER_OK;
  }
  
  void Timer::PinsHwInit(Chanel_t Chanel)
  {
    Port_t port;
    PIN_t pin;
    
    switch(Chanel)
    {
    case TIM1_CH1_PIN_A8	: port = PORTA; pin = 8; break;
    case TIM1_CH2_PIN_A9	: port = PORTA; pin = 9; break;
    case TIM1_CH3_PIN_A10	: port = PORTA; pin = 10; break;
    case TIM1_CH4_PIN_A11	: port = PORTA; pin = 11; break;
    case TIM1_ETR_PIN_A12	: port = PORTA; pin = 12; break;
    case TIM1_CH1N_PIN_B13    : port = PORTB; pin = 13; break; 
    case TIM1_CH1N_PIN_A7	: port = PORTA; pin = 7; break;
    case TIM1_CH2N_PIN_B14    : port = PORTB; pin = 14; break; 
    case TIM1_CH3N_PIN_B15    : port = PORTB; pin = 15; break; 
    case TIM1_CH3N_PIN_B1	:  port = PORTB; pin = 1; break;
    
    case TIM2_CH1_PIN_A0	: port = PORTA; pin = 0; break;
    case TIM2_CH2_PIN_A1	: port = PORTA; pin = 1; break;
    case TIM2_CH3_PIN_B10	: port = PORTB; pin = 10; break; 
    case TIM2_CH3_PIN_A2	: port = PORTA; pin = 2; break;
    case TIM2_CH4_PIN_B11	: port = PORTB; pin = 11; break; 
    case TIM2_CH4_PIN_A3	: port = PORTA; pin = 3; break;
    case TIM2_ETR_PIN_A0	: port = PORTA; pin = 0; break;
    
    case TIM3_CH1_PIN_B4	: port = PORTB; pin = 4; break;
    case TIM3_CH1_PIN_A6	: port = PORTA; pin = 6; break;
    case TIM3_CH2_PIN_B5	: port = PORTB; pin = 5; break;
    case TIM3_CH2_PIN_A7	: port = PORTA; pin = 7; break;
    case TIM3_CH3_PIN_B0	: port = PORTB; pin = 0; break;
    case TIM3_CH4_PIN_B1	: port = PORTB; pin = 1; break;
    
    case TIM4_CH1_PIN_B6	: port = PORTB; pin = 6; break;
    case TIM4_CH2_PIN_B7	: port = PORTB; pin = 7; break;
    case TIM4_CH3_PIN_B8	: port = PORTB; pin = 8; break;
    case TIM4_CH4_PIN_B9	: port = PORTB; pin = 9; break;
    default : break;
    }
    
    {
      GpioInput       TimerPin(port,pin);
      TimerPin.HwInit(INPUT_PULLDOWN);  // Input , No Pull
    }
  }
  
  void Timer::usDelay(uint16_t delay)
  {
    //DisableInterrupts(INTR_ALL);
    LL_TIM_SetCounter(m_TIMx,delay*8);
    
    LL_TIM_ClearFlag_UPDATE(m_TIMx);    
    
    EnableTimerCount();    
    
    while(LL_TIM_IsActiveFlag_UPDATE(m_TIMx) == 0);   
  }
  
  
  void Timer::Start(uint16_t delay)
  {
    LL_TIM_SetCounter(m_TIMx,delay*8);
    
    LL_TIM_ClearFlag_UPDATE(m_TIMx);    
    
    EnableTimerCount();    
  }
  
  void Timer::StartTicking(uint16_t TickFrequency)
  {
    LL_TIM_SetOnePulseMode(m_TIMx,LL_TIM_ONEPULSEMODE_REPETITIVE);    
    DisableInterrupts(INTR_ALL);
    LL_TIM_EnableARRPreload(m_TIMx);
    LL_TIM_SetCounter(m_TIMx,TickFrequency*8);
    LL_TIM_SetAutoReload(m_TIMx,TickFrequency*8);
    LL_TIM_ClearFlag_UPDATE(m_TIMx);     
    m_Ticks = 0;
    EnableTimerCount();    
    EnableInterrupts(INTR_UIE);
  }  
  
  
  void Timer::PulseIn_HwInit(Chanel_t channel, uint8_t* Buf, uint16_t len)
  {
    LL_TIM_IC_InitTypeDef TIM_ICInitStruct;
    
    HwInit();
    
    PinsHwInit(channel);
    
    TIM_ICInitStruct.ICPolarity    = LL_TIM_IC_POLARITY_RISING;
    TIM_ICInitStruct.ICActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
    TIM_ICInitStruct.ICPrescaler   = LL_TIM_ICPSC_DIV1;
    TIM_ICInitStruct.ICFilter      = LL_TIM_IC_FILTER_FDIV1;
    
    LL_TIM_IC_Init(m_TIMx,LL_TIM_CHANNEL_CH1,&TIM_ICInitStruct);
    
    TIM_ICInitStruct.ICPolarity    = LL_TIM_IC_POLARITY_FALLING;
    
    LL_TIM_IC_Init(m_TIMx,LL_TIM_CHANNEL_CH2,&TIM_ICInitStruct);
    
    LL_TIM_SetTriggerInput(m_TIMx,LL_TIM_TS_TI1FP1);
    
    LL_TIM_SetSlaveMode(m_TIMx,LL_TIM_SLAVEMODE_RESET); 
    
    LL_TIM_EnableMasterSlaveMode(m_TIMx);
    
    DisableInterrupts(INTR_ALL);
    
    EnableInterrupts( INTR_CC1IE | INTR_CC2IE);    //INTR_UIE
  }
  
  void Timer::ISR()
  {
    if(LL_TIM_IsActiveFlag_UPDATE(m_TIMx))
    {
      m_Ticks++;
      //TglPin.Toggle();
      LL_TIM_ClearFlag_UPDATE(m_TIMx);
      //if(m_TimerCb) m_TimerCb->CallbackFunction();
    }
    
    if(LL_TIM_IsActiveFlag_CC1(m_TIMx))
    {
      LL_TIM_ClearFlag_CC1(m_TIMx);
      ICValue2 = LL_TIM_OC_GetCompareCH1(m_TIMx);
      //if(m_TimerCBCC1) m_TimerCBCC1->CallbackFunction();
    }
    
    if(LL_TIM_IsActiveFlag_CC2(m_TIMx))
    {
      LL_TIM_ClearFlag_CC2(m_TIMx);
      ICValue1 = LL_TIM_OC_GetCompareCH2(m_TIMx);
      //if(m_TimerCBCC2) m_TimerCBCC2->CallbackFunction();
    }  
    
    
  }
  
  
  
  
  
  
  
}