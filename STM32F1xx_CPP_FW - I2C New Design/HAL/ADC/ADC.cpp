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

  uint8_t ADC::GlobalInitDone;
  
  ADC::ADC(ADC_TypeDef* ADCx, uint32_t Chanel) : m_ADCx(ADCx),m_Chanel(Chanel)
  {

  }
  

  void ADC::HwInit(Port_t Port, PIN_t Pin, uint32_t SamplingTime)
  {
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct;
    LL_ADC_InitTypeDef ADC_InitStruct;
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct;  
    //GpioInput AnalogPin(Port,Pin);
    
    if(Port) 
    {
      //AnalogPin.HwInit(ANALOG);
      HAL::DigitalIO::SetInputMode(Port,Pin,ANALOG); //Rx.HwInit(INPUT_PULLDOWN);
    }
    
    if(m_ADCx == ADC1)
    {
      LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    }
    else if(m_ADCx == ADC2)
    {
      LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC2);
    } 
    
    if(GlobalInitDone == 0)
    {    
      GlobalInitDone = 1;
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
      
      Enable();
      
      LL_ADC_StartCalibration(m_ADCx);
      while(LL_ADC_IsCalibrationOnGoing(m_ADCx));
    }
     LL_ADC_SetChannelSamplingTime(m_ADCx, m_Chanel, SamplingTime); 
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
  
  uint16_t ADC::Read()
  {
    uint16_t result;
    
      Enable(); // Wake up ADC from deep sleep
      if(m_Chanel == LL_ADC_CHANNEL_16)
      {
         LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(m_ADCx), LL_ADC_PATH_INTERNAL_TEMPSENSOR);
      } 
      
      LL_ADC_REG_SetSequencerRanks(m_ADCx, LL_ADC_REG_RANK_1, m_Chanel);          
    
      Enable();     // Start conversion
     
      //LL_mDelay(1);
      
      if( ADC_CONVERSION_DONE(m_ADCx)) 
      {
        result =  0xffff; // Error   
      }
      else        
      {
        result = LL_ADC_REG_ReadConversionData12(m_ADCx);
      }
      
      //Disable();
      
      return result;
  }
  

//Avg_Slope Average slope 4.0 4.3 4.6 mV/°C
//V25 Voltage at 25 °C 1.34 1.43 1.52 V

















}