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

  ADC::ADC(ADC_TypeDef* ADCx, uint8_t Chanel) : m_ADCx(ADCx), m_ADCState(ADC_RESET), m_Chanel(Chanel)
  {

  }
  

  void ADC::HwInit()
  {
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct;
    LL_ADC_InitTypeDef ADC_InitStruct;
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct;  
    GpioInput Pin(A0);    
    GpioInput Pin1(A2);
    Pin.HwInit(ANALOG);
    Pin1.HwInit(ANALOG);
    
    if(m_ADCx == ADC1)
    {
      LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    }
    else if(m_ADCx == ADC2)
    {
      LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC2);
    } 
    
    Disable();
    
    ADC_CommonInitStruct.Multimode      = LL_ADC_MULTI_INDEPENDENT;
    LL_ADC_CommonInit((ADC_Common_TypeDef *)m_ADCx, &ADC_CommonInitStruct);
    
    ADC_InitStruct.DataAlignment        = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.SequencersScanMode   = LL_ADC_SEQ_SCAN_DISABLE;//LL_ADC_SEQ_SCAN_ENABLE;    
    LL_ADC_Init(m_ADCx, &ADC_InitStruct);
    
    ADC_REG_InitStruct.TriggerSource    = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerLength  = LL_ADC_REG_SEQ_SCAN_DISABLE;//LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode   = LL_ADC_REG_CONV_CONTINUOUS;//LL_ADC_REG_CONV_SINGLE; LL_ADC_REG_CONV_CONTINUOUS
    ADC_REG_InitStruct.DMATransfer      = LL_ADC_REG_DMA_TRANSFER_NONE;  
    LL_ADC_REG_Init(m_ADCx, &ADC_REG_InitStruct);
   
  
    Enable();
    
    LL_mDelay(1);
    
    LL_ADC_StartCalibration(m_ADCx);
    while(LL_ADC_IsCalibrationOnGoing(m_ADCx));
    
    m_ADCState = ADC_READY;
  }
   
  void ADC::HwDeinit()
  {    
    if(m_ADCx == ADC1)
    {
      LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_ADC1);
    }
    else if(m_ADCx == ADC2)
    {
      LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_ADC2);
    }     
    LL_ADC_DeInit(m_ADCx);   
  }
  
  uint16_t ADC::Read(uint8_t Channel, uint32_t SamplingTime)
  {
    uint16_t result;
    
      Enable(); // Wake up ADC from deep sleep
      LL_mDelay(1); // Wake up stabalize time
      Enable();     // ADC active for conversion
      
      LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, Channel);
      LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, SamplingTime);
      
      if(Channel == 16)
         LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_TEMPSENSOR);
    
      LL_ADC_REG_StartConversionSWStart(m_ADCx);
      
      if( ADC_CONVERSION_DONE(m_ADCx)) 
        result =  0xffff; // Error   
      else        
        result = LL_ADC_REG_ReadConversionData12(m_ADCx);
      
      Disable();
      
      return result;
  }
  

//Avg_Slope Average slope 4.0 4.3 4.6 mV/°C
//V25 Voltage at 25 °C 1.34 1.43 1.52 V

















}