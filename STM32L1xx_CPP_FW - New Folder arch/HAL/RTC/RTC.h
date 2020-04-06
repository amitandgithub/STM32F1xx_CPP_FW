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

#include "CPP_HAL.h"
#include"chip_peripheral.h"
#include"Utils.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Date_N_Time.h"
#include"DigitalOut.h"

#define DD_MMM_YY 1

// RTC Alarm interrupt - EXTI Line 17, rising edge
// RTC Wakeup interrupt - EXTI Line 20, rising edge
// RTC Tamper interrupt - EXTI Line 19, rising edge
// RTC TimeStamp interrupt - EXTI Line 19, rising edge
extern HAL::InterruptManager InterruptManagerInstance;

namespace HAL
{  
  
  class Rtc : public InterruptSource
  {
  public:
    
    typedef enum  : uint8_t
    {
      RTC_122_US_TO_32_SEC,
      RTC_1_SEC_TO_18_HOURS,
      RTC_18_HOURS_TO_36_HOURS,
    }WakeUpTimerRange_t;
    
    typedef enum  : uint8_t
    {
      RTC_OK,
      RTC_ERROR,
    }RTCStatus_t;
    
    typedef enum  : uint8_t
    {
      EVERY_MINUTE,
      EVERY_HOUR,
      EVERY_DAY,
      EVERY_WEEK,
      EVERY_MONTH,
    }AlarmFreq_t;
    
    typedef enum
    {
      RTC_ALARM_A ,
      RTC_ALARM_B,
      RTC_WakewUP,
      RTC_TimeStamp,
      RTC_Tamper,
    }CallbackSource_t;
    
    typedef enum
    {
      RTC_TIMESTAMP_ON_FALLING = LL_RTC_TIMESTAMP_EDGE_FALLING,
      RTC_TIMESTAMP_ON_RISING = LL_RTC_TIMESTAMP_EDGE_RISING,
    }Rtc_Edge_t;
    
    typedef enum
    {
      ALARM_A_OUT = LL_RTC_ALARMOUT_ALMA,
      ALARM_B_OUT = LL_RTC_ALARMOUT_ALMB,
      WAKEUP_EVENT = LL_RTC_ALARMOUT_WAKEUP,
    }AlarmOut_t;
    
    typedef enum
    {
      OPEN_DRAIN = 0x00000000U,
      PUSH_PULL = RTC_TAFCR_ALARMOUTTYPE,
    }AlarmOutputType_t;
    
    typedef LL_RTC_TimeTypeDef RtcTime_t;
    typedef LL_RTC_DateTypeDef RtcDate_t;
    typedef LL_RTC_AlarmTypeDef RtcAlarm_t;
    typedef Callback RTCCallback_t;
    
    RTCStatus_t HwInit(RtcTime_t* RtcTime = nullptr,RtcDate_t* RtcDate = nullptr);
    
    void EnableRTCOutput();
    
    void Time2Str(uint32_t TimeCounter, char* timeStr);
    
    void Date2Str(uint32_t DateCounter, char* dateStr);
    
    /* Date and Time*/
    
    void GetTime(RtcTime_t* aRtcTime);
    
    void GetTime(char* timeStr);
    
    uint32_t GetTime_00HHMMSS(){return LL_RTC_TIME_Get(RTC);}
    
    uint32_t GetDate_WWDDMMYY(){return LL_RTC_DATE_Get(RTC);}
    
    RTCStatus_t SetTime(RtcTime_t* aRtcTime);  
    
    RTCStatus_t SetTime(uint8_t Hours, uint8_t Minutes, uint8_t Seconds); 
    
    uint32_t GetDay(){return LL_RTC_DATE_GetDay(RTC);}
    
    uint32_t GetMonth(){return LL_RTC_DATE_GetMonth(RTC);}
    
    uint32_t GetYear(){return LL_RTC_DATE_GetYear(RTC);}  
    
    uint32_t GetDate(char* dateStr);
    
    /* Alarm*/
    
