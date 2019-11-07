/******************
** FILE: RTC.h
**
** DESCRIPTION:
**  RTC Class
**
** CREATED: 3/11/2019, by Amit Chaudhary
******************/



#ifndef RTC_h
#define RTC_h

#include"chip_peripheral.h"
#include"Utils.h"
#include"InterruptManager.h"
#include"Callback.h"


extern HAL::InterruptManager InterruptManagerInstance;

namespace HAL
{  
  
  class Rtc : public InterruptSource
  {
  public:
    
    typedef enum  : uint8_t
    {
      RTC_OK,
      RTC_ERROR,
    }RTCStatus_t;
    
    typedef enum
    {
      RTC_ALARM = RTC_CRH_ALRIE,
      RTC_SEC = RTC_CRH_SECIE,
      RTC_COUNTER_OW = RTC_CRH_OWIE,      
    }CallbackSource_t;
    
    typedef LL_RTC_TimeTypeDef RtcTime_t;
    typedef LL_RTC_TimeTypeDef RtcDate_t;
    typedef Callback* RTCCallback_t;
    
//    Rtc();
//    
//    ~Rtc();
    
    RTCStatus_t HwInit(void *pInitStruct = nullptr);
    
    RTCStatus_t GetTime(RtcTime_t* aRtcTime);
    
    RTCStatus_t GetTime(char* timeStr);
    
    RTCStatus_t SetTime(RtcTime_t* aRtcTime);  
    
    RTCStatus_t SetTime(uint8_t Hours, uint8_t Minutes, uint8_t Seconds); 
    
    RTCStatus_t SetAlarm(RtcTime_t* AlarmTime, RTCCallback_t RTCCallback); 
    
    RTCStatus_t SetAlarm(uint8_t Hours, uint8_t Minutes, uint8_t Seconds, RTCCallback_t RTCCallback);
    
    RTCStatus_t ResetAlarm(RtcTime_t* AlarmTime); 
    
    RTCStatus_t ResetAlarm(uint8_t Hours, uint8_t Minutes, uint8_t Seconds);
    
    void RegisterCallback(CallbackSource_t CallbackSource, RTCCallback_t RTCCallback );
    
    void UnRegisterCallback(CallbackSource_t CallbackSource );
    
    virtual void ISR();
    
  private:
    uint32_t RTC_ReadTimeCounter(); 
    
    RTCCallback_t m_AlarmCallback;
    RTCCallback_t m_SecondsCallback;
    RTCCallback_t m_CounterOverflowCallback;
  };
  
  
  
}






#endif // RTC_h