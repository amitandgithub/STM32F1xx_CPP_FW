

#include "AnalogIn.h"


void Uart_DMA_Test();

extern HAL::Uart uart1;
extern HAL::AnalogIn adc0;
extern HAL::AnalogIn ChipTemperature;
HAL::AnalogIn adc1(LL_ADC_CHANNEL_1);
HAL::AnalogIn adc3(LL_ADC_CHANNEL_3);
HAL::AnalogIn adcVref(LL_ADC_CHANNEL_VREFINT);

uint16_t VolA0,VolA1,VolA2,Vref;
int32_t temperature;
char DbgBuf[200];

class ADCCallback_t : public Callback
{
public:
  virtual void CallbackFunction()
  {
    printf("Analog Watchdog activated at : %d mv\n\r",__LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adc0.Read(),LL_ADC_RESOLUTION_12B));
  }
  };
  
  ADCCallback_t AdcCallback;
  
  
  void Adc_Test()
  {
    
    static bool InitDone;  
    if(InitDone == false)
    {
      adc0.HwInit(A0,LL_ADC_SAMPLINGTIME_48CYCLES);
      adc1.HwInit(A1,LL_ADC_SAMPLINGTIME_48CYCLES);
      adc3.HwInit(A3,LL_ADC_SAMPLINGTIME_48CYCLES);
      adcVref.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_48CYCLES);
      ChipTemperature.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_48CYCLES);
      
       adc0.EnableAnalogWatchdog(0,3250,2000,&AdcCallback);
      InitDone = true;
    }
      
    while(1)
    {        
      VolA0 = adc0.ReadVoltage();
      VolA1 = adc1.ReadVoltage();
      VolA2 = adc3.ReadVoltage();
      Vref  = adcVref.ReadVoltage();    
      
      temperature = __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(1750,612,110,3000,ChipTemperature.Read(),LL_ADC_RESOLUTION_12B); 
      
      sprintf(DbgBuf,(char*)"VolA0 = %d \t VolA1 = %d \t VolA2 = %d \t Vref = %d \tTemperature = %d",VolA0,VolA1,VolA2,Vref,temperature);
      printf("%s\n\r",DbgBuf); 
      LL_mDelay(100);
    }
      
  }
    