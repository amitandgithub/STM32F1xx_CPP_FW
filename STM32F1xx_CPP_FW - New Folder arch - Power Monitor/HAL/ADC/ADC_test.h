

#include "AnalogIn.h"


void Uart_DMA_Test();

extern HAL::Uart uart1;
extern HAL::AnalogIn adc0;
extern HAL::AnalogIn ChipTemperature;
HAL::AnalogIn adc1(LL_ADC_CHANNEL_1);
HAL::AnalogIn adc2(LL_ADC_CHANNEL_2);
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
    //uart1.HwInit(9600);
    adc0.HwInit(A0,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    adc1.HwInit(A1,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    adc2.HwInit(A2,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    ChipTemperature.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    adcVref.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_239CYCLES_5);

    adc0.EnableAnalogWatchdog(0,3250,1000,&AdcCallback);
    InitDone = true;
  }

  while(1)
  {        
    VolA0 = adc0.ReadVoltage();
    VolA1 = adc1.ReadVoltage();
    VolA2 = adc2.ReadVoltage();
    Vref  = adcVref.ReadVoltage();
    
    temperature = __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(4300,1400,25,3300,ChipTemperature.Read(),LL_ADC_RESOLUTION_12B);
    
    sprintf(DbgBuf,(char*)"VolA0 = %d \t VolA1 = %d \t VolA2 = %d \t Vref = %d \tTemperature = %d",VolA0,VolA1,VolA2,Vref,temperature);
    printf("%s\n\r",DbgBuf); 
    //LL_mDelay(50);
  }
  
}
