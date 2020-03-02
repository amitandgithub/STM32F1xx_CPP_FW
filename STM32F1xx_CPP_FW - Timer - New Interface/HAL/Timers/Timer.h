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
  namespace Timer
  {
    typedef TIM_TypeDef* TIMx;
    typedef  Callback* TimerCb_t;
    
    typedef enum  : uint8_t
    {
      TIMER_OK,      
      TIMER_BUSY,
      TIMER_INVALID_PARAMS,
      TIMER_INIT_ERROR,
    }TimerStatus_t;
    
    enum
    {
      FALLING_EDGE = 0,
      RISING_EDGE = 1,
      NO_EDGE = 2,
    };
    
    enum
    {
      OUTPUT_PIN = 0,
      INPUT_PIN = 1,
    };
    
    enum 
    {
      uSecTimer = 8000000U,
      mSecTimer = 8000000U,
      SecTimer  = 8000000U,
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
    
    template<uint32_t Timerx, uint32_t Prescaler, uint32_t Autoreload = 0xFFFFU, bool CountUp = true>
      TimerStatus_t HwInit()
      {
        LL_TIM_InitTypeDef TIM_InitStruct;
        
        TIM_InitStruct.Prescaler         = Prescaler;
        TIM_InitStruct.CounterMode       = CountUp ? LL_TIM_COUNTERMODE_UP : LL_TIM_COUNTERMODE_DOWN;
        TIM_InitStruct.Autoreload        = Autoreload;
        TIM_InitStruct.ClockDivision     = LL_TIM_CLOCKDIVISION_DIV1;
        TIM_InitStruct.RepetitionCounter = (uint8_t)0x00;
       
        if(LL_TIM_Init(((TIM_TypeDef*)Timerx), &TIM_InitStruct) == ERROR)
        {
          return TIMER_INIT_ERROR;
        }        
        return TIMER_OK;
      }   
    
     template<uint32_t Timerx>
      void SetPrescaler(uint32_t Val)
      {
        LL_TIM_SetPrescaler((TIM_TypeDef*)Timerx, Val);
      }
    
    template<uint32_t Timerx>
      void SetAutoreload(uint32_t Val)
      {
        LL_TIM_SetAutoReload((TIM_TypeDef*)Timerx, Val);
      }
    
    template<uint32_t Timerx>
      void SetRepetitionCounter(uint8_t counter)
      {
        LL_TIM_SetRepetitionCounter((TIM_TypeDef*)Timerx, counter);
      }
    
    template<uint32_t Timerx>
      void SetARR(uint32_t AutoReload)
      {
        LL_TIM_SetAutoReload((TIM_TypeDef*)Timerx, AutoReload);
      }
    
    template<uint32_t Timerx>
      void EnableInterrupts(uint16_t flags)
      {
        SET_BIT( ((TIM_TypeDef*)Timerx)->DIER, flags); 
      }
    
    template<uint32_t Timerx>
      void DisableInterrupts(uint16_t flags)
      {
        CLEAR_BIT( ((TIM_TypeDef*)Timerx)->DIER, flags);
      }
    
    template<uint32_t Timerx>
      void EnableTimerCounter()
      {
        LL_TIM_EnableCounter(((TIM_TypeDef*)Timerx));
      }
    
    template<uint32_t Timerx>
      void DisableTimerCounter()
      {
        LL_TIM_DisableCounter(((TIM_TypeDef*)Timerx));
      }    
    
    template<uint32_t Timerx>
      TimerStatus_t ClockEnableAndRegisterInterrupt(InterruptSource* This)
      {
        if (((TIM_TypeDef*)Timerx) == TIM2)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM2_IRQn,0,This);  
        }
#if defined(TIM1)
        else if (((TIM_TypeDef*)Timerx) == TIM1)
        {
          LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
          // All Timer 1 interrupts share one ISR, can use 
          // different ISR for each IRQ if required
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM1_BRK_IRQn,0,This); 
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM1_UP_IRQn,0,This);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM1_TRG_COM_IRQn,0,This);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM1_CC_IRQn,0,This);
        }