    void EnableAlarmOutputEvent(AlarmOut_t AlarmOut, AlarmOutputType_t AlarmOutputType, bool ActiveLow = false);
    
    RTCStatus_t SetAlarmA(RtcAlarm_t* RtcAlarm, RTCCallback_t* RTCCallback); 
    
    RTCStatus_t SetAlarmB(RtcAlarm_t* RtcAlarm, RTCCallback_t* RTCCallback);
    
    void EnableAlarmA(){RTC->CR |= RTC_CR_ALRAIE | RTC_CR_ALRAE;}
    
    void EnableAlarmB(){RTC->CR |= RTC_CR_ALRBIE | RTC_CR_ALRBE;}   
    
    void DisableAlarmA(){RTC->CR &= ~(RTC_CR_ALRAIE | RTC_CR_ALRAE);}
    
    void DisableAlarmB(){RTC->CR &= ~(RTC_CR_ALRBIE | RTC_CR_ALRBE);}   
    
    RTCStatus_t SetMinuteAlarm(char AlarmA_B, RTCCallback_t* RTCCallback);
    
    RTCStatus_t SetHourlyAlarm(char AlarmA_B, RTCCallback_t* RTCCallback);
    
    RTCStatus_t SetDailyAlarm(char AlarmA_B, uint8_t Hour, uint8_t Minutes, uint8_t Seconds, RTCCallback_t* RTCCallback);
    
    RTCStatus_t SetWeeklyAlarm(char AlarmA_B, uint8_t DayOfWeek, uint8_t Hour, uint8_t Minutes, uint8_t Seconds, RTCCallback_t* RTCCallback);
    
    RTCStatus_t SetMonthlyAlarm(char AlarmA_B, uint8_t DayOfMonth, uint8_t Hour, uint8_t Minutes, uint8_t Seconds, RTCCallback_t* RTCCallback);
    
    /* Wake Up Timer*/
    
    void SetWakeUpTimer(WakeUpTimerRange_t WakeUpTimerRange, uint32_t Time, RTCCallback_t* RTCCallback);
    
    void EnableWakeUpTimer(){ RTC->CR |= RTC_CR_WUTIE | RTC_CR_WUTE;}
    
    void DisableWakeUpTimer(){ RTC->CR &= ~(RTC_CR_WUTIE | RTC_CR_WUTE);}
    
    /* TimeStamp*/
    
    void EnableTimestampIntput(){EnableRTCOutput();}
    
    void SetTimestampEvent(Rtc_Edge_t Rtc_Edge, RTCCallback_t* RTCCallback);   
    
    void EnableTimeStamp(){RTC->CR |= RTC_CR_TSIE | RTC_CR_TSE;}
    
    void DisableTimeStamp(){RTC->CR &= ~(RTC_CR_TSIE | RTC_CR_TSE);}   
    
    uint32_t GetTimestampTime(){return LL_RTC_TS_GetTime(RTC);}
    
    uint32_t GetTimestampDate(){return LL_RTC_TS_GetDate(RTC);}
    
    void GetTimestampTime(char* timeStr);
    
    void GetTimestampDate(char* timeStr);
    
     /* Backup Register*/
    
    uint32_t ReadBackupReg(uint32_t reg){return LL_RTC_BAK_GetRegister(RTC,reg);}
    
    void WriteBackupReg(uint32_t reg,uint32_t Data){LL_RTC_BAK_SetRegister(RTC,reg,Data);}    
    
    
    void RegisterCallback(CallbackSource_t CallbackSource, RTCCallback_t* RTCCallback );
    
    void UnRegisterCallback(CallbackSource_t CallbackSource );
    
    virtual void ISR();
    
  private:
    
    RTCCallback_t* m_AlarmACallback;
    RTCCallback_t* m_AlarmBCallback;
    RTCCallback_t* m_WakeUPCallback;
    RTCCallback_t* m_TimeStampCallback;
  };
  
  
  
}


/*

RTC_TAMP1/
RTC_TS/
RTC_OUT/
WKUP2


*/



#endif // RTC_h