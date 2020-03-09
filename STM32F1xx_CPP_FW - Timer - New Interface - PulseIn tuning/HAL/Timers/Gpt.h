/******************
** CLASS: Gpt
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 15 Feb 2020, by Amit Chaudhary
**
** FILE: Gpt.h
**
******************/
#ifndef Gpt_h
#define Gpt_h

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

extern void HAL_TIM_PeriodElapsedCallback();

namespace HAL
{
  template<uint32_t Timerx, uint32_t Prescaler, uint32_t Autoreload, bool CountUp = true>
    class Gpt : public InterruptSource 
    {
    public:
      
      Gpt():m_Ticks(0){};
      
      ~Gpt(){};    
      
      Timer::TimerStatus_t HwInit();
      
      void ClockDisable();      
      
      void Delay(uint16_t delay);
      
      uint32_t GetTimerCounter() const {return LL_TIM_GetCounter( (TIM_TypeDef*)Timerx );}
      
      void DelayTicks(uint32_t Ticks);
      
      uint32_t GetTicks() const { return m_Ticks + GetTimerCounter(); }
      
      virtual void ISR();
      
    private:
      uint32_t  m_Ticks;
    };   
   
    
    template<uint32_t Timerx, uint32_t Prescaler, uint32_t Autoreload, bool CountUp>
      Timer::TimerStatus_t Gpt<Timerx, Prescaler, Autoreload,CountUp>::HwInit()
      {
        Timer::TimerStatus_t TimerStatus;        
        
        TimerStatus = Timer::ClockEnableAndRegisterInterrupt<Timerx>(this);
        
        if( TimerStatus != Timer::TIMER_OK ) return TimerStatus;
        
        TimerStatus = Timer::HwInit<Timerx,Prescaler, Autoreload,CountUp>();
          
        if( TimerStatus != Timer::TIMER_OK ) return TimerStatus;
        
        //LL_TIM_SetCounter( (TIM_TypeDef*)Timerx,0);
        
        //LL_TIM_SetOnePulseMode((TIM_TypeDef*)Timerx,LL_TIM_ONEPULSEMODE_SINGLE);
        
        LL_TIM_ClearFlag_UPDATE( (TIM_TypeDef*)Timerx);    
        
        Timer::EnableTimerCounter<Timerx>();
        
        Timer::EnableInterrupts<Timerx>(Timer::INTR_UIE);

        return Timer::TIMER_OK;
      }  

    template<uint32_t Timerx, uint32_t Prescaler, uint32_t Autoreload, bool CountUp>
      void Gpt<Timerx, Prescaler, Autoreload,CountUp>::ISR()
      {
        
        if( LL_TIM_IsActiveFlag_UPDATE( (TIM_TypeDef*)Timerx) )
        {
          HAL_TIM_PeriodElapsedCallback();
          LL_TIM_ClearFlag_UPDATE((TIM_TypeDef*)Timerx);
        }
        
      }
    
    template<uint32_t Timerx, uint32_t Prescaler, uint32_t Autoreload, bool CountUp>
    void Gpt<Timerx, Prescaler, Autoreload,CountUp>::DelayTicks(uint32_t  Ticks)
    {
      uint32_t start, end;
      start = GetTimerCounter();
      end = start + Ticks*8;
      if (start < end)
      {
        while ((GetTimerCounter() >= start) && (GetTimerCounter() < end))
        {
          // do nothing
        }
      }
      else
      {
        while ((GetTimerCounter() >= start) || (GetTimerCounter() < end))
        {
          // do nothing
        }
      }
    }
    

}


#endif //Gpt_h