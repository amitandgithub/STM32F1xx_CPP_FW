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
#include"ClockManager.h"

extern HAL::DMA dma1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::InterruptManager InterruptManagerInstance;   

namespace HAL
{    
  
  class Adc :  public InterruptSource
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
    
    Adc(ADC_TypeDef* ADCx);
    
    Adc(Port_t Port, PIN_t Pin);
    
    ~Adc(){};
    
    void HwInit(Port_t Port, PIN_t Pin);
    
    void HwDeinit();
    
    void Enable(){LL_ADC_Enable(ADC1);}
    
    void Disable(){LL_ADC_Disable(ADC1);}
    
    bool StartConversion();
    
    uint16_t Read(uint32_t Chanel);  
    
    void Start();
    
    void EnableAnalogWatchdog(uint32_t chanel,uint32_t HighTh, uint32_t LowTh, Callback* pCallback);
    
    void DisableAnalogWatchdog(){LL_ADC_DisableIT_AWD1(ADC1); m_WDCallback = nullptr;}
    
    void ISR() final;
    
  protected:
    ADC_TypeDef* m_ADCx;
    static ADCState_t m_ADCState;
    static Callback* m_WDCallback;

  };
  
}

#define ADC_CONVERSION_DONE(__ADC__) WaitOnFlag(&__ADC__->SR, LL_ADC_FLAG_EOS, 0, ADC_TIMEOUT)

#define ADC_ENABLED(__ADC__) WaitOnFlag(&__ADC__->SR, ADC_SR_ADONS, 0, ADC_TIMEOUT)

#define __LL_ADC_CALC_VOLTAGE_TO_DATA(__VREFANALOG_VOLTAGE__,\
                                      __VOLTAGE_MV__,\
                                      __ADC_RESOLUTION__)           \
  ( __LL_ADC_DIGITAL_SCALE(__ADC_RESOLUTION__) * (__VOLTAGE_MV__)   \
   /(__VREFANALOG_VOLTAGE__)                                        \
  )
    
#endif