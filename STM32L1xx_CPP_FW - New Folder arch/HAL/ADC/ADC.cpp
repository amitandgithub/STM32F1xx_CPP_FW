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

  uint8_t Adc::GlobalInitDone;
  
  Adc::Adc(ADC_TypeDef* ADCx, uint32_t Chanel) :m_Chanel(Chanel)
  {

  }
  

  void Adc::HwInit(Port_t Port, PIN_t Pin, uint32_t SamplingTime)
  {
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct;
    LL_ADC_InitTypeDef ADC_InitStruct;
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct;  
    
    if(Port) 
    {
      HAL::DigitalIO::SetInputMode(Port,Pin,ANALOG);
    }

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    
    if(GlobalInitDone == 0)
    {    
      GlobalInitDone = 1;
      Disable();
      
      ADC_CommonInitStruct.CommonClock      = LL_ADC_CLOCK_ASYNC_DIV1;
      LL_ADC_CommonInit((ADC_Common_TypeDef *)ADC1, &ADC_CommonInitStruct);
      
      ADC_InitStruct.DataAlignment        = LL_ADC_DATA_ALIGN_RIGHT;
      ADC_InitStruct.SequencersScanMode   = LL_ADC_SEQ_SCAN_DISABLE;//LL_ADC_SEQ_SCAN_ENABLE;  
      ADC_InitStruct.Resolution           = LL_ADC_RESOLUTION_12B;
      ADC_InitStruct.LowPowerMode         = (LL_ADC_LP_AUTOWAIT_NONE | LL_ADC_LP_AUTOPOWEROFF_NONE);
      LL_ADC_Init(ADC1, &ADC_InitStruct);
      
      ADC_REG_InitStruct.TriggerSource    = LL_ADC_REG_TRIG_SOFTWARE;
      ADC_REG_InitStruct.SequencerLength  = LL_ADC_REG_SEQ_SCAN_DISABLE;//LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
      ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
      ADC_REG_InitStruct.ContinuousMode   = LL_ADC_REG_CONV_SINGLE;//LL_ADC_REG_CONV_SINGLE; LL_ADC_REG_CONV_CONTINUOUS
      ADC_REG_InitStruct.DMATransfer      = LL_ADC_REG_DMA_TRANSFER_NONE;  
      LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);     
      
      Enable();
      
     // LL_ADC_StartCalibration(ADC1);
     // while(LL_ADC_IsCalibrationOnGoing(ADC1));
    }    
    
    Disable();
    LL_ADC_SetChannelSamplingTime(ADC1, m_Chanel, SamplingTime); 
    LL_ADC_REG_SetFlagEndOfConversion(ADC1,LL_ADC_REG_FLAG_EOC_UNITARY_CONV);
    Enable();
  }
   
  void Adc::HwDeinit()
  {  
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_ADC1);    
    LL_ADC_DeInit(ADC1);   
  }
  
  bool Adc::StartConversion()
  {
    if( (ADC1->SR & ADC_SR_ADONS) && ((ADC1->SR & ADC_SR_RCNR) == 0) )
    {      
      LL_ADC_REG_StartConversionSWStart(ADC1);
      return true;
    }
    return false;
  }
  
  /* Converts the Input voltage to a 12 bit number.
  *  Return : 12-Bit ADC value or 0xFFFF if error
  */
  uint16_t Adc::Read()
  {
    uint16_t result;
      
    if(m_Chanel == LL_ADC_CHANNEL_TEMPSENSOR)
    {
      Disable();
      Enable();
      LL_mDelay(1);
      LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_TEMPSENSOR | LL_ADC_PATH_INTERNAL_VREFINT);
    } 
      
      LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, m_Chanel);          
    
      //Enable();     // Start conversion
      
      if( StartConversion() == false) return 0xFFFF; // error
     
      //LL_mDelay(1);
      
      if( ADC_CONVERSION_DONE(ADC1)) 
      {
        result =  0xffff; // Error   
      }
      else        
      {
        result = LL_ADC_REG_ReadConversionData12(ADC1);
      }
      
      //Disable();
      
      return result;
  }
  

//Avg_Slope Average slope 4.0 4.3 4.6 mV/°C
//V25 Voltage at 25 °C 1.34 1.43 1.52 V

















}