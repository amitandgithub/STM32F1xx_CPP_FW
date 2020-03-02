/******************
** CLASS: PulseOut
**
** DESCRIPTION:
**  Pulse Out functionality
**
** CREATED: 18 Feb 2020, by Amit Chaudhary
**
** FILE: PulseOut.h
**
******************/
#ifndef PulseIn_h
#define PulseIn_h

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
#include"Timer.h"

namespace HAL
{
  
  template<uint32_t Timerx, TimerChanel_t Chanel, uint32_t Tick, Callback* CB = nullptr >
    class PulseOut : public InterruptSource 
    {
    public:     
      
      PulseOut()
      {
        
      }
      
      ~PulseOut(){}; 
      
      Timer::TimerStatus_t HwInit(LL_TIM_InitTypeDef* pTIM_InitStruct = nullptr);
      
      void PinsHwInit(TimerChanel_t Ch);
      
      virtual void ISR();
    private:
      uint32_t                  m_Ticks;
      uint32_t                  ICValue1;
      uint32_t                  ICValue2;
      uint32_t                  Width;
    };
    
    
    template<uint32_t Timerx, TimerChanel_t Chanel, uint32_t Tick, Callback* CB>
      Timer::TimerStatus_t PulseOut<Timerx,Chanel, Tick, CB>::HwInit(LL_TIM_InitTypeDef* pTIM_InitStruct)
      {
        Timer::TimerStatus_t TimerStatus;
        LL_TIM_InitTypeDef TIM_InitStruct;
        LL_TIM_IC_InitTypeDef TIM_ICInitStruct;
        
        TIM_InitStruct.Prescaler         = __LL_TIM_CALC_PSC (72000000, Tick);
        TIM_InitStruct.CounterMode       = LL_TIM_COUNTERMODE_UP;
        TIM_InitStruct.Autoreload        = 0xFFFFFFFFU;
        TIM_InitStruct.ClockDivision     = LL_TIM_CLOCKDIVISION_DIV1;
        TIM_InitStruct.RepetitionCounter = (uint8_t)0x00;
        
        TimerStatus = Timer::ClockEnableAndRegisterInterrupt<Timerx>(this);
        
        if( TimerStatus != Timer::TIMER_OK ) return TimerStatus;
         
        if(pTIM_InitStruct == nullptr)
        {
          TimerStatus = Timer::HwInit<Timerx,Tick>(&TIM_InitStruct);
        }
        else
        {
          TimerStatus = Timer::HwInit<Tick,Timerx>(pTIM_InitStruct);
        }
        
        if( TimerStatus != Timer::TIMER_OK ) return TimerStatus;
        
        Timer::DisableTimerCounter<Timerx>();
        
        Timer::PinsHwInit<Chanel>();
        
        TIM_ICInitStruct.ICPolarity    = LL_TIM_IC_POLARITY_RISING;
        TIM_ICInitStruct.ICActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI; 
        TIM_ICInitStruct.ICPrescaler   = LL_TIM_ICPSC_DIV1;
        TIM_ICInitStruct.ICFilter      = LL_TIM_IC_FILTER_FDIV1;
        
        LL_TIM_IC_Init((TIM_TypeDef*)Timerx,LL_TIM_CHANNEL_CH1,&TIM_ICInitStruct);
        
        TIM_ICInitStruct.ICActiveInput = LL_TIM_ACTIVEINPUT_INDIRECTTI;
        TIM_ICInitStruct.ICPolarity    = LL_TIM_IC_POLARITY_FALLING;
        
        LL_TIM_IC_Init((TIM_TypeDef*)Timerx,LL_TIM_CHANNEL_CH2,&TIM_ICInitStruct);
        
        LL_TIM_SetTriggerInput((TIM_TypeDef*)Timerx,LL_TIM_TS_TI1FP1); //LL_TIM_TS_TI2FP2 LL_TIM_TS_TI1FP1
        
        LL_TIM_SetSlaveMode((TIM_TypeDef*)Timerx,LL_TIM_SLAVEMODE_RESET); 
        
        LL_TIM_EnableMasterSlaveMode((TIM_TypeDef*)Timerx);
        
        Timer::DisableInterrupts<Timerx>(Timer::INTR_ALL);
        
        Timer::EnableInterrupts<Timerx>( Timer::INTR_UIE | Timer::INTR_CC1IE | Timer::INTR_CC2IE);  
        
        Timer::EnableTimerCounter<Timerx>();
        
        return TimerStatus;
        
      }    
    
    template<uint32_t Timerx, TimerChanel_t Chanel, uint32_t Tick, Callback* CB>
      void PulseOut<Timerx,Chanel, Tick, CB>::ISR()
      {
        if(LL_TIM_IsActiveFlag_UPDATE((TIM_TypeDef*)Timerx))
        {
          m_Ticks++;
          LL_TIM_ClearFlag_UPDATE((TIM_TypeDef*)Timerx);
        }    
        if(LL_TIM_IsActiveFlag_CC1((TIM_TypeDef*)Timerx))
        {
          LL_TIM_ClearFlag_CC1((TIM_TypeDef*)Timerx);
          ICValue1 = m_Ticks;  
        }    
        if(LL_TIM_IsActiveFlag_CC2((TIM_TypeDef*)Timerx))
        {
          LL_TIM_ClearFlag_CC2((TIM_TypeDef*)Timerx);
          ICValue2 = m_Ticks;
          
          Width = (ICValue2 - ICValue1)<<3;
        }
        
      }
}

#endif //