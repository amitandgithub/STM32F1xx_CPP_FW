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
#ifndef PulseOut_h
#define PulseOut_h

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
  
  template<uint32_t Timerx, TimerChanel_t Chanel,uint32_t Frequency = 10000>
    class PulseOut : public InterruptSource 
    {
    public:     
      
      PulseOut() : m_CCRx((uint32_t*)GetCCRx())
      {
        
      }
      
      ~PulseOut(){}; 
      
      Timer::TimerStatus_t HwInit();
      
      void PinsHwInit(TimerChanel_t Ch);
      
      void SetDutyCycle(uint8_t dutycycle);
      
      void SetFrequency(uint32_t Freq);
      
      void SinglePulseOut(uint8_t dutycycle, uint32_t Freq, uint32_t Count);
      
      void SinglePulseOutDisable();
      
      uint32_t GetCCRx();
      
      void Start(){LL_TIM_CC_EnableChannel((TIM_TypeDef*)Timerx, Timer::GetChanel<Chanel>());}
      
      void Stop(){LL_TIM_CC_DisableChannel((TIM_TypeDef*)Timerx, Timer::GetChanel<Chanel>());}
      
      virtual void ISR();
      
    private:
      uint32_t*   m_CCRx;
    };    
    
    template<uint32_t Timerx, TimerChanel_t Chanel,uint32_t Frequency>
      Timer::TimerStatus_t PulseOut<Timerx,Chanel,Frequency>::
    HwInit()
      {
        Timer::TimerStatus_t TimerStatus; 
        uint32_t ch = Timer::GetChanel<Chanel>();
        LL_TIM_OC_InitTypeDef TIM_OC_InitStruct;
          
        TIM_OC_InitStruct.OCMode       = LL_TIM_OCMODE_PWM1;
        TIM_OC_InitStruct.OCState      = LL_TIM_OCSTATE_ENABLE;
       // TIM_OC_InitStruct.OCNState     = LL_TIM_OCSTATE_ENABLE;
        TIM_OC_InitStruct.CompareValue = 0x00000000U;
        TIM_OC_InitStruct.OCPolarity   = LL_TIM_OCPOLARITY_HIGH;
       // TIM_OC_InitStruct.OCNPolarity  = LL_TIM_OCPOLARITY_HIGH;
       // TIM_OC_InitStruct.OCIdleState  = LL_TIM_OCIDLESTATE_LOW;
       // TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;      
        
        TimerStatus = Timer::ClockEnableAndRegisterInterrupt<Timerx>(nullptr);
        
        if( TimerStatus != Timer::TIMER_OK ) return TimerStatus;
        
        TimerStatus = Timer::HwInit<Timerx,0, __LL_TIM_CALC_PSC(72000000, Frequency),true>();
        
        if( TimerStatus != Timer::TIMER_OK ) return TimerStatus;
        
        if( LL_TIM_OC_Init((TIM_TypeDef*)Timerx, ch, &TIM_OC_InitStruct) == ERROR) return Timer::TIMER_INIT_ERROR;  
        
       // LL_TIM_CC_EnablePreload((TIM_TypeDef*)Timerx);
        
        LL_TIM_OC_EnablePreload((TIM_TypeDef*)Timerx,ch);
          
        SetDutyCycle(50);
          
        Timer::PinsHwInit<Chanel>(Timer::OUTPUT_PIN);
        
        Timer::EnableTimerCounter<Timerx>();

        //LL_TIM_CC_DisableChannel(ch);
        
        //LL_TIM_CC_EnableChannel((TIM_TypeDef*)Timerx, ch);
        
        return TimerStatus;
        
      }    
    
    template<uint32_t Timerx, TimerChanel_t Chanel,uint32_t Frequency>
      void PulseOut<Timerx,Chanel,Frequency>::
    ISR()
      {
        
      }
  
   template<uint32_t Timerx, TimerChanel_t Chanel,uint32_t Frequency>
      void PulseOut<Timerx,Chanel,Frequency>::
    SetDutyCycle(uint8_t dutycycle)
      {
        uint32_t Arr = LL_TIM_GetAutoReload((TIM_TypeDef*)Timerx);
        *m_CCRx = (Arr*dutycycle)/100;
      }
  
  template<uint32_t Timerx, TimerChanel_t Chanel,uint32_t Frequency>
      void PulseOut<Timerx,Chanel,Frequency>::
    SetFrequency(uint32_t Freq)
      {
        LL_TIM_SetAutoReload((TIM_TypeDef*)Timerx,Freq);
      }
                                              
    template<uint32_t Timerx, TimerChanel_t Chanel,uint32_t Frequency>
      uint32_t PulseOut<Timerx,Chanel,Frequency>::
    GetCCRx()
      {
        TIM_TypeDef* TIMx = (TIM_TypeDef*)Timerx;
        
        switch (Timer::GetChanel<Chanel>())
        {
        case LL_TIM_CHANNEL_CH1: return (uint32_t)&TIMx->CCR1;
        case LL_TIM_CHANNEL_CH2: return (uint32_t)&TIMx->CCR2;
        case LL_TIM_CHANNEL_CH3: return (uint32_t)&TIMx->CCR3;
        case LL_TIM_CHANNEL_CH4: return (uint32_t)&TIMx->CCR4;
        default: return (uint32_t)&TIMx->CCR1;
        }
      }
    template<uint32_t Timerx, TimerChanel_t Chanel,uint32_t Frequency>
      void PulseOut<Timerx,Chanel,Frequency>::
    SinglePulseOut(uint8_t dutycycle, uint32_t Freq, uint32_t Count)       
      {
        Timer::DisableTimerCounter<Timerx>();
        LL_TIM_SetOnePulseMode((TIM_TypeDef*)Timerx,LL_TIM_ONEPULSEMODE_SINGLE);       
        SetDutyCycle(dutycycle);
        SetFrequency(Freq);
       // LL_TIM_SetRepetitionCounter((TIM_TypeDef*)Timerx,Count);
        Timer::EnableTimerCounter<Timerx>(); 
      }
    
    template<uint32_t Timerx, TimerChanel_t Chanel,uint32_t Frequency>
      void PulseOut<Timerx,Chanel,Frequency>::
    SinglePulseOutDisable()       
      {
        Timer::DisableTimerCounter<Timerx>();
        LL_TIM_SetOnePulseMode((TIM_TypeDef*)Timerx,LL_TIM_ONEPULSEMODE_REPETITIVE);       
        Timer::EnableTimerCounter<Timerx>();
      }
                                              
  
}

#endif //