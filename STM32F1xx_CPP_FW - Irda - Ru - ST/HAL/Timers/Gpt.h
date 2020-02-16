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


namespace HAL
{
  GpioOutput TglPin1(A3);
  template<uint32_t Timerx, uint32_t Tick, Callback* CB = nullptr >
    class Gpt : public InterruptSource 
    {
    public:
      
      Gpt():m_Ticks(0){};
      
      ~Gpt(){};      
      
      
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
        INTR_UIE          = TIM_DIER_UIE,
        INTR_CC1IE        = TIM_DIER_CC1IE,
        INTR_CC2IE        = TIM_DIER_CC2IE,
        INTR_CC3IE        = TIM_DIER_CC3IE,
        INTR_CC4IE        = TIM_DIER_CC4IE,
        INTR_ALL          = TIM_DIER_UIE | TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE,
      };
      
      enum 
      {
        uSecTimer = 8000000U,
        mSecTimer = 8000000U,
        SecTimer  = 8000000U,
      };
      
      TimerStatus_t HwInit(uint32_t TickFreq = uSecTimer, LL_TIM_InitTypeDef* pTIM_InitStruct = nullptr);
      
      TimerStatus_t ClockEnableAndRegisterInterrupt();
      
      void ClockDisable();
      
      void EnableInterrupts(uint16_t flags){SET_BIT( ((TIM_TypeDef*)Timerx)->DIER, flags); }
      
      void DisableInterrupts(uint16_t flags){CLEAR_BIT( ((TIM_TypeDef*)Timerx)->DIER, flags); }
      
      void EnableTimerCounter(){LL_TIM_EnableCounter(((TIM_TypeDef*)Timerx));}
      
      void DisableTimerCounter(){LL_TIM_DisableCounter(((TIM_TypeDef*)Timerx));}
      
      void Delay(uint16_t delay);
      
      uint32_t GetTickCount() const {return LL_TIM_GetCounter( (TIM_TypeDef*)Timerx );}
      
      void DelayTicks(uint32_t Ticks);
      
      uint32_t GetTicks() const { return m_Ticks + GetTickCount(); }
      
      virtual void ISR();
      
    private:
      uint32_t            m_Ticks;
    };
    
    
    template<uint32_t Timerx, uint32_t Tick, Callback* CB>
      Gpt<Timerx, Tick, CB>::TimerStatus_t Gpt<Timerx, Tick, CB>::ClockEnableAndRegisterInterrupt()
      {
        if (((TIM_TypeDef*)Timerx) == TIM2)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM2_IRQn,0,this);  
        }
#if defined(TIM1)
        else if (((TIM_TypeDef*)Timerx) == TIM1)
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
        else if (((TIM_TypeDef*)Timerx) == TIM3)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM3_IRQn,0,this);  
        }
#endif
#if defined(TIM4)
        else if (((TIM_TypeDef*)Timerx) == TIM4)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM4_IRQn,0,this);  
        }
#endif
#if defined(TIM5)
        else if (((TIM_TypeDef*)Timerx) == TIM5)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM5_IRQn,0,this);  
        }
#endif
#if defined(TIM6)
        else if (((TIM_TypeDef*)Timerx) == TIM6)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM6_IRQn,0,this);  
        }
#endif
#if defined (TIM7)
        else if (((TIM_TypeDef*)Timerx) == TIM7)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM7_IRQn,0,this);  
        }
#endif
#if defined(TIM8)
        else if (((TIM_TypeDef*)Timerx) == TIM8)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM8);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM8_IRQn,0,this);  
        }
#endif
#if defined(TIM9)
        else if (((TIM_TypeDef*)Timerx) == TIM9)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM9);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM9_IRQn,0,this);  
        }
#endif
#if defined(TIM10)
        else if (((TIM_TypeDef*)Timerx) == TIM10)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM10);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM10_IRQn,0,this);  
        }
