/******************
** FILE: RTC.cpp
**
** DESCRIPTION:
**  RTC functionality
**
** CREATED: 10/7/2018, by Amit Chaudhary
******************/

#include"RTC_Module.h"
#include"DateNTime.h"

        
namespace Peripherals
{

RTC_HandleTypeDef RTC_Module::hrtc;


RTC_Module::RTC_Module()
{
    
}
   
Status_t RTC_Module::HwInit()
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef DateToUpdate;

    /* Initialize RTC Only */    
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM; //RTC_OUTPUTSOURCE_SECOND;// 
  
  HAL_PWR_EnableBkUpAccess();
  /* Enable BKP CLK enable for backup registers */
  __HAL_RCC_BKP_CLK_ENABLE();
  
  /* Peripheral clock enable */
  __HAL_RCC_RTC_ENABLE();
  
  //__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
    
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
      return HAL_ERROR;
  }

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
      return HAL_ERROR;
  }


  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
      return HAL_ERROR;
  }

  InitDateAndTime();

  return HAL_OK;

}

void RTC_Module::InitDateAndTime()
{    
    RTC_Date_t RTC_Date;
    RTC_Time_t RTC_Time;
    
    RTC_Time.Seconds = __SECONDS__;//(uint8_t)((BUILD_SEC_CH0 - '0')<<4 | (BUILD_SEC_CH1 - '0'));
    RTC_Time.Minutes = __MINUTES__;//(uint8_t)((BUILD_MIN_CH0 - '0')<<4 | (BUILD_MIN_CH1 - '0'));
    RTC_Time.Hours   = __HOURS__;//(uint8_t)((BUILD_HOUR_CH0 - '0')<<4 | (BUILD_HOUR_CH1 - '0'));
    SetTime( &RTC_Time,RTC_FORMAT_BCD);
    

    RTC_Date.Month = __MONTH__;//(uint8_t)((BUILD_MONTH_CH0 - '0')<<4 | (BUILD_MONTH_CH1 - '0'));
    RTC_Date.Date  = __DAY__;//(uint8_t)((BUILD_DAY_CH0 - '0')<<4 | (BUILD_DAY_CH1 - '0'));
    RTC_Date.Year  = __YEAR__;//(uint8_t)((BUILD_YEAR_CH2 - '0')<<4 | (BUILD_YEAR_CH3 - '0'));
    SetDate( &RTC_Date,RTC_FORMAT_BCD);
}

Status_t RTC_Module::HwDeinit()
{ 
    return HAL_RTC_DeInit(&hrtc);
}
 
Status_t RTC_Module::GetTime( RTC_Time_t* pRTC_Time, uint32_t Format )
{ 
    return HAL_RTC_GetTime(&hrtc, pRTC_Time, Format);
}
    
Status_t RTC_Module::SetTime( RTC_Time_t* pRTC_Time, uint32_t Format )
{ 
    return HAL_RTC_SetTime(&hrtc, pRTC_Time, Format);
} 
    
Status_t RTC_Module::GetDate( RTC_Date_t* pRTC_Date, uint32_t Format )
{ 
    return HAL_RTC_GetDate(&hrtc, pRTC_Date, Format);
}    

Status_t RTC_Module::SetDate( RTC_Date_t* pRTC_Date, uint32_t Format )
{ 
    return HAL_RTC_SetDate(&hrtc, pRTC_Date, Format);
}
 
Status_t RTC_Module::GetTime( char * pStr, uint32_t Format )
{ 
    Status_t Status;
    RTC_Time_t RTC_Time;
    
    if(pStr)
    {
        if ( (Status = HAL_RTC_GetTime(&hrtc, &RTC_Time, Format)) != HAL_OK )
        {
            return Status;
        }
        
        intToStr(RTC_Time.Hours, pStr, 2);
        pStr[2] = ':';
        intToStr(RTC_Time.Minutes, &pStr[3], 2);
        pStr[5] = ':';
        intToStr(RTC_Time.Seconds, &pStr[6], 2);         
    }
    
    return Status;
} 


Status_t RTC_Module::GetDate( char * pStr, uint32_t Format )
{ 
    Status_t Status;
    RTC_Date_t RTC_Date;
    const char Weekdays[][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    const char Months  [][4] = {"   ","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    
    if(pStr)
    {
        if ( (Status = GetDate(&RTC_Date, Format)) != HAL_OK )
        {
            return Status;
        }        
#if 1
        mem_cpy(&pStr[0],Weekdays[RTC_Date.WeekDay],3);
        pStr[3] = ',';
        intToStr(RTC_Date.Date, &pStr[4], 2);
        pStr[6] = ' ';
        mem_cpy(&pStr[7],Months[RTC_Date.Month],3);
        pStr[10] = ' ';
        intToStr(RTC_Date.Year, &pStr[11], 2);
#else
        mem_cpy(&pStr[0],Weekdays[RTC_Date.WeekDay],3);
        pStr[3] = ',';
        mem_cpy(&pStr[4],Months[RTC_Date.Month],3);
        pStr[7] = '-';
        intToStr(RTC_Date.Year, &pStr[8], 2);
#endif
    }
    
    return Status;
} 

uint32_t RTC_Module::GetTimeCounter()
{ 
    uint32_t Hi, Lo;
    Hi = RTC->CNTH & RTC_CNTH_RTC_CNT;
    Lo = RTC->CNTL & RTC_CNTL_RTC_CNT;
    return ( Hi<<16U | Lo );
}

void RTC_Module::SetTimeCounter(uint32_t Curent_Counter)
{ 
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while((RTC->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET);
  SET_BIT(RTC->CRL, RTC_CRL_CNF);

    
    RTC->CNTH  = Curent_Counter>>16;
    RTC->CNTL  = Curent_Counter & 0xFFFFU;
    
    CLEAR_BIT(RTC->CRL, RTC_CRL_CNF);
    while( (RTC->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET );
}

void RTC_Module::DateToStr(RTC_Date_t* Date, char* pStr)
{
    const char Weekdays[][4] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
    const char Months  [][4] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
          //char Year      [2];  
    
    if((pStr == nullptr) || ( Date == nullptr))
        return;
    
    
    
    mem_cpy(&pStr[0],Weekdays[Date->WeekDay],3);
    mem_cpy(&pStr[3],Months[Date->Month],3);
    intToStr(Date->Year, &pStr[6], 2);    
    
}


void RTC_Module::CounterToTime(char* pStr)
{
    uint8_t Hours,Minutes,Seconds;
    static uint32_t Initial_Count = GetTimeCounter();
    uint32_t counter_time = GetTimeCounter() - Initial_Count;
    
    Hours = counter_time / 3600U;
    Minutes  = (uint8_t)((counter_time % 3600U) / 60U);
    Seconds  = (uint8_t)((counter_time % 3600U) % 60U);
    
    intToStr(Hours, pStr, 2);
    pStr[2] = ':';
    intToStr(Minutes, &pStr[3], 2);
    pStr[5] = ':';
    intToStr(Seconds, &pStr[6], 2); 
    pStr[8] = '\0';
    
}
    

    
}