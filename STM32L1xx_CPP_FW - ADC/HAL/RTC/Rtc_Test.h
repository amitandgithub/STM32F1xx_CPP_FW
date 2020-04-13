

#include"RTC.h"

using namespace HAL;


//Rtc::RtcTime_t myTime;
extern GpioOutput LED;




char myTime[40];
static uint32_t Idx,RtcSecCount;

class RTCSecCallback : public Callback
{      
    void CallbackFunction()
    {       
      rtc.GetTime(myTime);
      rtc.GetDate(&myTime[8]);      
      RtcSecCount++;
    }
};

class RTCTimestampCallback : public Callback
{  
    void CallbackFunction()
    {       
      //rtc.WriteBackupReg(Idx++,rtc.GetTimestampTime());
      rtc.GetTimestampTime(&myTime[21]);
      rtc.GetTimestampDate(&myTime[31]);
      //LED.Toggle();
    }
};

RTCSecCallback RTCSecCallbackObj;
RTCTimestampCallback RTCTimestampCallbackObj;



void Rtc_test()
{  
  
  static bool InitDone;
  LL_RTC_TimeTypeDef RTC_TimeStruct;
  LL_RTC_DateTypeDef RTC_DateStruct;
    
  if(InitDone == false)
  {
    RTC_TimeStruct.Hours        = __HOURS__; 
    RTC_TimeStruct.Minutes      = __MINUTES__;
    RTC_TimeStruct.Seconds      = __SECONDS__;
       
    RTC_DateStruct.WeekDay      = 5;
    RTC_DateStruct.Day          = __DAY__;
    RTC_DateStruct.Month        = __MONTH__;
    RTC_DateStruct.Year         = __YEAR__;
    
    rtc.HwInit(&RTC_TimeStruct,&RTC_DateStruct);
    //rtc.SetAlarmA(&RtcAlarm,&RTCSecCallbackObj);
    //rtc.SetMinuteAlarm('A',&RTCSecCallbackObj); 
    //rtc.SetHourlyAlarm('A',&RTCSecCallbackObj); 
    // rtc.SetDailyAlarm('A',0x20,0x01,0x03,&RTCSecCallbackObj); 
    // rtc.SetMonthlyAlarm('b',0x31,0x20,0x01,0x03,&RTCSecCallbackObj);
    // rtc.SetWeeklyAlarm('b',0x01,0x20,0x01,0x03,&RTCSecCallbackObj);
   // rtc.EnableAlarmOutputEvent(Rtc::WAKEUP_EVENT, Rtc::PUSH_PULL);
    rtc.SetWakeUpTimer(Rtc::RTC_122_US_TO_32_SEC,0xFFFF>>5,&RTCSecCallbackObj);
    
    //rtc.SetTimestampEvent(Rtc::RTC_TIMESTAMP_ON_RISING,&RTCTimestampCallbackObj);
    //rtc.EnableRTCOutput();

    InitDone = true;
  }
  
  
  while(1)
  {
    //TimeC = LL_RTC_TIME_Get(RTC);
//    rtc.GetTime(myTime);
//    printf("%s",myTime);
//    LL_mDelay(1000);
    return;
  }
  
}