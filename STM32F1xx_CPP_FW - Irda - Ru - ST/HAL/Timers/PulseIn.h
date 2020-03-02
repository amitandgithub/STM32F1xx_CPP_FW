/******************
** CLASS: PulseIn
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 15 Feb 2020, by Amit Chaudhary
**
** FILE: PulseIn.h
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

#include"Gpt.h"

#define m_TIMx ((TIM_TypeDef*)Timerx)
namespace HAL
{
  
  template<uint32_t Timerx, TimerChanel_t Chanel, uint32_t Tick, Callback* CB = nullptr >
    class PulseIn : public InterruptSource 
    {
    public:     
      
      PulseIn()
      {
        
      }
      
      ~PulseIn(){}; 
      
      void HwInit();
      
      void PinsHwInit(TimerChanel_t Ch);
      
      virtual void ISR();
    private:
      Gpt<Timerx,Tick,nullptr>  m_GPT;  
      uint32_t                  m_Ticks;
      uint32_t                  ICValue1;
      uint32_t                  ICValue2;
      uint32_t                  Width;
    };
    
    
    template<uint32_t Timerx, TimerChanel_t Chanel, uint32_t Tick, Callback* CB>
      void PulseIn<Timerx,Chanel, Tick, CB>::HwInit( )
      {
        LL_TIM_InitTypeDef TIM_InitStruct;
        LL_TIM_IC_InitTypeDef TIM_ICInitStruct;
        
        TIM_InitStruct.Prescaler         = __LL_TIM_CALC_PSC (72000000, Tick);
        TIM_InitStruct.CounterMode       = LL_TIM_COUNTERMODE_UP;
        TIM_InitStruct.Autoreload        = 0xFFFFFFFFU;
        TIM_InitStruct.ClockDivision     = LL_TIM_CLOCKDIVISION_DIV1;
        TIM_InitStruct.RepetitionCounter = (uint8_t)0x00;
        
        m_GPT.HwInit(Tick,&TIM_InitStruct);
        
        m_GPT.DisableTimerCounter();
        
        PinsHwInit(Chanel);
        
        InterruptManagerInstance.RegisterDeviceInterrupt(TIM2_IRQn,0,this);
        
        TIM_ICInitStruct.ICPolarity    = LL_TIM_IC_POLARITY_RISING;
        TIM_ICInitStruct.ICActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI; 
        TIM_ICInitStruct.ICPrescaler   = LL_TIM_ICPSC_DIV1;
        TIM_ICInitStruct.ICFilter      = LL_TIM_IC_FILTER_FDIV1;
        
        LL_TIM_IC_Init(m_TIMx,LL_TIM_CHANNEL_CH1,&TIM_ICInitStruct);
        
        TIM_ICInitStruct.ICActiveInput = LL_TIM_ACTIVEINPUT_INDIRECTTI;
        TIM_ICInitStruct.ICPolarity    = LL_TIM_IC_POLARITY_FALLING;
        
        LL_TIM_IC_Init(m_TIMx,LL_TIM_CHANNEL_CH2,&TIM_ICInitStruct);
        
        LL_TIM_SetTriggerInput(m_TIMx,LL_TIM_TS_TI1FP1); //LL_TIM_TS_TI2FP2 LL_TIM_TS_TI1FP1
        
        LL_TIM_SetSlaveMode(m_TIMx,LL_TIM_SLAVEMODE_RESET); 
        
        LL_TIM_EnableMasterSlaveMode(m_TIMx);
        
        m_GPT.DisableInterrupts(Gpt<Timerx,Tick,nullptr>::INTR_ALL);
        
        m_GPT.EnableInterrupts( Gpt<Timerx,Tick,nullptr>::INTR_UIE | Gpt<Timerx,Tick,nullptr>::INTR_CC1IE | Gpt<Timerx,Tick,nullptr>::INTR_CC2IE);  
        
        m_GPT.EnableTimerCounter();
        
      }
    
    template<uint32_t Timerx, TimerChanel_t Chanel, uint32_t Tick, Callback* CB>
      void PulseIn<Timerx,Chanel, Tick, CB>::PinsHwInit(TimerChanel_t Ch)
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
        
        {
          GpioInput TimerPin(port,pin);
          TimerPin.HwInit(INPUT_FLOATING);  // Input , No Pull
        }
      }    
    
    template<uint32_t Timerx, TimerChanel_t Chanel, uint32_t Tick, Callback* CB>
      void PulseIn<Timerx,Chanel, Tick, CB>::ISR()
      {
        if(LL_TIM_IsActiveFlag_UPDATE(m_TIMx))
        {
          m_Ticks++;
          LL_TIM_ClearFlag_UPDATE(m_TIMx);
        }    
        if(LL_TIM_IsActiveFlag_CC1(m_TIMx))
        {
          LL_TIM_ClearFlag_CC1(m_TIMx);
          ICValue1 = m_Ticks;  
        }    
        if(LL_TIM_IsActiveFlag_CC2(m_TIMx))
        {
          LL_TIM_ClearFlag_CC2(m_TIMx);
          ICValue2 = m_Ticks;
          
          Width = (ICValue2 - ICValue1)<<3;
        }
        
      }
}

#endif //