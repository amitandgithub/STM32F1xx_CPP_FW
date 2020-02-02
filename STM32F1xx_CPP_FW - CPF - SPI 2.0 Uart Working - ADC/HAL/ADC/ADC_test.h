

#include "ADC.h"


void Uart_DMA_Test();

extern HAL::ADC adc0;
extern HAL::ADC ChipTemperature;


class ADCCallback_t : public HAL::ADC::ADCCallback
{
public:
  virtual void CallbackFunction(HAL::ADC::ADCStatus_t Status)
  {
    
  }
};

ADCCallback_t AdcCallback;

uint16_t VolA0,RawAdc,temperature;
float tempfloat;
void Adc_Test()
{
  static bool InitDone;  
  if(InitDone == false)
  {
    adc0.HwInit();
    InitDone = true;
  }

  while(1)
  { 

    temperature = adc0.Read(16,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    tempfloat = __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(4300,1390,25,3300,temperature,LL_ADC_RESOLUTION_12B);
    
    RawAdc = adc0.Read(0,LL_ADC_SAMPLINGTIME_239CYCLES_5);
     VolA0 = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,RawAdc,LL_ADC_RESOLUTION_12B);
    
    LL_mDelay(500);
  }
  
}
