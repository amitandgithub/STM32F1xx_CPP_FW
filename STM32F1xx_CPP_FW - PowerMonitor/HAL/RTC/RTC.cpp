
/******************
** FILE: RTC.cpp
**
** DESCRIPTION:
**  RTC Class Implementation
**
** CREATED: 3/11/2019, by Amit Chaudhary
******************/

#include"RTC.h"

namespace HAL
{ 
  
  static const uint8_t month_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  static const char* weekdays[7] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
  
  Rtc::RTCStatus_t Rtc::HwInit(HAL::ClockManager::RTCClock_t Clock)
  {
    
    LL_RTC_InitTypeDef RTC_InitStruct = {0};
    
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOC);    
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_AFIO);
    
//    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_PWR);  
//    /* Enable BKP CLK enable for backup registers */
//    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_BKP);
//    
//    LL_PWR_EnableBkUpAccess();
    PowerBackupDomain();
    
//    LL_RCC_ForceBackupDomainReset();
//    LL_RCC_ReleaseBackupDomainReset();
    
    if(Clock == HAL::ClockManager::CLOCK_LSE)
    {
      //InterruptManagerInstance.RegisterDeviceInterrupt(RCC_IRQn,2,this);
      LL_RCC_LSE_Disable();
      LL_RCC_LSE_Enable();
      
      /* Wait till LSE is ready */
      while(LL_RCC_LSE_IsReady() != 1)
      {
        
      }
      LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
      RTC_InitStruct.AsynchPrescaler = 0x00007FFFU;
    }
    else
    {     
      //LL_RCC_EnableIT_LSIRDY();
      
      LL_RCC_LSI_Enable();
      
      /* Wait till LSE is ready */
      while(LL_RCC_LSI_IsReady() != 1)
      {
        
      }

      LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
      RTC_InitStruct.AsynchPrescaler = 0x00007FFFU;
    }
    
    LL_RCC_EnableRTC();
    
    /* RTC interrupt Init */  
    InterruptManagerInstance.RegisterDeviceInterrupt(RTC_IRQn,2,this);

    // RTC_InitStruct.OutPutSource = LL_RTC_CALIB_OUTPUT_SECOND;
    
    if(LL_RTC_BKP_GetRegister(BKP, LL_RTC_BKP_DR1) != 0xBABE)
    {        
      if(LL_RTC_Init(RTC, &RTC_InitStruct) == SUCCESS)
      {          
        LL_RTC_EnableWriteProtection(RTC);
        Set(2020,5,23,12,12,12);
        LL_RTC_BKP_SetRegister(BKP,LL_RTC_BKP_DR1,0xBABE);
        return RTC_OK;
      }     
    } 
    return RTC_ERROR;
  } 
  
  Rtc::RTCStatus_t Rtc::GetTime(RtcTime_t* aRtcTime)
  {
    if(aRtcTime != nullptr) 
    {
      CountertoTime(ReadTimeCounter() % 86400,aRtcTime);
      return RTC_OK;
    }
    return RTC_ERROR;
  }
  
  void Rtc::PowerBackupDomain()
  {
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_BKP);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_PWR);
    LL_PWR_EnableBkUpAccess();    
    LL_RTC_DisableWriteProtection(RTC);
  }
  
  void Rtc::CountertoTimeStr(uint32_t counter, char* timeStr)
  {
    RtcTime_t aRtcTime;
    if(timeStr != nullptr)
    {
      CountertoTime(counter,&aRtcTime);
      sprintf(timeStr, "%02d:%02d:%02d", aRtcTime.Hours,aRtcTime.Minutes,aRtcTime.Seconds);
    }
  }
  
  void Rtc::CountertoTime(uint32_t counter, RtcTime_t* aRtcTime)
  {    
    if(aRtcTime != nullptr)
    {           
      aRtcTime->Hours     = counter / 3600U;
      aRtcTime->Minutes   = (uint8_t)((counter % 3600U) / 60U);
      aRtcTime->Seconds   = (uint8_t)((counter % 3600U) % 60U);    
      
      if(aRtcTime->Hours>=24) aRtcTime->Hours = (aRtcTime->Hours % 24U);
    }
  }
  
  Rtc::RTCStatus_t Rtc::GetTime(char* timeStr)
  {
    if(timeStr != nullptr)
    {
      CountertoTimeStr(ReadTimeCounter() % 86400,timeStr);
      return RTC_OK;
    }
    return RTC_ERROR;  
  }
  
  Rtc::RTCStatus_t Rtc::GetDate(RtcDate_t* RtcDate)
  {
    uint32_t temp1 = 0;
    static uint32_t day_count;
    
    uint32_t temp = 0;
    uint32_t counts = 0;
    uint16_t Year =0;
    
    if(RtcDate)
    {      
      counts = ReadTimeCounter();
      temp = (counts / 86400);
      
      if(day_count != temp)
      {
        day_count = temp;
        temp1 = 1970;
        
        while(temp >= 365)
        {
          if(check_for_leap_year(temp1) == 1)
          {
            if(temp >= 366)
            {
              temp -= 366;
            }
            
            else
            {
              break;
            }
          }
          
          else
          {
            temp -= 365;
          }
          
          temp1++;
        };
        
        //cal_year = temp1;
        Year = temp1;
        
        temp1 = 0;
        while(temp >= 28)
        {
          if((temp1 == 1) && (check_for_leap_year(Year) == 1))
          {
            if(temp >= 29)
            {
              temp -= 29;
            }
            
            else
            {
              break;
            }
          }
          
          else
          {
            if(temp >= month_table[temp1])
            {
              temp -= ((uint32_t)month_table[temp1]);
            }
            
            else
            {
              break;
            }
          }
          
          temp1++;
        };
   
        m_RtcDate.Weekday = WeekDayNum(Year, (temp1 + 1), (temp + 1));
        
        m_RtcDate.Day = (temp + 1);        
        m_RtcDate.Month = (temp1 + 1);
        m_RtcDate.Year = Year - 2000;   
        
      }
     
      RtcDate->Day = m_RtcDate.Day; 
      RtcDate->Month = m_RtcDate.Month;
      RtcDate->Year = m_RtcDate.Year;
      return RTC_OK;
    }
    return RTC_ERROR;
  }
  
  Rtc::RTCStatus_t Rtc::GetDate(char* dateStr)
  {
    RtcDate_t RtcDate;
    
    if(dateStr)
    {      
      if( GetDate(&RtcDate) == RTC_OK)
      {
        sprintf(dateStr, "%03s,%02d-%02d-%02d", weekdays[m_RtcDate.Weekday], RtcDate.Day, RtcDate.Month, RtcDate.Year);
        return RTC_OK;
      }
    }
    return RTC_ERROR;
  }
  
  /**
  * @brief  Read the time counter available in RTC_CNT registers.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval Time counter
  */
  uint32_t  Rtc::ReadTimeCounter()
  {
    uint16_t high1 = 0U, high2 = 0U, low = 0U;
    uint32_t timecounter = 0U;
    
    high1 = READ_REG(RTC->CNTH & RTC_CNTH_RTC_CNT);
    low   = READ_REG(RTC->CNTL & RTC_CNTL_RTC_CNT);
    high2 = READ_REG(RTC->CNTH & RTC_CNTH_RTC_CNT);
    
    if (high1 != high2)
    { /* In this case the counter roll over during reading of CNTL and CNTH registers, 
      read again CNTL register then return the counter value */
      timecounter = (((uint32_t) high2 << 16U) | READ_REG(RTC->CNTL & RTC_CNTL_RTC_CNT));
    }
    else
    { /* No counter roll over during reading of CNTL and CNTH registers, counter 
      value is equal to first value of CNTL and CNTH */
      timecounter = (((uint32_t) high1 << 16U) | low);
    }    
    return timecounter;
  }
  
  Rtc::RTCStatus_t Rtc::Set(uint16_t year, uint8_t month, uint8_t date,uint8_t hour, uint8_t minute, uint8_t second)
  {
    uint32_t i = 0;
    uint32_t counts = 0;
    
    if(year > 2099)
    {
      year = 2099;
    }
    
    if(year < 1970)
    {
      year = 1970;
    }
    
    for(i = 1970; i < year; i++)
    {
      if(check_for_leap_year(i) == 1)
      {
        counts += 31622400;
      }
      
      else
      {
        counts += 31536000;
      }
    }
    
    month -= 1;
    
    for(i = 0; i < month; i++)
    {
      counts += (((uint32_t)month_table[i]) * 86400);
    }
    
    if(check_for_leap_year(1970) == 1)
    {
      counts += 86400;
    }
    
    counts += ((uint32_t)(date) * 86400);
    counts += ((uint32_t)hour * 3600);
    counts += ((uint32_t)minute * 60);
    counts += second;
 
    SetCounter(counts);
    
    return RTC_ERROR;
  }
  
  Rtc::RTCStatus_t Rtc::SetCounter(uint32_t counter_time)
  {
    PowerBackupDomain();
    if (LL_RTC_WaitForSynchro(RTC) != ERROR)
    {
      /* Enter Initialization mode */
      if (LL_RTC_EnterInitMode(RTC) != ERROR)
      {        
        LL_RTC_TIME_Set(RTC, counter_time);
        
        /* Exit Initialization mode */
        LL_RTC_ExitInitMode(RTC);
        
        return RTC_OK;
      }      
    }
    return RTC_ERROR;
  }
  
  uint8_t Rtc::WeekDayNum(uint32_t nYear, uint8_t nMonth, uint8_t nDay)
  {
    uint32_t year = 0U, weekday = 0U;
    
    year = 2000U + nYear;
    
    if(nMonth < 3U)
    {
      /*D = { [(23 x month)/9] + day + 4 + year + [(year-1)/4] - [(year-1)/100] + [(year-1)/400] } mod 7*/
      weekday = (((23U * nMonth)/9U) + nDay + 4U + year + ((year-1U)/4U) - ((year-1U)/100U) + ((year-1U)/400U)) % 7U;
    }
    else
    {
      /*D = { [(23 x month)/9] + day + 4 + year + [year/4] - [year/100] + [year/400] - 2 } mod 7*/
      weekday = (((23U * nMonth)/9U) + nDay + 4U + year + (year/4U) - (year/100U) + (year/400U) - 2U ) % 7U; 
    }    
    return (uint8_t)weekday;
  }
  
