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
#include"Timer.h"

#include "rc5_decode.h"
extern void HAL_TIM_PeriodElapsedCallback();
extern HAL::DigitalOut<B5> TglPin;
namespace HAL
{
  
  template< uint32_t Timerx, TimerChanel_t Chanel, uint32_t FirstEdge, uint32_t SecondEdge >
    class PulseIn : public InterruptSource 
    {
    public:     
      
      //      template < typename BaseT >
      //      struct PulseData
      //      {
      //        BaseT Start:1;
      //        BaseT Timestamp:((sizeof(BaseT)*8)-2);
      //        BaseT Stop:1;        
      //      };   
      
      
        
      template < typename BaseT >
        struct PulseData
        {
          BaseT Start:1;
          BaseT Timestamp:((sizeof(BaseT)*8)-1);      
        };
      
      typedef union 
      {
        struct Member
        {
          uint8_t  Start;
          uint16_t Timestamp;
          uint8_t  Stop;       
        };
        uint32_t word;        
      }PulseSample_t;
      
      typedef PulseData<uint16_t> PulseData_t;
      
      //using SampleQueue_t = Utils::queue<PulseSample_t,uint32_t,Samples> ;
      
      PulseIn()
      {
        ;//static_assert( (pCallback != 0), "Error: pCallback can't be NULL");
      }
      
      ~PulseIn(){}; 
      
      Timer::TimerStatus_t HwInit(uint32_t Prescaler, uint32_t Autoreload = 0xFFFFU, bool CountUp = true);
      
      void PinsHwInit(TimerChanel_t Ch);
      
      uint32_t GetTimerCounter() const {return LL_TIM_GetCounter( (TIM_TypeDef*)Timerx );}
      
      bool Start(PulseBuf_t* PulseBuf);
      
      void Stop();

      virtual void ISR(); 
    private:
      PulseBuf_t* m_PulseBuf;
      uint16_t MAX_COUNT;
      uint16_t ICValue1;
      uint16_t ICValue2;
    };
    
    
    template< uint32_t Timerx, TimerChanel_t Chanel, uint32_t FirstEdge, uint32_t SecondEdge >
      Timer::TimerStatus_t PulseIn<Timerx,Chanel,FirstEdge,SecondEdge>::
      HwInit(uint32_t Prescaler, uint32_t Autoreload, bool CountUp)
      {
        LL_TIM_InitTypeDef TIM_InitStruct;
        LL_TIM_IC_InitTypeDef TIM_ICInitStruct;
        
        static_assert( (FirstEdge != SecondEdge), "Error: Both edges should not be same");
        
        MAX_COUNT = Autoreload;
        
        TIM_InitStruct.Prescaler         = Prescaler;
        TIM_InitStruct.CounterMode       = CountUp == true ? LL_TIM_COUNTERMODE_UP : LL_TIM_COUNTERMODE_DOWN;
        TIM_InitStruct.Autoreload        = Autoreload;
        TIM_InitStruct.ClockDivision     = LL_TIM_CLOCKDIVISION_DIV1;
        //TIM_InitStruct.RepetitionCounter = (uint8_t)0x00;
        
        if( ( Timer::ClockEnableAndRegisterInterrupt<Timerx>(this) != Timer::TIMER_OK ) || (LL_TIM_Init(((TIM_TypeDef*)Timerx), &TIM_InitStruct) == ERROR) )
        {
          return Timer::TIMER_INIT_ERROR;
        }      
        
        Timer::DisableTimerCounter<Timerx>();
        
        Timer::PinsHwInit<Chanel>(Timer::INPUT_PIN);
        
        TIM_ICInitStruct.ICPolarity    = FirstEdge == Timer::RISING_EDGE ? LL_TIM_IC_POLARITY_RISING : LL_TIM_IC_POLARITY_FALLING ;
        TIM_ICInitStruct.ICActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI; 
        TIM_ICInitStruct.ICPrescaler   = LL_TIM_ICPSC_DIV1;
        TIM_ICInitStruct.ICFilter      = LL_TIM_IC_FILTER_FDIV1;
        
        LL_TIM_IC_Init((TIM_TypeDef*)Timerx,LL_TIM_CHANNEL_CH1,&TIM_ICInitStruct);
        
        TIM_ICInitStruct.ICActiveInput = LL_TIM_ACTIVEINPUT_INDIRECTTI;
        TIM_ICInitStruct.ICPolarity    = SecondEdge == Timer::RISING_EDGE ? LL_TIM_IC_POLARITY_RISING : LL_TIM_IC_POLARITY_FALLING;
        
        LL_TIM_IC_Init((TIM_TypeDef*)Timerx,LL_TIM_CHANNEL_CH2,&TIM_ICInitStruct);
        
        LL_TIM_SetTriggerInput((TIM_TypeDef*)Timerx,LL_TIM_TS_TI2FP2); //LL_TIM_TS_TI2FP2 LL_TIM_TS_TI1FP1
        
        LL_TIM_SetSlaveMode((TIM_TypeDef*)Timerx,LL_TIM_SLAVEMODE_RESET); 
        
        LL_TIM_EnableMasterSlaveMode((TIM_TypeDef*)Timerx);
        
        Timer::DisableInterrupts<Timerx>(Timer::INTR_ALL);
        
        Timer::EnableInterrupts<Timerx>( Timer::INTR_CC1IE | Timer::INTR_CC2IE);  //Timer::INTR_UIE |
        
        LL_TIM_EnableARRPreload((TIM_TypeDef*)Timerx);
        
        LL_TIM_SetUpdateSource((TIM_TypeDef*)Timerx,LL_TIM_UPDATESOURCE_COUNTER);  
        
        
        return Timer::TIMER_OK;        
      }    
    
