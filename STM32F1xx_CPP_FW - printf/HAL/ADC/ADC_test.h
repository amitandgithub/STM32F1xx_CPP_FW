

#include "ADC.h"


void Uart_DMA_Test();

extern HAL::Uart printf_uart;
extern HAL::ADC adc0;
extern HAL::ADC ChipTemperature;
HAL::ADC adc1(ADC1,LL_ADC_CHANNEL_1);
HAL::ADC adc2(ADC1,LL_ADC_CHANNEL_2);
HAL::ADC adcVref(ADC1,LL_ADC_CHANNEL_17);
HAL::ADC ACS_IN(ADC1,LL_ADC_CHANNEL_10);


class ADCCallback_t : public HAL::ADC::ADCCallback
{
public:
  virtual void CallbackFunction(HAL::ADC::ADCStatus_t Status)
  {
    
  }
};

ADCCallback_t AdcCallback;

uint16_t VolA0,VolA1,VolA2,Vref,VolACS;
float temperature;
char DbgBuf[100];
void Adc_Test()
{
  
  static bool InitDone;  
  if(InitDone == false)
  {
    printf_uart.HwInit(9600);
    adc0.HwInit(A0,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    adc1.HwInit(A1,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    adc2.HwInit(A2,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    adcVref.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    ChipTemperature.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    ACS_IN.HwInit(C1,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    InitDone = true;
  }

  while(1)
  {    
    VolA0 = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adc0.Read(),LL_ADC_RESOLUTION_12B);
    VolA1 = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adc1.Read(),LL_ADC_RESOLUTION_12B);
    VolA2 = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adc2.Read(),LL_ADC_RESOLUTION_12B);
    Vref  = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adcVref.Read(),LL_ADC_RESOLUTION_12B);
    VolACS = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300,ACS_IN.Read(),LL_ADC_RESOLUTION_12B);
    temperature = __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(4300,1400,25,3300,ChipTemperature.Read(),LL_ADC_RESOLUTION_12B);
    
    
    ftoa(temperature, (char*)&DbgBuf[90], 1);
    sprintf(DbgBuf,(char*)"VolA0 = %d \t VolA1 = %d \t VolA2 = %d \t Vref = %d \t VolACS = %d \tTemperature = %s",VolA0,VolA1,VolA2,Vref,VolACS,(char*)&DbgBuf[90]);
    
    HAL::DBG_PRINT((uint8_t*)DbgBuf,90);
    HAL::DBG_PRINT_NL();
    
    LL_mDelay(500);
  }
  
}