#if 0  
  void EnableConfiguration()
  {    
    enable_power_control_module(true);
    enable_backup_module(true);
    
    disable_backup_domain_write_protection(true);
    
    set_RTC_configuration_flag(true);
    set_RTC_counter(counts);
    set_RTC_configuration_flag(false);
    
    while(get_RTC_operation_state() == false);
    
    disable_backup_domain_write_protection(false);
  }

  Rtc::RTCStatus_t Rtc::Get(DateAndTime_t* DateAndTime)
  {
    uint32_t temp1 = 0;
    static uint32_t day_count;
    
    uint32_t temp = 0;
    uint32_t counts = 0;
    
    if(DateAndTime)
    {      
      counts = RTC_ReadTimeCounter();
      temp = (counts / 86400);
      
      if(day_count != temp)
      {
        day_count = temp;
        temp1 = 1970;
        
        while(temp >= 365)
        {
          if(check_for_leap_year(temp1) == 1)
          {
            if(temp >= 366)
            {
              temp -= 366;
            }
            
            else
            {
              break;
            }
          }
          
          else
          {
            temp -= 365;
          }
          
          temp1++;
        };
        
        //cal_year = temp1;
        DateAndTime->Year = temp1;
        
        temp1 = 0;
        while(temp >= 28)
        {
          if((temp1 == 1) && (check_for_leap_year(DateAndTime->Year) == 1))
          {
            if(temp >= 29)
            {
              temp -= 29;
            }
            
            else
            {
              break;
            }
          }
          
          else
          {
            if(temp >= month_table[temp1])
            {
              temp -= ((uint32_t)month_table[temp1]);
            }
            
            else
            {
              break;
            }
          }
          
          temp1++;
        };

        DateAndTime->Month = (temp1 + 1);
        DateAndTime->Date = (temp + 1);
      }
      
      temp = (counts % 86400);
      
      DateAndTime->Hours = (temp / 3600);
      DateAndTime->Minutes = ((temp % 3600) / 60);
      DateAndTime->Seconds = ((temp % 3600) % 60);
      
      return RTC_OK;
    }
    return RTC_ERROR;
  }