     template< uint32_t Timerx, TimerChanel_t Chanel, uint32_t FirstEdge, uint32_t SecondEdge >
      bool PulseIn<Timerx,Chanel,FirstEdge,SecondEdge>::
      Start(PulseBuf_t* PulseBuf)
      {
        if( (PulseBuf->Buf == nullptr) && (PulseBuf->Len == 0) ) return false;
        
        m_PulseBuf = PulseBuf;    
        
        Timer::EnableInterrupts<Timerx>( Timer::INTR_CC1IE | Timer::INTR_CC2IE);
        Timer::EnableTimerCounter<Timerx>();
        return true;
      }
    
    template< uint32_t Timerx, TimerChanel_t Chanel, uint32_t FirstEdge, uint32_t SecondEdge >
      void PulseIn<Timerx,Chanel,FirstEdge,SecondEdge>::
      Stop()
      {
        Timer::DisableInterrupts<Timerx>( Timer::INTR_CC1IE | Timer::INTR_CC2IE);
        Timer::DisableTimerCounter<Timerx>();
      }
    
    template< uint32_t Timerx, TimerChanel_t Chanel, uint32_t FirstEdge, uint32_t SecondEdge >
      void PulseIn<Timerx,Chanel,FirstEdge,SecondEdge>::
      ISR()
      {            
        if(LL_TIM_IsActiveFlag_CC1((TIM_TypeDef*)Timerx))
        {      
          ICValue1 = LL_TIM_OC_GetCompareCH1((TIM_TypeDef*)Timerx);           
          m_PulseBuf->Buf[m_PulseBuf->Idx++] = (ICValue1 > ICValue2) ? ICValue1 - ICValue2 : MAX_COUNT - ICValue2 + ICValue1;
          LL_TIM_ClearFlag_CC1((TIM_TypeDef*)Timerx);
          
          if(m_PulseBuf->Idx >= (m_PulseBuf->Len) )
          {
            Stop();
            if(m_PulseBuf->pCallback) m_PulseBuf->pCallback->CallbackFunction();
          }          
        }    
        else if(LL_TIM_IsActiveFlag_CC2((TIM_TypeDef*)Timerx))
        {         
          ICValue2 = LL_TIM_OC_GetCompareCH2((TIM_TypeDef*)Timerx);
          m_PulseBuf->Buf[m_PulseBuf->Idx++] = (ICValue2 > ICValue1) ? ICValue2 - ICValue1 : MAX_COUNT - ICValue1 + ICValue2;
          LL_TIM_ClearFlag_CC2((TIM_TypeDef*)Timerx);
          
          if(m_PulseBuf->Idx >= (m_PulseBuf->Len))
          {
            Stop();
            if(m_PulseBuf->pCallback) m_PulseBuf->pCallback->CallbackFunction();
          }          
        }        
        else if(LL_TIM_IsActiveFlag_UPDATE( (TIM_TypeDef*)Timerx) )         
        {
          TglPin.High();
          LL_TIM_ClearFlag_UPDATE((TIM_TypeDef*)Timerx);   
          //m_Ticks++;
          TglPin.Low();
        } 
      }
}

#endif 