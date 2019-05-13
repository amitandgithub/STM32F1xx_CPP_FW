/******************
** CLASS: RTC_Module
**
** DESCRIPTION:
**  RTC Functionality
**
** CREATED: 10/7/2018, by Amit Chaudhary
**
** FILE: RTC_Module.h
**
******************/
#ifndef RTC_Module_h
#define RTC_Module_h

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_rtc.h"
#include "Peripheral.hpp"
#include "Utils.h"
#include <ctime>

namespace Peripherals
{
    
class RTC_Module : public Peripheral
{
public:
    static const char DATE_SPACER = ' ';
    typedef RTC_TimeTypeDef RTC_Time_t;
    typedef RTC_DateTypeDef RTC_Date_t;
    
    typedef struct 
    {
        char Weekday[3] ;
        char Month[3] ;
        char Year[2] ;
    }DateStr_t;
    
    RTC_Module();
    
    virtual ~RTC_Module(){};
    
    virtual       Status_t       HwInit             ();
    
    virtual       Status_t       HwDeinit           ();
    
                  Status_t       GetTime            (RTC_Time_t* pRTC_Time, uint32_t Format = RTC_FORMAT_BIN);
                  
                  uint32_t       GetTimeCounter      ();                  
                  
                  Status_t       GetTime            (char *pStr, uint32_t Format = RTC_FORMAT_BIN);
                  
                  Status_t       SetTime            (RTC_Time_t* pRTC_Time, uint32_t Format = RTC_FORMAT_BIN);
    
                  Status_t       GetDate            (RTC_Date_t* pRTC_Date, uint32_t Format = RTC_FORMAT_BIN);
                  
                  Status_t       GetDate            (char * pStr, uint32_t Format = RTC_FORMAT_BIN);

                  Status_t       SetDate            (RTC_Date_t* pRTC_Date, uint32_t Format = RTC_FORMAT_BIN);
                  
                  void           DateToStr          (RTC_Date_t* Date, char* pStr);
                  
                  void           SetTimeCounter     (uint32_t Curent_Counter);
                  
                  void           CounterToTime      (char* pStr);
                  
                  void           InitDateAndTime();
private:
    static RTC_HandleTypeDef hrtc;
};

}
#endif //RTC_Module_h
