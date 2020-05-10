/******************
** FILE: ADC.cpp
**
** DESCRIPTION:
**  ADC implementation
**
** CREATED: 31/1/2020, by Amit Chaudhary
******************/

#include "ADC.h"

namespace HAL
{

  Adc::ADCState_t Adc::m_ADCState = Adc::ADC_RESET;
  Callback* Adc::m_WDCallback;
  
  Adc::Adc(ADC_TypeDef* ADCx):m_ADCx(ADCx)
  {

  }
  

  void Adc::HwInit(Port_t Port, PIN_t Pin)
  {
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct;
    LL_ADC_InitTypeDef ADC_InitStruct;
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct;  
    
    if(Port) 
    {
      HAL::DigitalIO::SetInputMode(Port,Pin,ANALOG);
    }   
    
    if(m_ADCState == ADC_RESET)
    {    
      HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_ADC1);
      m_ADCState = ADC_READY;
      Disable();
      
      ADC_CommonInitStruct.Multimode      = LL_ADC_MULTI_INDEPENDENT;
      LL_ADC_CommonInit((ADC_Common_TypeDef *)m_ADCx, &ADC_CommonInitStruct);
      
      ADC_InitStruct.DataAlignment        = LL_ADC_DATA_ALIGN_RIGHT;
      ADC_InitStruct.SequencersScanMode   = LL_ADC_SEQ_SCAN_DISABLE;//LL_ADC_SEQ_SCAN_ENABLE;  
      LL_ADC_Init(m_ADCx, &ADC_InitStruct);
      
      ADC_REG_InitStruct.TriggerSource    = LL_ADC_REG_TRIG_SOFTWARE;
      ADC_REG_InitStruct.SequencerLength  = LL_ADC_REG_SEQ_SCAN_DISABLE;//LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
      ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
      ADC_REG_InitStruct.ContinuousMode   = LL_ADC_REG_CONV_SINGLE;//LL_ADC_REG_CONV_SINGLE; LL_ADC_REG_CONV_CONTINUOUS
      ADC_REG_InitStruct.DMATransfer      = LL_ADC_REG_DMA_TRANSFER_NONE;  
      LL_ADC_REG_Init(m_ADCx, &ADC_REG_InitStruct);  
      
      InterruptManagerInstance.RegisterDeviceInterrupt(ADC1_2_IRQn,0,this);
      
      Enable();
      
      LL_ADC_StartCalibration(m_ADCx);
      while(LL_ADC_IsCalibrationOnGoing(m_ADCx));
    }  

  }
   
  void Adc::HwDeinit()
  {  
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_ADC1);    
    LL_ADC_DeInit(m_ADCx);   
  }
  
  void Adc::EnableAnalogWatchdog(uint32_t chanel,uint32_t HighTh, uint32_t LowTh, Callback* pCallback)
  {
    m_WDCallback = pCallback;      
                              
    LL_ADC_SetAnalogWDThresholds(m_ADCx, LL_ADC_AWD_THRESHOLD_HIGH, __LL_ADC_CALC_VOLTAGE_TO_DATA(3300,HighTh,LL_ADC_RESOLUTION_12B));
    LL_ADC_SetAnalogWDThresholds(m_ADCx, LL_ADC_AWD_THRESHOLD_LOW,  __LL_ADC_CALC_VOLTAGE_TO_DATA(3300,LowTh,LL_ADC_RESOLUTION_12B));
    
    m_ADCx->CR1 &= ~ADC_CR1_AWDCH;
    m_ADCx->CR1 |= (chanel & 0x0000001FU) | ADC_CR1_AWDEN | ADC_CR1_JAWDEN| ADC_CR1_AWDSGL;
    
    LL_ADC_EnableIT_AWD1(m_ADCx);
  }
  
  bool Adc::StartConversion()
  {
    Enable();
    return true;
  }
  
  /* Converts the Input voltage to a 12 bit number.
  *  Return : 12-Bit ADC value or 0xFFFF if error
  */
  uint16_t Adc::Read(uint32_t Chanel)
  {  
  
    Enable(); // Wake up ADC from deep sleep
    
    if(Chanel == LL_ADC_CHANNEL_16)
    {
      LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(m_ADCx), LL_ADC_PATH_INTERNAL_TEMPSENSOR);
    } 
     
    LL_ADC_REG_SetSequencerRanks(m_ADCx, LL_ADC_REG_RANK_1, Chanel);          
    
    if( StartConversion() == false) return 0xFFFF; // error
    
    //LL_mDelay(1);
    
    if( ADC_CONVERSION_DONE(m_ADCx)) 
    {
      return 0xFFFF; // error 
    }
    else        
    {
      return LL_ADC_REG_ReadConversionData12(m_ADCx);
    }
  }

  void Adc::ISR()
  {
    if(m_ADCx->SR & LL_ADC_FLAG_AWD1)
    {
      m_ADCx->SR &= ~LL_ADC_FLAG_AWD1;
      if(m_WDCallback) m_WDCallback->CallbackFunction();
    }
    
  }














}