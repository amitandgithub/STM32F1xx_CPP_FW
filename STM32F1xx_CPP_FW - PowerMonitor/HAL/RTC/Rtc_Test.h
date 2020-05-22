

#include"RTC.h"

using namespace HAL;


//Rtc::RtcTime_t myTime;





char myTime[10];

class RTCSecCallback : public Callback
{  
    static uint32_t RtcSecCount;
    
    void CallbackFunction()
    {       
      rtc.GetTime(myTime);
      RtcSecCount++;
    }
};

uint32_t RTCSecCallback::RtcSecCount;


RTCSecCallback RTCSecCallbackObj;



void Rtc_test()
{  
  
  static bool InitDone;
  
  if(InitDone == false)
  {
    rtc.HwInit(HAL::ClockManager::CLOCK_LSE);
    rtc.Set(2020,5,21,12,0,0);
    //rtc.SetTime(0,0,0);
    //rtc.SetTime(__HOURS__,__MINUTES__,__SECONDS__);
   // rtc.SetAlarm(0,1,0,&RTCSecCallbackObj);
   // rtc.RegisterCallback(Rtc::RTC_SEC,&RTCSecCallbackObj); 
    rtc.RegisterCallback(Rtc::RTC_SEC,&RTCSecCallbackObj);
    InitDone = true;
  }
  
  
  while(1)
  {
   // rtc.GetTime(myTime);
   // printf("%s",myTime);
    //HAL_Delay(100);
    return;
  }
  
}