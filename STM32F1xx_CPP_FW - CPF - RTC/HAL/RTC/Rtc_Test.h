

#include"RTC.h"

using namespace HAL;


//Rtc::RtcTime_t myTime;

void Rtc_test()
{  
  
  static bool InitDone;
  
  if(InitDone == false)
  {
    rtc.HwInit();
    InitDone = true;
  }
  
  
  while(1)
  {
    //rtc.GetTime(&myTime);
    //HAL_Delay(100);
    return;
  }
  
}