/******************
** CLASS: Timer
**
** DESCRIPTION:
**  Implements the Timer driver
**
** CREATED: 7 Feb 2020, by Amit Chaudhary
**
** FILE: Timer.h
**
******************/
#ifndef Timer_h
#define Timer_h

#include"chip_peripheral.h"
#include"CPP_HAL.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Queue.h"
#include"DebugLog.h"
#include"DigitalOut.h"
#include"GpioOutput.h"
#include"GpioInput.h"

#include"DMA.h"
#include <stdio.h>


extern HAL::DMA dma1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::InterruptManager InterruptManagerInstance;   


namespace HAL
{ 
  
  class Timer : public InterruptSource
  {
  public:
    
    typedef TIM_TypeDef* TIMx;
    typedef  Callback* TimerCb_t;
    
    typedef enum  : uint8_t
    {
      TIMER_OK,      
      TIMER_BUSY,
      TIMER_INVALID_PARAMS,
    }TimerStatus_t;
    
    enum
    {
      TIMER_CH1 = LL_TIM_CHANNEL_CH1,
      TIMER_CH2 = LL_TIM_CHANNEL_CH2,
      TIMER_CH3 = LL_TIM_CHANNEL_CH3,
      TIMER_CH4 = LL_TIM_CHANNEL_CH4,
    };
      
    enum
    {
      INTR_UIE          = TIM_DIER_UIE,
      INTR_CC1IE        = TIM_DIER_CC1IE,
      INTR_CC2IE        = TIM_DIER_CC2IE,
      INTR_CC3IE        = TIM_DIER_CC3IE,
      INTR_CC4IE        = TIM_DIER_CC4IE,
      INTR_ALL          = TIM_DIER_UIE | TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE,
    };
      
    typedef enum : uint8_t
    {
      /* Timer 1*/
      TIM1_CH1_PIN_A8,
      TIM1_CH2_PIN_A9,
      TIM1_CH3_PIN_A10,
      TIM1_CH4_PIN_A11,
      TIM1_ETR_PIN_A12,
      TIM1_CH1N_PIN_B13,
      TIM1_CH1N_PIN_A7,
      TIM1_CH2N_PIN_B14,
      TIM1_CH3N_PIN_B15,
      TIM1_CH3N_PIN_B1,
      
      /* Timer 2*/
      TIM2_CH1_PIN_A0,
      TIM2_CH2_PIN_A1,
      TIM2_CH3_PIN_B10,
      TIM2_CH3_PIN_A2,
      TIM2_CH4_PIN_B11,
      TIM2_CH4_PIN_A3,
      TIM2_ETR_PIN_A0,
      
      /* Timer 3*/
      TIM3_CH1_PIN_B4,
      TIM3_CH1_PIN_A6,
      TIM3_CH2_PIN_B5,
      TIM3_CH2_PIN_A7,
      TIM3_CH3_PIN_B0,
      TIM3_CH4_PIN_B1,
      
      /* Timer 4*/
      TIM4_CH1_PIN_B6,
      TIM4_CH2_PIN_B7,
      TIM4_CH3_PIN_B8,
      TIM4_CH4_PIN_B9,
    }Chanel_t;
    
    Timer(TIMx timerx);
    
    ~Timer(){};
    
    TimerStatus_t HwInit();
    
    void PinsHwInit(Chanel_t Chanel);
    
    void HwDeinit();
    
    void EnableInterrupts(uint16_t flags){SET_BIT(m_TIMx->DIER, flags); }
    
    void DisableInterrupts(uint16_t flags){CLEAR_BIT(m_TIMx->DIER, flags); }
    
    void EnableTimerCount(){LL_TIM_EnableCounter(m_TIMx);}
    
    void StopTicking(){LL_TIM_DisableCounter(m_TIMx);}
    
    void usDelay(uint16_t delay);
    
    template<uint16_t del>
      void usDelay_T()
      {
        LL_TIM_SetCounter(m_TIMx,del*8);
        
        LL_TIM_ClearFlag_UPDATE(m_TIMx);    
        
        EnableTimerCount();    
        while(LL_TIM_IsActiveFlag_UPDATE(m_TIMx) == 0); 
      }
    
    template<uint16_t del, TimerCb_t TimerCb>
      void usDelay_T()
      {
        LL_TIM_SetCounter(m_TIMx,(del-2)*8);  // 2 micros it takes to process the interrupts and call the callback
        
        LL_TIM_ClearFlag_UPDATE(m_TIMx);    
        
        m_TimerCb = TimerCb;
        
        EnableTimerCount();    
        
        EnableInterrupts(INTR_UIE);
      }
      
    void usDelay(uint16_t delay, TimerCb_t TimerCb);
    
    void Start(uint16_t delay);
    
    void Wait(){while(LL_TIM_IsActiveFlag_UPDATE(m_TIMx) == 0);}
    
    void ISR();
    
    void StartTicking(uint16_t TickFrequency);
      
    uint32_t GetTicks() const {return m_Ticks;}
    
    void Timer::PulseIn_HwInit(Chanel_t channel, uint8_t* Buf, uint16_t len);

  private:
    TIMx                m_TIMx;
    TimerStatus_t       m_TimerStatus;
    TimerCb_t           m_TimerCb;
    uint32_t            m_Ticks;
    
    TimerCb_t           m_TimerCBCC1;
    TimerCb_t           m_TimerCBCC2;
    uint32_t            ICValue1;
    uint32_t            ICValue2;
    
  };  
  
}

#endif