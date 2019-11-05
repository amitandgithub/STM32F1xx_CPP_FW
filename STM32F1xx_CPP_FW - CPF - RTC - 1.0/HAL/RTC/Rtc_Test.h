

#include"RTC.h"

using namespace HAL;


//Rtc::RtcTime_t myTime;






class RTCSecCallback : public Callback
{  
    static uint32_t RtcSecCount;
    
    void CallbackFunction()
    {
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
    rtc.HwInit();
    rtc.SetTime(21,15,0);
   // rtc.SetAlarm(0,1,0,&RTCSecCallbackObj);
    rtc.RegisterCallback(Rtc::RTC_SEC,&RTCSecCallbackObj);
    InitDone = true;
  }
  
  
  while(1)
  {
    //rtc.GetTime(&myTime);
    //HAL_Delay(100);
    return;
  }
  
}