

#include "AnalogIn.h"


void Uart_DMA_Test();

extern HAL::Uart uart1;
extern HAL::AnalogIn adc0;
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
    adcVref.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    //LL_ADC_AWD_CHANNEL_0_REG
    //adc0.EnableAnalogWatchdog(0,2600,2300,&AdcCallback);
    InitDone = true;
  }

  while(1)
  {    
    VolA0 = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adc0.Read(),LL_ADC_RESOLUTION_12B);
    VolA1 = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adc1.Read(),LL_ADC_RESOLUTION_12B);
    VolA2 = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adc2.Read(),LL_ADC_RESOLUTION_12B);
    Vref  = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adcVref.Read(),LL_ADC_RESOLUTION_12B);
    temperature = adc0.GetChipTemperature();
    sprintf(DbgBuf,(char*)"VolA0 = %d \t VolA1 = %d \t VolA2 = %d \t Vref = %d \tTemperature = %d",VolA0,VolA1,VolA2,Vref,temperature);
    printf("%s\n\r",DbgBuf); 
    LL_mDelay(100);
  }
  
}
