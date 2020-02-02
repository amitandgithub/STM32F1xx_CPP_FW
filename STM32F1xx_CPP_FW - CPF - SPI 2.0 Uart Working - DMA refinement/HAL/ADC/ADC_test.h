

#include "ADC.h"


void Uart_DMA_Test();

extern HAL::ADC adc1;


class ADCCallback_t : public HAL::ADC::ADCCallback
{
public:
  virtual void CallbackFunction(HAL::ADC::ADCStatus_t Status)
  {
    
  }
};

ADCCallback_t AdcCallback;

void Adc_Test()
{
  static bool InitDone;  
  if(InitDone == false)
  {
    adc1.HwInit();
    InitDone = true;
  }

  while(1)
  {
    
  }
  
}
