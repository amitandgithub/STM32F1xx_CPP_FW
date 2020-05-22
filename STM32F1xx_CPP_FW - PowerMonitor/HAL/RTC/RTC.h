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

#include"CPP_HAL.h"
#include"chip_peripheral.h"
#include"Utils.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Date_N_Time.h"
#include"ClockManager.h"


extern HAL::InterruptManager InterruptManagerInstance;

namespace HAL
{  
#define RTC_BASE_YEAR 1970
  
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
    
    enum : uint8_t
    {
      Mon=1,
      Tue,
      Wed,
      Thu,
      Fri,
      Sat,
      Sun
    };
    
    typedef struct
    {      
      uint8_t Hours;      
      uint8_t Minutes;      
      uint8_t Seconds;      
    }Time_t;
    
    typedef struct
    {
      uint8_t Year;
      uint8_t Month;
      uint8_t Day;
      uint8_t Weekday;     
    }Date_t;
    
    typedef Time_t RtcTime_t;
    typedef Date_t RtcDate_t;
    typedef Callback* RTCCallback_t;
    
    RTCStatus_t HwInit(HAL::ClockManager::RTCClock_t Clock = HAL::ClockManager::CLOCK_LSI);
    
    uint32_t ReadTimeCounter();
    
    RTCStatus_t SetCounter(uint32_t counter_time);
    
    void CountertoTime(uint32_t counter, RtcTime_t* aRtcTime);
    
    void CountertoTimeStr(uint32_t counter, char* timeStr);
      
    RTCStatus_t GetTime(RtcTime_t* aRtcTime);
    
    RTCStatus_t GetTime(char* timeStr);
    
    RTCStatus_t GetDate(RtcDate_t* RtcDate);
    
    RTCStatus_t GetDate(char* dateStr);
    
    RTCStatus_t SetTime(RtcTime_t* aRtcTime);  
    
    RTCStatus_t SetTime(uint8_t Hours, uint8_t Minutes, uint8_t Seconds); 
    
    RTCStatus_t Set(uint16_t year,uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);
    
    uint8_t WeekDayNum(uint32_t nYear, uint8_t nMonth, uint8_t nDay);
      
    uint8_t check_for_leap_year(uint16_t year);
    
    RTCStatus_t SetAlarm(RtcTime_t* AlarmTime, RTCCallback_t RTCCallback); 
    
    RTCStatus_t SetAlarm(uint8_t Hours, uint8_t Minutes, uint8_t Seconds, RTCCallback_t RTCCallback);
    
    RTCStatus_t ResetAlarm(RtcTime_t* AlarmTime); 
    
    RTCStatus_t ResetAlarm(uint8_t Hours, uint8_t Minutes, uint8_t Seconds);
    
    void RegisterCallback(CallbackSource_t CallbackSource, RTCCallback_t RTCCallback );
    
    void UnRegisterCallback(CallbackSource_t CallbackSource );
    
    virtual void ISR();
    
  private:     
    //RtcTime_t m_RtcTime;
    RtcDate_t m_RtcDate;
    RTCCallback_t m_AlarmCallback;
    RTCCallback_t m_SecondsCallback;
    RTCCallback_t m_CounterOverflowCallback;
  };
  
  
  
}






#endif // RTC_h