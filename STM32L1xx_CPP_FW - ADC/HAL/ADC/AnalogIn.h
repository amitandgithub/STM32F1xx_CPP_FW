/******************
** CLASS: AnalogIn
**
** DESCRIPTION:
**  Analog Input functionality
**
** CREATED: 7-Apr-2020, by Amit Chaudhary (Under Corona Lockdown till 14 Apr 2020)
**
** FILE: AnalogIn.h
**
******************/
#ifndef AnalogIn_h
#define AnalogIn_h


#include"ADC.h"
#include"InterruptManager.h"


namespace HAL
{
  class AnalogIn : public Adc
  {
  public:
    
    AnalogIn(uint32_t Chanel) : m_Chanel(Chanel), Adc(ADC1)
    {
      
    }
    
    void HwInit(Port_t Port, PIN_t Pin, uint32_t SamplingTime)
    {
      Adc::HwInit(Port,Pin);
      Disable();
      LL_ADC_SetChannelSamplingTime(ADC1, m_Chanel, SamplingTime); 
      LL_ADC_REG_SetFlagEndOfConversion(ADC1,LL_ADC_REG_FLAG_EOC_UNITARY_CONV);
      Enable();
    }
    
    uint16_t Read(){ return Adc::Read(m_Chanel);}
    
//    uint16_t operator= (AnalogIn const& obj)
//    {
//      return Read();
//    } 
    
  private:
    uint32_t m_Chanel; 
    static Callback* m_WDCallback;
  };  
  
} 
#endif //AnalogIn_h