#endif
#if defined(TIM3)
        else if (((TIM_TypeDef*)Timerx) == TIM3)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM3_IRQn,0,This);  
        }
#endif
#if defined(TIM4)
        else if (((TIM_TypeDef*)Timerx) == TIM4)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM4_IRQn,0,This);  
        }
#endif
#if defined(TIM5)
        else if (((TIM_TypeDef*)Timerx) == TIM5)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM5_IRQn,0,This);  
        }
#endif
#if defined(TIM6)
        else if (((TIM_TypeDef*)Timerx) == TIM6)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM6_IRQn,0,This);  
        }
#endif
#if defined (TIM7)
        else if (((TIM_TypeDef*)Timerx) == TIM7)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM7_IRQn,0,This);  
        }
#endif
#if defined(TIM8)
        else if (((TIM_TypeDef*)Timerx) == TIM8)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM8);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM8_IRQn,0,This);  
        }
#endif
#if defined(TIM9)
        else if (((TIM_TypeDef*)Timerx) == TIM9)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM9);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM9_IRQn,0,This);  
        }
#endif
#if defined(TIM10)
        else if (((TIM_TypeDef*)Timerx) == TIM10)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM10);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM10_IRQn,0,This);  
        }
#endif
#if defined(TIM11)
        else if (((TIM_TypeDef*)Timerx) == TIM11)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM11);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM11_IRQn,0,This);  
        }
#endif
#if defined(TIM12)
        else if (((TIM_TypeDef*)Timerx) == TIM12)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM12);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM12_IRQn,0,This);  
        }
#endif
#if defined(TIM13)
        else if (((TIM_TypeDef*)Timerx) == TIM13)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM13);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM13_IRQn,0,This);  
        }
#endif
#if defined(TIM14)
        else if (((TIM_TypeDef*)Timerx) == TIM14)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM14_IRQn,0,This);  
        }
#endif
#if defined(TIM15)
        else if (((TIM_TypeDef*)Timerx) == TIM15)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM15);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM15_IRQn,0,This);  
        }
#endif
#if defined(TIM16)
        else if (((TIM_TypeDef*)Timerx) == TIM16)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM16);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM16_IRQn,0,This);  
        }
#endif
#if defined(TIM17)
        else if (((TIM_TypeDef*)Timerx) == TIM17)
        {
          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM17);
          InterruptManagerInstance.RegisterDeviceInterrupt(TIM17_IRQn,0,This);  
        }