#endif
  
  uint8_t Rtc::check_for_leap_year(uint16_t year)
  {
    if(year % 4 == 0)
    {
      if(year % 100 == 0)
      {
        if(year % 400 == 0)
        {
          return 1;
        }
        
        else 
        {
          return 0;
        }
      }
      
      else 
      {
        return 1;
      }
    }
    
    else 
    {
      return 0;
    }
  }

  Rtc::RTCStatus_t Rtc::SetAlarm(RtcTime_t* AlarmTime, RTCCallback_t RTCCallback)
  {
    if(AlarmTime == nullptr) return RTC_ERROR;  
    
    return SetAlarm(AlarmTime->Hours, AlarmTime->Minutes, AlarmTime->Seconds, RTCCallback);    
  }
  
  Rtc::RTCStatus_t Rtc::SetAlarm(uint8_t Hours, uint8_t Minutes, uint8_t Seconds, RTCCallback_t RTCCallback)
  {
    uint32_t counter_alarm = 0U, counter_time;
    
    /* Enter Initialization mode */
    if (LL_RTC_EnterInitMode(RTC) != ERROR)
    {
      counter_time = ReadTimeCounter();
      
      counter_alarm = (uint32_t)(((uint32_t)Hours * 3600U) + \
                      ((uint32_t)Minutes * 60U) + \
                      ((uint32_t)Seconds));  
      
      /* Check that requested alarm should expire in the same day (otherwise add 1 day) */
      if (counter_alarm < counter_time)
      {
        /* Add 1 day to alarm counter*/
        counter_alarm += (uint32_t)(24U * 3600U);
      }  
      
      LL_RTC_ALARM_Set(RTC, counter_alarm);
      
      RegisterCallback(RTC_ALARM,RTCCallback);
      
      /* Exit Initialization mode */
      LL_RTC_ExitInitMode(RTC);     
      
    }   
    return RTC_OK;
  }
    
  void Rtc::RegisterCallback(CallbackSource_t CallbackSource, RTCCallback_t RTCCallback )
  {
    if( CallbackSource == RTC_ALARM )
    {
      m_AlarmCallback = RTCCallback;
      /* Clear flag alarm A */
      LL_RTC_ClearFlag_ALR(RTC);
      
      /* Configure the Alarm interrupt */
      LL_RTC_EnableIT_ALR(RTC);
      
      /* RTC Alarm Interrupt Configuration: EXTI configuration */
//      SET_BIT(EXTI->IMR, RTC_EXTI_LINE_ALARM_EVENT);
//      
//      SET_BIT(EXTI->RTSR, RTC_EXTI_LINE_ALARM_EVENT);
    }
    else  if( CallbackSource == RTC_SEC )
    {
      LL_RTC_ClearFlag_SEC(RTC);
      LL_RTC_EnableIT_SEC(RTC);
      
      m_SecondsCallback = RTCCallback;
    }
    else  if( CallbackSource == RTC_COUNTER_OW )
    {
      LL_RTC_ClearFlag_OW(RTC);
      LL_RTC_EnableIT_OW(RTC);
      m_CounterOverflowCallback = RTCCallback;
    }
    else
    {
      //Error
    }    
  }
  
  void Rtc::UnRegisterCallback(CallbackSource_t CallbackSource )
  {
    if( CallbackSource == RTC_ALARM )
    {
      m_AlarmCallback = nullptr;
      
      /* Configure the Alarm interrupt */
      LL_RTC_DisableIT_ALR(RTC);
      
      /* RTC Alarm Interrupt Configuration: EXTI configuration */
      //CLEAR_BIT(EXTI->IMR, RTC_EXTI_LINE_ALARM_EVENT);
      
      //CLEAR_BIT(EXTI->RTSR, RTC_EXTI_LINE_ALARM_EVENT);
    }
    else  if( CallbackSource == RTC_SEC )
    {
      LL_RTC_DisableIT_SEC(RTC);      
      m_SecondsCallback = nullptr;
    }
    else  if( CallbackSource == RTC_COUNTER_OW )
    {
      LL_RTC_DisableIT_OW(RTC);
      m_CounterOverflowCallback = nullptr;
    }
    else
    {
      //Error
    }    
  }
  
  void Rtc::ISR()
  {
    if( LL_RTC_IsActiveFlag_ALR(RTC) )
    {
      LL_RTC_ClearFlag_ALR(RTC);
      if(m_AlarmCallback) m_AlarmCallback->CallbackFunction();
    }
    
    if( LL_RTC_IsActiveFlag_SEC(RTC) )
    {
      LL_RTC_ClearFlag_SEC(RTC);
      if(m_SecondsCallback) m_SecondsCallback->CallbackFunction();
    }
    
    if( LL_RTC_IsActiveFlag_OW(RTC) )
    {
      LL_RTC_ClearFlag_OW(RTC);
      if(m_CounterOverflowCallback) m_CounterOverflowCallback->CallbackFunction();
    }
    
//    if(LL_RCC_IsActiveFlag_LSERDY())
//    {
//      LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
//      LL_RCC_ClearFlag_LSERDY();
//      LL_RCC_EnableRTC();
//    }
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
}