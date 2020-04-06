/******************
** CLASS: ADC
**
** DESCRIPTION:
**  Implements the ADC driver
**
** CREATED: 31/1/2020, by Amit Chaudhary
**
** FILE: ADC.h
**
******************/
#ifndef ADC_h
#define ADC_h

#include"chip_peripheral.h"
#include"CPP_HAL.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"DebugLog.h"
#include"DMA.h"
#include"DigitalIO.h"
#include <stdio.h>

extern HAL::DMA dma1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::InterruptManager InterruptManagerInstance;   


namespace HAL
{    
  
  class ADC //: public InterruptSource
  {
  public:
    static const uint32_t ADC1_RX_DMA_CHANNEL = 2;
    static const uint32_t ADC1_TX_DMA_CHANNEL = 3;
    static const uint32_t ADC2_RX_DMA_CHANNEL = 4;
    static const uint32_t ADC2_TX_DMA_CHANNEL = 5;
    static const uint32_t ADC_TIMEOUT = 5000U;
    
    typedef enum  : uint8_t
    {
      ADC_OK,
      ADC_BUSY,
      ADC_DMA_ERROR,
    }ADCStatus_t;
    
    typedef uint32_t Hz_t ;
    
    typedef ADC_TypeDef* ADCx_t;    
    
    class ADCCallback
    {
    public:
      virtual void CallbackFunction(ADCStatus_t ADCStatus) = 0;
    };
    
    typedef ADCCallback* ADCCallback_t;  
    
    typedef enum
    {
      ADC_RESET,
      ADC_READY,
      ADC_IN_PROGRESS,
      ADC_DONE,
    }ADCState_t;          
    
    ADC(ADC_TypeDef* ADCx, uint32_t Chanel);
    
    ADC(Port_t Port, PIN_t Pin);
    
    ~ADC(){};
    
    void HwInit(Port_t Port, PIN_t Pin, uint32_t SamplingTime);
    
    void HwDeinit();
    
    void Enable(){LL_ADC_Enable(m_ADCx);}
    
    void Disable(){LL_ADC_Disable(m_ADCx);}
    
    uint16_t Read();
    
    uint16_t ReadVoltage();
    
    void Start();
    
  private:
    uint32_t m_Chanel;
    ADCx_t m_ADCx;
    static uint8_t GlobalInitDone;
   // HAL::GpioInput Pin;    
  };
  
}

#define ADC_CONVERSION_DONE(__ADC__) WaitOnFlag(&__ADC__->SR, LL_ADC_FLAG_EOS, 0, ADC_TIMEOUT)
    
#endif