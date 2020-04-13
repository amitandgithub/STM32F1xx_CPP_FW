

#include "AnalogIn.h"


void Uart_DMA_Test();

extern HAL::Uart uart1;
extern HAL::AnalogIn adc0;
extern HAL::AnalogIn ChipTemperature;
HAL::AnalogIn adc1(LL_ADC_CHANNEL_1);
HAL::AnalogIn adc2(LL_ADC_CHANNEL_3);
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
    adc0.HwInit(A0,LL_ADC_SAMPLINGTIME_384CYCLES);
    adc1.HwInit(A1,LL_ADC_SAMPLINGTIME_384CYCLES);
    adc2.HwInit(A2,LL_ADC_SAMPLINGTIME_384CYCLES);
    adcVref.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_384CYCLES);
    ChipTemperature.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_384CYCLES);
    //LL_ADC_AWD_CHANNEL_0_REG
    adc0.EnableAnalogWatchdog(0,2600,2300,&AdcCallback);
    InitDone = true;
  }

  while(1)
  {    
    VolA0 = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adc0.Read(),LL_ADC_RESOLUTION_12B);
    VolA1 = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adc1.Read(),LL_ADC_RESOLUTION_12B);
    VolA2 = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adc2.Read(),LL_ADC_RESOLUTION_12B);
    Vref  = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adcVref.Read(),LL_ADC_RESOLUTION_12B);
    temperature = adc0.GetChipTemperature();
   //Vref  = __LL_ADC_CALC_VREFANALOG_VOLTAGE(__VREFINT_ADC_DATA__,__ADC_RESOLUTION__)
      
    //temperature = __LL_ADC_CALC_TEMPERATURE(Vref,ChipTemperature.Read(),LL_ADC_RESOLUTION_12B) ;
    //temperature = __LL_ADC_CALC_TEMPERATURE(1220,1200,LL_ADC_RESOLUTION_12B) ;
    
  // ftoa(temperature, (char*)&DbgBuf[90], 1);
   sprintf(DbgBuf,(char*)"VolA0 = %d \t VolA1 = %d \t VolA2 = %d \t Vref = %d \tTemperature = %d",VolA0,VolA1,VolA2,Vref,temperature);
   printf("%s\n\r",DbgBuf); 
   // printf("%d      %d      %d      %d      %d\n\r",VolA0,VolA1,VolA2,Vref,temperature);
    LL_mDelay(10);
  }
  
}
