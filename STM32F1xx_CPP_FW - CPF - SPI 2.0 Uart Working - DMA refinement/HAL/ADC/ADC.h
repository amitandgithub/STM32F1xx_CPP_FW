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
      ADC_DMA_ERROR,
      ADC_SLAVE_TX_DONE,
      ADC_SLAVE_RX_DONE,
      ADC_SLAVE_TXRX_DONE,
      ADC_SLAVE_TX_DMA_DONE,
      ADC_SLAVE_RX_DMA_DONE,
      ADC_SLAVE_TXRX_DMA_DONE,
      ADC_BUSY,
      ADC_INVALID_PARAMS,
      ADC_TXE_TIMEOUT,
      ADC_RXNE_TIMEOUT,
      ADC_BUSY_TIMEOUT,
      ADC_TXN_POSTED,
      ADC_TXN_QUEUE_ERROR,
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
      ADC_MASTER_TX,
      ADC_MASTER_RX,
      ADC_MASTER_TXRX,
      ADC_SLAVE_TX,
      ADC_SLAVE_RX,
      ADC_SLAVE_TXRX,
      ADC_SLAVE_RX_LISTENING,
      ADC_MASTER_TX_DMA,
      ADC_MASTER_RX_DMA,
      ADC_SLAVE_TX_DMA,
      ADC_SLAVE_RX_DMA,
      ADC_MASTER_TXRX_DMA,
      ADC_ERROR_BUSY_TIMEOUT,
    }ADCState_t;          
    
    ADC(uint8_t adcx);
    
    ~ADC(){};
    
    void HwInit();
    
    void HwDeinit();
    
    void Enable(){LL_ADC_Enable(m_ADCx);}
    
    void Disable(){LL_ADC_Disable(m_ADCx);}
    
  private:
    ADCx_t m_ADCx;
    
  };
  
}
    
#endif