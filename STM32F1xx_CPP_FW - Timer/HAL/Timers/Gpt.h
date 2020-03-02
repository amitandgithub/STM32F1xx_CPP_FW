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


namespace HAL
{
  GpioOutput TglPin1(A3);
  template<uint32_t Timerx, uint32_t TickFreq, Callback* CB = nullptr >
    class Gpt : public InterruptSource 
    {
    public:
      
      Gpt():m_Ticks(0){};
      
      ~Gpt(){};       
      
     
      
      Timer::TimerStatus_t HwInit(LL_TIM_InitTypeDef* pTIM_InitStruct = nullptr);
      
      void ClockDisable();      
      
      void Delay(uint16_t delay);
      
      uint32_t GetTickCount() const {return LL_TIM_GetCounter( (TIM_TypeDef*)Timerx );}
      
      void DelayTicks(uint32_t Ticks);
      
      uint32_t GetTicks() const { return m_Ticks + GetTickCount(); }
      
      virtual void ISR();
      
    private:
      uint32_t            m_Ticks;
    };   
   
    
    template<uint32_t Timerx, uint32_t TickFreq, Callback* CB>
      Timer::TimerStatus_t Gpt<Timerx, TickFreq, CB>::HwInit(LL_TIM_InitTypeDef* pTIM_InitStruct)
      {
        Timer::TimerStatus_t TimerStatus;        
        
        TimerStatus = Timer::ClockEnableAndRegisterInterrupt<Timerx>(this);
        
        if( TimerStatus != Timer::TIMER_OK ) return TimerStatus;
        
        TimerStatus = Timer::HwInit<Timerx,TickFreq>(pTIM_InitStruct);
          
        if( TimerStatus != Timer::TIMER_OK ) return TimerStatus;
        
        //LL_TIM_SetCounter( (TIM_TypeDef*)Timerx,0);
        
        //LL_TIM_SetOnePulseMode((TIM_TypeDef*)Timerx,LL_TIM_ONEPULSEMODE_SINGLE);
        
        LL_TIM_ClearFlag_UPDATE( (TIM_TypeDef*)Timerx);    
        
        Timer::EnableTimerCounter<Timerx>();
        
        Timer::EnableInterrupts<Timerx>(Timer::INTR_UIE);
        
        TglPin1.HwInit();

        return Timer::TIMER_OK;
      }  

    template<uint32_t Timerx, uint32_t TickFreq, Callback* CB>
      void Gpt<Timerx, TickFreq, CB>::Delay(uint16_t delay)
      {
#if 0
        LL_TIM_SetCounter( (TIM_TypeDef*)Timerx,delay<<3);
        
        LL_TIM_ClearFlag_UPDATE( (TIM_TypeDef*)Timerx);    
        
        EnableTimerCounter();    
        
        while(LL_TIM_IsActiveFlag_UPDATE( (TIM_TypeDef*)Timerx) == 0);  
      
#elif 0
    //LL_TIM_ClearFlag_UPDATE( (TIM_TypeDef*)Timerx);
    int16_t InitialTicks = (int16_t)GetTickCount();
    int16_t Dly = (InitialTicks + delay*16);  
    int16_t Current = (int16_t)GetTickCount();
    //while( (int16_t)GetTickCount() < Dly);
     //while( (int16_t)GetTickCount() < InitialTicks + delay*8);
    
    if(Dly > 0)
    {
      while( (int16_t)GetTickCount() < Dly);
    }
    else
    {
      while( (int16_t)(Current + Dly) < (int16_t)0 )
      {
        Current = (int16_t)GetTickCount();
      }
    }
#else
    uint16_t InitialTicks = GetTickCount();
    uint32_t FinalTicks = (InitialTicks + delay*1);
    uint16_t y;
    
    if( FinalTicks <= 0xFFFFU )
    {
      while( GetTickCount() < FinalTicks);
    }
    else
    {
      
      y = (uint16_t)FinalTicks;
      
      while(GetTickCount() < 0xFFFFU);
      while(GetTickCount() < y);
    }
#endif
      }
    
   
    
    template<uint32_t Timerx, uint32_t TickFreq, Callback* CB>
      void Gpt<Timerx, TickFreq, CB>::ISR()
      {
        
        if( LL_TIM_IsActiveFlag_UPDATE( (TIM_TypeDef*)Timerx) )
        {
          LL_TIM_ClearFlag_UPDATE((TIM_TypeDef*)Timerx);
          m_Ticks += 0xFFFF>>3;
          TglPin1.Toggle();
        }
        
      }
    
    template<uint32_t Timerx, uint32_t TickFreq, Callback* CB>
    void Gpt<Timerx, TickFreq, CB>::DelayTicks(uint32_t  Ticks)
    {
      uint32_t start, end;
      start = GetTickCount();
      end = start + Ticks*8;
      if (start < end)
      {
        while ((GetTickCount() >= start) && (GetTickCount() < end))
        {
          // do nothing
        }
      }
      else
      {
        while ((GetTickCount() >= start) || (GetTickCount() < end))
        {
          // do nothing
        };
      }
    }
    

}


#endif //Gpt_h