#endif
#if defined(TIM11)
        else if (((TIM_TypeDef*)Timerx) == TIM11)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM11);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM11_IRQn,0,this);  
        }
#endif
#if defined(TIM12)
        else if (((TIM_TypeDef*)Timerx) == TIM12)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM12);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM12_IRQn,0,this);  
        }
#endif
#if defined(TIM13)
        else if (((TIM_TypeDef*)Timerx) == TIM13)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM13);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM13_IRQn,0,this);  
        }
#endif
#if defined(TIM14)
        else if (((TIM_TypeDef*)Timerx) == TIM14)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM14_IRQn,0,this);  
        }
#endif
#if defined(TIM15)
        else if (((TIM_TypeDef*)Timerx) == TIM15)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM15);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM15_IRQn,0,this);  
        }
#endif
#if defined(TIM16)
        else if (((TIM_TypeDef*)Timerx) == TIM16)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM16);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM16_IRQn,0,this);  
        }
#endif
#if defined(TIM17)
        else if (((TIM_TypeDef*)Timerx) == TIM17)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM17);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM17_IRQn,0,this);  
        }
#endif
        else
        {
          //m_TimerStatus = TIMER_INVALID_PARAMS;
          return TIMER_INVALID_PARAMS;
        }  
        return TIMER_OK;
      }  
    
    template<uint32_t Timerx, uint32_t Tick, Callback* CB>
      Gpt<Timerx, Tick, CB>::TimerStatus_t Gpt<Timerx, Tick, CB>::HwInit(uint32_t TickFreq, LL_TIM_InitTypeDef* pTIM_InitStruct)
      {
        TimerStatus_t TimerStatus;
        LL_TIM_InitTypeDef TIM_InitStruct;
        
        TimerStatus = ClockEnableAndRegisterInterrupt();
        
        if( TimerStatus != TIMER_OK ) return TimerStatus;
        
        if(pTIM_InitStruct == nullptr)
        {      
          TIM_InitStruct.Prescaler         = __LL_TIM_CALC_PSC (72000000, 8000000);
          TIM_InitStruct.CounterMode       = LL_TIM_COUNTERMODE_UP;
          TIM_InitStruct.Autoreload        = 0xFFFFFFFFU;
          TIM_InitStruct.ClockDivision     = LL_TIM_CLOCKDIVISION_DIV1;
          TIM_InitStruct.RepetitionCounter = (uint8_t)0x00;
          LL_TIM_Init(((TIM_TypeDef*)Timerx), &TIM_InitStruct);
        }
        else
        {
          LL_TIM_Init(((TIM_TypeDef*)Timerx), pTIM_InitStruct);
        }  
        
        //LL_TIM_SetCounter( (TIM_TypeDef*)Timerx,0);
        
        //LL_TIM_SetOnePulseMode((TIM_TypeDef*)Timerx,LL_TIM_ONEPULSEMODE_SINGLE);
        
        LL_TIM_ClearFlag_UPDATE( (TIM_TypeDef*)Timerx);    
        
        EnableTimerCounter();
        
        EnableInterrupts(INTR_UIE);
        
        TglPin1.HwInit();

        return TIMER_OK;
      }  

    template<uint32_t Timerx, uint32_t Tick, Callback* CB>
      void Gpt<Timerx, Tick, CB>::Delay(uint16_t delay)
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
    
   
    
    template<uint32_t Timerx, uint32_t Tick, Callback* CB>
      void Gpt<Timerx, Tick, CB>::ISR()
      {
        
        if( LL_TIM_IsActiveFlag_UPDATE( (TIM_TypeDef*)Timerx) )
        {
          LL_TIM_ClearFlag_UPDATE((TIM_TypeDef*)Timerx);
          m_Ticks += 0xFFFF>>3;
          TglPin1.Toggle();
        }
        
      }
    
    template<uint32_t Timerx, uint32_t Tick, Callback* CB>
    void Gpt<Timerx, Tick, CB>::DelayTicks(uint32_t  Ticks)
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