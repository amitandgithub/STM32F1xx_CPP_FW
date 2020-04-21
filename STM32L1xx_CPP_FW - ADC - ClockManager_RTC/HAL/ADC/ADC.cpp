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
  
  Adc::Adc(ADC_TypeDef* ADCx)
  {

  }
  

  void Adc::HwInit(Port_t Port, PIN_t Pin)
  {
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct;
    LL_ADC_InitTypeDef ADC_InitStruct;
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct;  
    
    // Enable HSI clock for ADC peripheral
    HAL::ClockManager::SetSystemClock(HAL::ClockManager::CLOCK_HSI,LL_RCC_PLL_MUL_4,LL_RCC_PLL_DIV_3);
    
    if(Port) 
    {
      HAL::DigitalIO::SetInputMode(Port,Pin,ANALOG);
    }
    
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    
    if(m_ADCState == ADC_RESET)
    {    
      m_ADCState = ADC_READY;
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
      
      InterruptManagerInstance.RegisterDeviceInterrupt(ADC1_IRQn,0,this);
      
      Enable();
    }  

  }
   
  void Adc::HwDeinit()
  {  
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_ADC1);    
    LL_ADC_DeInit(ADC1);   
  }
  
  void Adc::EnableAnalogWatchdog(uint32_t chanel,uint32_t HighTh, uint32_t LowTh, Callback* pCallback)
  {
    m_WDCallback = pCallback;      
                              
    LL_ADC_SetAnalogWDThresholds(ADC1, LL_ADC_AWD_THRESHOLD_HIGH, __LL_ADC_CALC_VOLTAGE_TO_DATA(3300,HighTh,LL_ADC_RESOLUTION_12B));
    LL_ADC_SetAnalogWDThresholds(ADC1, LL_ADC_AWD_THRESHOLD_LOW,  __LL_ADC_CALC_VOLTAGE_TO_DATA(3300,LowTh,LL_ADC_RESOLUTION_12B));
    
    ADC1->CR1 &= ~ADC_CR1_AWDCH;
    ADC1->CR1 |= (chanel & 0x0000001FU) | ADC_CR1_AWDEN | ADC_CR1_JAWDEN| ADC_CR1_AWDSGL;
    
    LL_ADC_EnableIT_AWD1(ADC1);
  }
  
  bool Adc::StartConversion()
  {
    // ADC should be enabled and Chanel should be ready for concersion
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
  uint16_t Adc::Read(uint32_t Chanel)
  {    
    if(Chanel == LL_ADC_CHANNEL_16)
    {
      LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(m_ADCx), LL_ADC_PATH_INTERNAL_TEMPSENSOR);

    } 

    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, Chanel);          
    
    if( StartConversion() == false) 
      return 0xFFFF; // error

    if( ADC_CONVERSION_DONE(ADC1)) 
    {
      return 0xFFFF; // error 
    }
    else        
    {
      return LL_ADC_REG_ReadConversionData12(ADC1);
    } 
  }


//  int32_t Adc::GetChipTemperature()
//  {
////    Disable();
////    LL_mDelay(1);
////    Enable();
//    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_VREFINT);
//    LL_mDelay(1);
//    return __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(1750,612,110+5,3000,Read(LL_ADC_CHANNEL_TEMPSENSOR),LL_ADC_RESOLUTION_12B); 
//  }
  
  void Adc::ISR()
  {
    if(ADC1->SR & LL_ADC_FLAG_AWD1)
    {
      ADC1->SR &= ~LL_ADC_FLAG_AWD1;
      if(m_WDCallback) m_WDCallback->CallbackFunction();
    }
    
  }














}