#endif
        else
        {
          return TIMER_INVALID_PARAMS;
        }  
        return TIMER_OK;
      }  
    
    
    template<TimerChanel_t Chanel>
      void PinsHwInit(bool Input)
      {
        Port_t port;
        PIN_t pin;
        
        switch(Chanel)
        {
          /* Timer 1*/
        case TIM1_ETR_PIN_A12: port = PORTA; pin = 12; break;
        case TIM1_CH1_PIN_A8: port = PORTA; pin = 8; break;
        case TIM1_CH2_PIN_A9: port = PORTA; pin = 9; break;
        case TIM1_CH3_PIN_A10: port = PORTA; pin = 10; break;
        case TIM1_CH4_PIN_A11: port = PORTA; pin = 11; break;  
        case TIM1_BKIN_PIN_B12: port = PORTB; pin = 12; break;  
        case TIM1_CH1N_PIN_B13: port = PORTB; pin = 13; break;
        case TIM1_CH2N_PIN_B14: port = PORTB; pin = 14; break;
        case TIM1_CH3N_PIN_B15: port = PORTB; pin = 15; break;
#ifdef PORTE        
        case TIM1_ETR_PIN_REMAP_E7: port = PORTE; pin = 7; break;
        case TIM1_CH1_PIN_REMAP_E9: port = PORTE; pin = 9; break;
        case TIM1_CH2_PIN_REMAP_E11: port = PORTE; pin = 11; break;
        case TIM1_CH3_PIN_REMAP_E13: port = PORTE; pin = 13; break;
        case TIM1_CH4_PIN_REMAP_E14: port = PORTE; pin = 14; break;  
        case TIM1_BKIN_PIN_REMAP_E15: port = PORTE; pin = 15; break;  
        case TIM1_CH1N_PIN_REMAP_E8: port = PORTE; pin = 8; break;
        case TIM1_CH2N_PIN_REMAP_E10: port = PORTE; pin = 10; break;
        case TIM1_CH3N_PIN_REMAP_E12: port = PORTE; pin = 12; break;
#endif // PORTE  
        case TIM1_ETR_PIN_PAR_REMAP_A12: port = PORTA; pin = 12; break;
        case TIM1_CH1_PIN_PAR_REMAP_A8: port = PORTA; pin = 8; break;
        case TIM1_CH2_PIN_PAR_REMAP_A9: port = PORTA; pin = 9; break;
        case TIM1_CH3_PIN_PAR_REMAP_A10: port = PORTA; pin = 10; break;
        case TIM1_CH4_PIN_PAR_REMAP_A11: port = PORTA; pin = 11; break;  
        case TIM1_BKIN_PIN_PAR_REMAP_A6: port = PORTA; pin = 6; break;  
        case TIM1_CH1N_PIN_PAR_REMAP_A7: port = PORTA; pin = 7; break;
        case TIM1_CH2N_PIN_PAR_REMAP_B0: port = PORTB; pin = 0; break;
        case TIM1_CH3N_PIN_PAR_REMAP_B1: port = PORTB; pin = 1; break;
        
        /* Timer 2*/
        case TIM2_ETR_PIN_A0: port = PORTA; pin = 0; break;
        case TIM2_CH1_PIN_A0: port = PORTA; pin = 0; break;
        case TIM2_CH2_PIN_A1: port = PORTA; pin = 1; break;
        case TIM2_CH3_PIN_A2: port = PORTA; pin = 2; break;
        case TIM2_CH4_PIN_A3: port = PORTA; pin = 3; break;  
        
        case TIM2_ETR_PIN_REMAP_A15: port = PORTA; pin = 15; break;
        case TIM2_CH1_PIN_REMAP_A15: port = PORTA; pin = 15; AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_FULLREMAP; break;
        case TIM2_CH2_PIN_REMAP_B3: port = PORTB; pin = 3; break;
        case TIM2_CH3_PIN_REMAP_B10: port = PORTB; pin = 10; break;
        case TIM2_CH4_PIN_REMAP_B11: port = PORTB; pin = 11; break;  
        
        case TIM2_ETR_PIN_PAR_REMAP_A15: port = PORTA; pin = 15; break;
        case TIM2_CH1_PIN_PAR_REMAP_A15: port = PORTA; pin = 15;break;
        case TIM2_CH2_PIN_PAR_REMAP_B3: port = PORTB; pin = 3; break;
        case TIM2_CH3_PIN_PAR_REMAP_A2: port = PORTA; pin = 2; break;
        case TIM2_CH4_PIN_PAR_REMAP_A3: port = PORTA; pin = 3; break;
        
        case TIM2_ETR_PIN_PAR_REMAP_A0: port = PORTA; pin = 0; break;
        case TIM2_CH1_PIN_PAR_REMAP_A0: port = PORTA; pin = 0; break;
        case TIM2_CH2_PIN_PAR_REMAP_B1: port = PORTB; pin = 1; break;
        case TIM2_CH3_PIN_PAR_REMAP_B10: port = PORTB; pin = 10; break;
        case TIM2_CH4_PIN_PAR_REMAP_B11: port = PORTB; pin = 11; break;        
        
        /* Timer 3*/ 
        case TIM3_CH1_PIN_A6: port = PORTA; pin = 6; break;
        case TIM3_CH2_PIN_A7: port = PORTA; pin = 7; break;
        case TIM3_CH3_PIN_B0: port = PORTB; pin = 0; break;
        case TIM3_CH4_PIN_B1: port = PORTB; pin = 1; break;
        
        case TIM3_CH1_PIN_PAR_REMAP_B4: port = PORTB; pin = 4; AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP; break;
        case TIM3_CH2_PIN_PAR_REMAP_B5: port = PORTB; pin = 5;  AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;break;
        case TIM3_CH3_PIN_PAR_REMAP_B0: port = PORTB; pin = 0; break;
        case TIM3_CH4_PIN_PAR_REMAP_B1: port = PORTB; pin = 1; break; 
        
        case TIM3_CH1_PIN_REMAP_C6: port = PORTC; pin = 6; break;
        case TIM3_CH2_PIN_REMAP_C7: port = PORTC; pin = 7; break;
        case TIM3_CH3_PIN_REMAP_C8: port = PORTC; pin = 8; break;
        case TIM3_CH4_PIN_REMAP_C9: port = PORTC; pin = 9; break;
        
        /* Timer 4*/
        case TIM4_CH1_PIN_B6: port = PORTB; pin = 6; break;
        case TIM4_CH2_PIN_B7: port = PORTB; pin = 7; break;
        case TIM4_CH3_PIN_B8: port = PORTB; pin = 8; break;
        case TIM4_CH4_PIN_B9: port = PORTB; pin = 9; break;
        
        case TIM4_CH1_PIN_REMAP_D12: port = PORTD; pin = 12; break;
        case TIM4_CH2_PIN_REMAP_D13: port = PORTD; pin = 13; break;
        case TIM4_CH3_PIN_REMAP_D14: port = PORTD; pin = 14; break;
        case TIM4_CH4_PIN_REMAP_D15: port = PORTD; pin = 15; break;
        default : break;
        }
        if(Input == true)
        {
          HAL::GpioInput TimerPin(port,pin);
          TimerPin.HwInit(INPUT_FLOATING);  // Input , No Pull
        }
        else
        {
          HAL::GpioOutput TimerPin(port,pin);
          TimerPin.HwInit(OUTPUT_AF_PUSH_PULL);  // Output Alternate function Push-Pull
        }
      }   
    
    template<TimerChanel_t Chanel>
      uint32_t GetChanel()
      {        
        switch(Chanel)
        {
          /* Timer 1*/
        case TIM1_CH1_PIN_A8:  return LL_TIM_CHANNEL_CH1;
        case TIM1_CH2_PIN_A9:  return LL_TIM_CHANNEL_CH2;
        case TIM1_CH3_PIN_A10: return LL_TIM_CHANNEL_CH3;
        case TIM1_CH4_PIN_A11: return LL_TIM_CHANNEL_CH4; 
        
#ifdef PORTE        
        case TIM1_CH1_PIN_REMAP_E9 : return LL_TIM_CHANNEL_CH1;
        case TIM1_CH2_PIN_REMAP_E11: return LL_TIM_CHANNEL_CH2;
        case TIM1_CH3_PIN_REMAP_E13: return LL_TIM_CHANNEL_CH3;
        case TIM1_CH4_PIN_REMAP_E14: return LL_TIM_CHANNEL_CH4;  
#endif // PORTE  
        case TIM1_CH1_PIN_PAR_REMAP_A8 : return LL_TIM_CHANNEL_CH1;
        case TIM1_CH2_PIN_PAR_REMAP_A9 : return LL_TIM_CHANNEL_CH2;
        case TIM1_CH3_PIN_PAR_REMAP_A10: return LL_TIM_CHANNEL_CH3;
        case TIM1_CH4_PIN_PAR_REMAP_A11: return LL_TIM_CHANNEL_CH4;  
        
        /* Timer 2*/
        case TIM2_CH1_PIN_A0: return LL_TIM_CHANNEL_CH1;
        case TIM2_CH2_PIN_A1: return LL_TIM_CHANNEL_CH2;
        case TIM2_CH3_PIN_A2: return LL_TIM_CHANNEL_CH3;
        case TIM2_CH4_PIN_A3: return LL_TIM_CHANNEL_CH4;  
        
        case TIM2_CH1_PIN_REMAP_A15: return LL_TIM_CHANNEL_CH1;
        case TIM2_CH2_PIN_REMAP_B3:  return LL_TIM_CHANNEL_CH2;
        case TIM2_CH3_PIN_REMAP_B10: return LL_TIM_CHANNEL_CH3;
        case TIM2_CH4_PIN_REMAP_B11: return LL_TIM_CHANNEL_CH4;
        
        case TIM2_CH1_PIN_PAR_REMAP_A15: return LL_TIM_CHANNEL_CH1;
        case TIM2_CH2_PIN_PAR_REMAP_B3:  return LL_TIM_CHANNEL_CH2;
        case TIM2_CH3_PIN_PAR_REMAP_A2:  return LL_TIM_CHANNEL_CH3;
        case TIM2_CH4_PIN_PAR_REMAP_A3:  return LL_TIM_CHANNEL_CH4;
        
        case TIM2_CH1_PIN_PAR_REMAP_A0:  return LL_TIM_CHANNEL_CH1;
        case TIM2_CH2_PIN_PAR_REMAP_B1:  return LL_TIM_CHANNEL_CH2;
        case TIM2_CH3_PIN_PAR_REMAP_B10: return LL_TIM_CHANNEL_CH3;
        case TIM2_CH4_PIN_PAR_REMAP_B11: return LL_TIM_CHANNEL_CH4;       
        
        /* Timer 3*/ 
        case TIM3_CH1_PIN_A6: return LL_TIM_CHANNEL_CH1;
        case TIM3_CH2_PIN_A7: return LL_TIM_CHANNEL_CH2;
        case TIM3_CH3_PIN_B0: return LL_TIM_CHANNEL_CH3;
        case TIM3_CH4_PIN_B1: return LL_TIM_CHANNEL_CH4;
        
        case TIM3_CH1_PIN_PAR_REMAP_B4: return LL_TIM_CHANNEL_CH1;
        case TIM3_CH2_PIN_PAR_REMAP_B5: return LL_TIM_CHANNEL_CH2;
        case TIM3_CH3_PIN_PAR_REMAP_B0: return LL_TIM_CHANNEL_CH3;
        case TIM3_CH4_PIN_PAR_REMAP_B1: return LL_TIM_CHANNEL_CH4;
        
        case TIM3_CH1_PIN_REMAP_C6: return LL_TIM_CHANNEL_CH1;
        case TIM3_CH2_PIN_REMAP_C7: return LL_TIM_CHANNEL_CH2;
        case TIM3_CH3_PIN_REMAP_C8: return LL_TIM_CHANNEL_CH3;
        case TIM3_CH4_PIN_REMAP_C9: return LL_TIM_CHANNEL_CH4;
        
        /* Timer 4*/
        case TIM4_CH1_PIN_B6: return LL_TIM_CHANNEL_CH1;
        case TIM4_CH2_PIN_B7: return LL_TIM_CHANNEL_CH2;
        case TIM4_CH3_PIN_B8: return LL_TIM_CHANNEL_CH3;
        case TIM4_CH4_PIN_B9: return LL_TIM_CHANNEL_CH4;
        
        case TIM4_CH1_PIN_REMAP_D12: return LL_TIM_CHANNEL_CH1;
        case TIM4_CH2_PIN_REMAP_D13: return LL_TIM_CHANNEL_CH2;
        case TIM4_CH3_PIN_REMAP_D14: return LL_TIM_CHANNEL_CH3;
        case TIM4_CH4_PIN_REMAP_D15: return LL_TIM_CHANNEL_CH4;
        default : return 0;
        }
      }
  }
  
}

#endif