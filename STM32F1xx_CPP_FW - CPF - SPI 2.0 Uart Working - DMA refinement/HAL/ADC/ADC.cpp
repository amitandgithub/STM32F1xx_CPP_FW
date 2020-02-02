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

  ADC::ADC(uint8_t adcx)
  {
    if(adcx == 1)
    {
      m_ADCx = ADC1;
    }
    else if(adcx == 2)
    {
      m_ADCx = ADC2;
    }    
  }
  
  void ADC::HwInit()
  {
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct;
    LL_ADC_InitTypeDef ADC_InitStruct;
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct;      
    
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
    ADC_InitStruct.SequencersScanMode   = LL_ADC_SEQ_SCAN_ENABLE;    
    LL_ADC_Init(m_ADCx, &ADC_InitStruct);
    
    ADC_REG_InitStruct.TriggerSource    = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerLength  = LL_ADC_REG_SEQ_SCAN_DISABLE;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode   = LL_ADC_REG_CONV_SINGLE;
    ADC_REG_InitStruct.DMATransfer      = LL_ADC_REG_DMA_TRANSFER_NONE;  
    LL_ADC_REG_Init(m_ADCx, &ADC_REG_InitStruct);
       
    Enable();
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
  
























}