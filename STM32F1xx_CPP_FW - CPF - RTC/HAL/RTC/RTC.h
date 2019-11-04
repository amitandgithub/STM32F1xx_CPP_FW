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

//#include"CPP_HAL.h"
#include "Utils.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"stm32f1xx_ll_rtc.h"
#include"stm32f1xx_ll_pwr.h"


//extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
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
    
    typedef LL_RTC_TimeTypeDef RtcTime_t;
    typedef LL_RTC_TimeTypeDef RtcDate_t;
    
//    Rtc();
//    
//    ~Rtc();
    
    RTCStatus_t HwInit(void *pInitStruct = nullptr);
    
    RTCStatus_t GetTime(RtcTime_t* aRtcTime);
    
    RTCStatus_t GetTime(char* timeStr);
    
    RTCStatus_t SetTime(RtcTime_t* aRtcTime);  
    
    RTCStatus_t SetTime(uint8_t Hours, uint8_t Minutes, uint8_t Seconds); 
    
    RTCStatus_t SetAlarm(RtcTime_t* AlarmTime); 
    
    RTCStatus_t SetAlarm(uint8_t Hours, uint8_t Minutes, uint8_t Seconds);
    
    virtual void ISR(){};
  private:
    uint32_t RTC_ReadTimeCounter();  
  };
  
  
  
}






#endif // RTC_h