
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
  
  Rtc::RTCStatus_t Rtc::HwInit(RtcTime_t* RtcTime, RtcDate_t* RtcDate)
  {
    ErrorStatus RTCStatus = ERROR; 
      
    LL_RTC_InitTypeDef RTC_InitStruct = {0};
    LL_RTC_TimeTypeDef RTC_TimeStruct = {0};
    LL_RTC_DateTypeDef RTC_DateStruct = {0};
    RtcTime_t* pRtcTime;
    RtcDate_t* pRtcDate;
    
     if(RtcTime == nullptr)
     {
       RTC_TimeStruct.Hours = 0;
       RTC_TimeStruct.Minutes = 0;
       RTC_TimeStruct.Seconds = 0;
       pRtcTime = &RTC_TimeStruct;
     }
     else
     {
       pRtcTime = RtcTime;
     }
     
     if(RtcDate == nullptr)
     {
       RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_MONDAY;
       RTC_DateStruct.Month = LL_RTC_MONTH_JANUARY;
       RTC_DateStruct.Year = 0;
       pRtcDate = &RTC_DateStruct;
     }
     else
     {
       pRtcDate = RtcDate;
     }
    
    /* Peripheral clock enable */
    LL_RCC_EnableRTC();

    /** Initialize RTC and set the Time and Date 
    */
    RTC_InitStruct.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
    RTC_InitStruct.AsynchPrescaler = 127;
    RTC_InitStruct.SynchPrescaler = 255;
    RTCStatus = LL_RTC_Init(RTC, &RTC_InitStruct);
    
    if(RTCStatus == ERROR ) return RTC_ERROR;
    
    /** Initialize RTC and set the Time and Date 
    */
    if(LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) != 0x32F2)
    {        
     if( (LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BCD, pRtcTime) == ERROR) || (LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BCD, pRtcDate) == ERROR) )
      return RTC_ERROR;

      LL_RTC_BAK_SetRegister(RTC,LL_RTC_BKP_DR0,0x32F2);
    } 
    
    InterruptManagerInstance.RegisterDeviceInterrupt(RTC_Alarm_IRQn,0,this); 
    InterruptManagerInstance.RegisterDeviceInterrupt(RTC_WKUP_IRQn,0,this); 
    InterruptManagerInstance.RegisterDeviceInterrupt(TAMPER_STAMP_IRQn,0,this); 

    
    return RTC_OK;
  } 
  
  void Rtc::Time2Str(uint32_t TimeCounter, char* timeStr)
  {
    uint8_t Hours,Minutes,Seconds;
    
    if(timeStr) 
    {      
      Hours   = (uint8_t)((TimeCounter & (RTC_TR_HT | RTC_TR_HU)) >> 16U);
      Minutes = (uint8_t)((TimeCounter & (RTC_TR_MNT | RTC_TR_MNU)) >> 8U);
      Seconds = (uint8_t)(TimeCounter & (RTC_TR_ST | RTC_TR_SU));
      
      sprintf(timeStr, "%02x:%02x:%02x", Hours, Minutes, Seconds);
    }
  }  
  
  void Rtc::Date2Str(uint32_t DateCounter, char* dateStr)
  {
    //DateCounter = 0xWWDDMMYY
    char MonthArray[][4] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    char WeekDayArray[][4] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
    
    if(dateStr)
    {
      sprintf(dateStr,"%s %x-%s-%x", WeekDayArray[DateCounter>>24], (DateCounter>>16)&0xff,MonthArray[ ((DateCounter>>8)&0xff) - 1],(DateCounter)&0xff); 
    }
  }  
  
  Rtc::RTCStatus_t Rtc::SetTime(RtcTime_t* aRtcTime)
  {
    if(aRtcTime == nullptr) return RTC_ERROR;
    
    return SetTime(aRtcTime->Hours, aRtcTime->Minutes, aRtcTime->Seconds);
  }
  
  Rtc::RTCStatus_t Rtc::SetTime(uint8_t Hours, uint8_t Minutes, uint8_t Seconds)
  {
    ErrorStatus status = ERROR;
    
     /* Disable the write protection for RTC registers */
      LL_RTC_DisableWriteProtection(RTC);
      
    /* Enter Initialization mode */
    if (LL_RTC_EnterInitMode(RTC) != ERROR)
    {
      
      LL_RTC_TIME_Config(RTC, LL_RTC_TIME_FORMAT_AM_OR_24, Hours, Minutes, Seconds);      
      
      /* Exit Initialization mode */
      LL_RTC_ExitInitMode(RTC);
      
      status = SUCCESS;
    }    
    LL_RTC_EnableWriteProtection(RTC);
    return status==ERROR ? RTC_ERROR : RTC_OK;
  }
  
  void Rtc::GetTime(RtcTime_t* aRtcTime)
  {    
    if(aRtcTime)
    {      
      /* Read the time counter*/
      uint32_t counter_time = GetTime_00HHMMSS();
      
      /* Fill the structure fields with the read parameters */
      aRtcTime->Hours     = (uint8_t)((counter_time & (RTC_TR_HT | RTC_TR_HU)) >> 16U);
      aRtcTime->Minutes   = (uint8_t)((counter_time & (RTC_TR_MNT | RTC_TR_MNU)) >> 8U);
      aRtcTime->Seconds   = (uint8_t)(counter_time & (RTC_TR_ST | RTC_TR_SU));      
    }
  }

  void Rtc::GetTime(char* timeStr)
  {   
    Time2Str(GetTime_00HHMMSS(), timeStr);    
  }
  
  uint32_t  Rtc::GetDate(char* dateStr)
  {
    Date2Str(GetDate_WWDDMMYY(), dateStr);
    return 1;
  }  
  
  Rtc::RTCStatus_t Rtc::SetAlarmA(RtcAlarm_t* RtcAlarm, RTCCallback_t* RTCCallback)
  {
    if(RtcAlarm == nullptr) return RTC_ERROR;
    
    /* Disable the write protection for RTC registers */
    LL_RTC_DisableWriteProtection(RTC);
    
    /* Select weekday selection */
    if (RtcAlarm->AlarmDateWeekDaySel == LL_RTC_ALMA_DATEWEEKDAYSEL_DATE)
    {
      /* Set the date for ALARM */
      LL_RTC_ALMA_DisableWeekday(RTC);
      LL_RTC_ALMA_SetDay(RTC, RtcAlarm->AlarmDateWeekDay);
    }
    else
    {
      /* Set the week day for ALARM */
      LL_RTC_ALMA_EnableWeekday(RTC);
      LL_RTC_ALMA_SetWeekDay(RTC, RtcAlarm->AlarmDateWeekDay);
    }
    
    /* Configure the Alarm register */
      LL_RTC_ALMA_ConfigTime(RTC, RtcAlarm->AlarmTime.TimeFormat, RtcAlarm->AlarmTime.Hours,
                             RtcAlarm->AlarmTime.Minutes, RtcAlarm->AlarmTime.Seconds);
    /* Set ALARM mask */
    LL_RTC_ALMA_SetMask(RTC, RtcAlarm->AlarmMask);
    
    EnableAlarmA();

    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    
    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTC);
    
    m_AlarmACallback = RTCCallback;    
    
    return RTC_OK;
  }
  
  Rtc::RTCStatus_t Rtc::SetAlarmB(RtcAlarm_t* RtcAlarm, RTCCallback_t* RTCCallback)
  {
    if(RtcAlarm == nullptr) return RTC_ERROR;
    
    /* Disable the write protection for RTC registers */
    LL_RTC_DisableWriteProtection(RTC);
    
    /* Select weekday selection */
    if (RtcAlarm->AlarmDateWeekDaySel == LL_RTC_ALMA_DATEWEEKDAYSEL_DATE)
    {
      /* Set the date for ALARM */
      LL_RTC_ALMB_DisableWeekday(RTC);
      LL_RTC_ALMB_SetDay(RTC, RtcAlarm->AlarmDateWeekDay);
    }
    else
    {
      /* Set the week day for ALARM */
      LL_RTC_ALMB_EnableWeekday(RTC);
      LL_RTC_ALMB_SetWeekDay(RTC, RtcAlarm->AlarmDateWeekDay);
    }
    
    /* Configure the Alarm register */
      LL_RTC_ALMB_ConfigTime(RTC, RtcAlarm->AlarmTime.TimeFormat, RtcAlarm->AlarmTime.Hours,
                             RtcAlarm->AlarmTime.Minutes, RtcAlarm->AlarmTime.Seconds);
    /* Set ALARM mask */
    LL_RTC_ALMB_SetMask(RTC, RtcAlarm->AlarmMask);
    
    EnableAlarmB();
    
     // EXTI->IMR |= (uint32_t)EXTI_IMR_MR17;
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    
    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTC);
    
    m_AlarmBCallback = RTCCallback;   
    
    return RTC_OK;
  }
  
  Rtc::RTCStatus_t Rtc::SetMinuteAlarm(char AlarmA_B, RTCCallback_t* RTCCallback)
  {    
    /* Disable the write protection for RTC registers */
    LL_RTC_DisableWriteProtection(RTC);
    
    if( (AlarmA_B == 'A') || (AlarmA_B == 'a') )
    {      
      /* Configure the Alarm register */
      LL_RTC_ALMA_ConfigTime(RTC, LL_RTC_ALMA_TIME_FORMAT_AM,0,0,1);
      /* Set ALARM mask */
      LL_RTC_ALMA_SetMask(RTC,LL_RTC_ALMA_MASK_DATEWEEKDAY | LL_RTC_ALMA_MASK_HOURS | LL_RTC_ALMA_MASK_MINUTES);      
      
      EnableAlarmA();
      
      m_AlarmACallback = RTCCallback;         
    }
    else if( (AlarmA_B == 'B') || (AlarmA_B == 'b') )
    {      
      /* Configure the Alarm register */
      LL_RTC_ALMB_ConfigTime(RTC, LL_RTC_ALMA_TIME_FORMAT_AM,0,0,1);
      /* Set ALARM mask */
      LL_RTC_ALMB_SetMask(RTC,LL_RTC_ALMA_MASK_DATEWEEKDAY | LL_RTC_ALMA_MASK_HOURS | LL_RTC_ALMA_MASK_MINUTES);      
      
      EnableAlarmB();
      
      m_AlarmBCallback = RTCCallback;         
    }
    
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    
    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTC);     
    
    return RTC_OK;
  }
  
  
  Rtc::RTCStatus_t Rtc::SetHourlyAlarm(char AlarmA_B, RTCCallback_t* RTCCallback)
  {    
    /* Disable the write protection for RTC registers */
    LL_RTC_DisableWriteProtection(RTC);
    
    if( (AlarmA_B == 'A') || (AlarmA_B == 'a') )
    {      
      /* Configure the Alarm register */
      LL_RTC_ALMA_ConfigTime(RTC, LL_RTC_ALMA_TIME_FORMAT_AM,0,1,1);
      /* Set ALARM mask */
      LL_RTC_ALMA_SetMask(RTC,LL_RTC_ALMA_MASK_DATEWEEKDAY | LL_RTC_ALMA_MASK_HOURS);      
      
      EnableAlarmA();
      
      m_AlarmACallback = RTCCallback;         
    }
    else if( (AlarmA_B == 'B') || (AlarmA_B == 'b') )
    {      
      /* Configure the Alarm register */
      LL_RTC_ALMB_ConfigTime(RTC, LL_RTC_ALMA_TIME_FORMAT_AM,0,1,1);
      /* Set ALARM mask */
      LL_RTC_ALMB_SetMask(RTC,LL_RTC_ALMA_MASK_DATEWEEKDAY | LL_RTC_ALMA_MASK_HOURS);    
      
      EnableAlarmB();
      
      m_AlarmBCallback = RTCCallback;         
    }
    
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    
    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTC);     
    
    return RTC_OK;
  }
  
    
  Rtc::RTCStatus_t Rtc::SetDailyAlarm(char AlarmA_B, uint8_t Hour, uint8_t Minutes, uint8_t Seconds, RTCCallback_t* RTCCallback)
  {    
    /* Disable the write protection for RTC registers */
    LL_RTC_DisableWriteProtection(RTC);
    
    if( (AlarmA_B == 'A') || (AlarmA_B == 'a') )
    {      
      /* Configure the Alarm register */
      LL_RTC_ALMA_ConfigTime(RTC, LL_RTC_ALMA_TIME_FORMAT_AM,Hour,Minutes,Seconds);
      /* Set ALARM mask */
      LL_RTC_ALMA_SetMask(RTC,LL_RTC_ALMA_MASK_DATEWEEKDAY);      
      
      EnableAlarmA();
      
      m_AlarmACallback = RTCCallback;         
    }
    else if( (AlarmA_B == 'B') || (AlarmA_B == 'b') )
    {      
      /* Configure the Alarm register */
      LL_RTC_ALMB_ConfigTime(RTC, LL_RTC_ALMA_TIME_FORMAT_AM,Hour,Minutes,Seconds);
      /* Set ALARM mask */
      LL_RTC_ALMB_SetMask(RTC,LL_RTC_ALMA_MASK_DATEWEEKDAY);      
      
      EnableAlarmB();
      
      m_AlarmBCallback = RTCCallback;         
    }
    
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    
    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTC);     
    
    return RTC_OK;
  }
  
    Rtc::RTCStatus_t Rtc::SetWeeklyAlarm(char AlarmA_B, uint8_t DayOfWeek, uint8_t Hour, uint8_t Minutes, uint8_t Seconds, RTCCallback_t* RTCCallback)
  {    
    /* Disable the write protection for RTC registers */
    LL_RTC_DisableWriteProtection(RTC);
    
    if( (AlarmA_B == 'A') || (AlarmA_B == 'a') )
    {      
     /* Set the week day for ALARM */
      LL_RTC_ALMA_EnableWeekday(RTC);
      LL_RTC_ALMA_SetWeekDay(RTC,DayOfWeek);
      
      /* Configure the Alarm register */
      LL_RTC_ALMA_ConfigTime(RTC, LL_RTC_ALMA_TIME_FORMAT_AM,Hour,Minutes,Seconds);
      /* Set ALARM mask */
      LL_RTC_ALMA_SetMask(RTC,LL_RTC_ALMA_MASK_NONE);      
      
      EnableAlarmA();
      
      m_AlarmACallback = RTCCallback;         
    }
    else if( (AlarmA_B == 'B') || (AlarmA_B == 'b') )
    {      
     /* Set the week day for ALARM */
      LL_RTC_ALMB_EnableWeekday(RTC);
      LL_RTC_ALMB_SetWeekDay(RTC,DayOfWeek);
      
      /* Configure the Alarm register */
      LL_RTC_ALMB_ConfigTime(RTC, LL_RTC_ALMA_TIME_FORMAT_AM,Hour,Minutes,Seconds);
      /* Set ALARM mask */
      LL_RTC_ALMB_SetMask(RTC,LL_RTC_ALMA_MASK_NONE);      
      
      EnableAlarmB();
      
      m_AlarmBCallback = RTCCallback;         
    }
    
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    
    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTC);     
    
    return RTC_OK;
  }
  
  Rtc::RTCStatus_t Rtc::SetMonthlyAlarm(char AlarmA_B, uint8_t DayOfMonth, uint8_t Hour, uint8_t Minutes, uint8_t Seconds, RTCCallback_t* RTCCallback)
  {    
    /* Disable the write protection for RTC registers */
    LL_RTC_DisableWriteProtection(RTC);
    
    if( (AlarmA_B == 'A') || (AlarmA_B == 'a') )
    {      
      /* Set the date for ALARM */
      LL_RTC_ALMA_DisableWeekday(RTC);
      LL_RTC_ALMA_SetDay(RTC,DayOfMonth);
      
      /* Configure the Alarm register */
      LL_RTC_ALMA_ConfigTime(RTC, LL_RTC_ALMA_TIME_FORMAT_AM,Hour,Minutes,Seconds);
      /* Set ALARM mask */
      LL_RTC_ALMA_SetMask(RTC,LL_RTC_ALMA_MASK_NONE);      
      
      EnableAlarmA();
      
      m_AlarmACallback = RTCCallback;         
    }
    else if( (AlarmA_B == 'B') || (AlarmA_B == 'b') )
    {      
      /* Set the date for ALARM */
      LL_RTC_ALMB_DisableWeekday(RTC);
      LL_RTC_ALMB_SetDay(RTC, DayOfMonth);
      
      /* Configure the Alarm register */
      LL_RTC_ALMB_ConfigTime(RTC, LL_RTC_ALMA_TIME_FORMAT_AM,Hour,Minutes,Seconds);
      /* Set ALARM mask */
      LL_RTC_ALMB_SetMask(RTC,LL_RTC_ALMA_MASK_NONE);      
      
      EnableAlarmB();
      
      m_AlarmBCallback = RTCCallback;         
    }
    
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    
    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTC);     
    
    return RTC_OK;
  }
  
  void Rtc::SetWakeUpTimer(WakeUpTimerRange_t WakeUpTimerRange, uint32_t Time, RTCCallback_t* RTCCallback)
  {
    /* Disable the write protection for RTC registers */
    LL_RTC_DisableWriteProtection(RTC);
    
    LL_RTC_WAKEUP_Disable(RTC);
    
    if(WakeUpTimerRange == RTC_122_US_TO_32_SEC)
    {
      LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_DIV_16);
    }
    else if(WakeUpTimerRange == RTC_1_SEC_TO_18_HOURS)
    {
      Time--;
      LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);
    }
    else if(WakeUpTimerRange == RTC_18_HOURS_TO_36_HOURS)
    {
      LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE_WUT);
    }
    
    LL_RTC_WAKEUP_SetAutoReload(RTC,Time );
      
    EnableWakeUpTimer();
    
    m_WakeUPCallback = RTCCallback;  
    
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_20);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_20);   
    
    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTC);      
  }
  
  void Rtc::SetTimestampEvent(Rtc_Edge_t Rtc_Edge, RTCCallback_t* RTCCallback)
  {
    /* Disable the write protection for RTC registers */
    LL_RTC_DisableWriteProtection(RTC);
    
    LL_RTC_TS_SetActiveEdge(RTC,Rtc_Edge);
    
    EnableTimeStamp();
    
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_19);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_19); 
    
    EnableRTCOutput();
      
    m_TimeStampCallback = RTCCallback;    
    
    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTC);   
  }
  
  void Rtc::GetTimestampTime(char* timeStr)
  {    
    if(timeStr)
    {
      Time2Str(GetTimestampTime(),timeStr);
    }
  }
  
  void Rtc::GetTimestampDate(char* timeStr)
  {    
    if(timeStr)
    {
      Date2Str(GetTimestampDate(),timeStr);
    }
  }  
  
  void Rtc::RegisterCallback(CallbackSource_t CallbackSource, RTCCallback_t* RTCCallback )
  {
    if( CallbackSource == RTC_ALARM_A )
    {
      m_AlarmACallback = RTCCallback;      
    }
    else  if( CallbackSource == RTC_ALARM_B )
    {      
      m_AlarmBCallback = RTCCallback;
    }
    else  if( CallbackSource == RTC_WakewUP )
    {
      m_WakeUPCallback = RTCCallback;
    }
    else  if( CallbackSource == RTC_TimeStamp )
    {
      m_TimeStampCallback = RTCCallback;
    }   
  }
  
  void Rtc::UnRegisterCallback(CallbackSource_t CallbackSource )
  {
    /* Disable the write protection for RTC registers */
    LL_RTC_DisableWriteProtection(RTC);
    
    if( CallbackSource == RTC_ALARM_A )
    {
      m_AlarmACallback = nullptr;
      DisableAlarmA();
    }
    else  if( CallbackSource == RTC_ALARM_B )
    {      
      m_AlarmBCallback = nullptr;
      DisableAlarmB();
    }
    else  if( CallbackSource == RTC_WakewUP )
    {
      m_WakeUPCallback = nullptr;
      DisableWakeUpTimer();
    }
    else  if( CallbackSource == RTC_TimeStamp )
    {
      m_TimeStampCallback = nullptr;
      DisableTimeStamp();
    }   
    
    /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTC);
  }
  
  void Rtc::EnableRTCOutput()
  {   
    DigitalIO::SetOutputMode(PORTC,13,OUTPUT_AF_PUSH_PULL,LL_GPIO_SPEED_FREQ_MEDIUM);
  }
  
  void Rtc:: EnableAlarmOutputEvent(AlarmOut_t AlarmOut, AlarmOutputType_t AlarmOutputType, bool ActiveLow)
  {
    /* Disable the write protection for RTC registers */
    LL_RTC_DisableWriteProtection(RTC);
    
    LL_RTC_SetAlarmOutEvent(RTC,AlarmOut);  
    LL_RTC_SetAlarmOutputType(RTC,AlarmOutputType);
    if(ActiveLow == true) LL_RTC_SetOutputPolarity(RTC,LL_RTC_OUTPUTPOLARITY_PIN_LOW);
    else                  LL_RTC_SetOutputPolarity(RTC,LL_RTC_OUTPUTPOLARITY_PIN_HIGH);
    
     /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTC);
    
    EnableRTCOutput();
    
  }
  
  // 9467405917 - Gadwali
  void Rtc::ISR()
  {
    uint32_t ISR = RTC->ISR;
    
    if(ISR & RTC_ISR_ALRAF)
    {
      LL_RTC_ClearFlag_ALRA(RTC);
      if(m_AlarmACallback) m_AlarmACallback->CallbackFunction();
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);
    }
    
    if( ISR & RTC_ISR_ALRBF)
    {
      LL_RTC_ClearFlag_ALRB(RTC);
      if(m_AlarmBCallback) m_AlarmBCallback->CallbackFunction();
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);
    }
    
    if( ISR & RTC_ISR_WUTF )
    {
      LL_RTC_ClearFlag_WUT(RTC);
      if(m_WakeUPCallback) m_WakeUPCallback->CallbackFunction();
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_20);
    }
    
    if( ISR & RTC_ISR_TSF )
    {           
      if(m_TimeStampCallback) m_TimeStampCallback->CallbackFunction();
      LL_RTC_ClearFlag_TS(RTC); 
      if(RTC->ISR & RTC_ISR_TSOVF) LL_RTC_ClearFlag_TSOV(RTC);
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_19);
    }
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
}