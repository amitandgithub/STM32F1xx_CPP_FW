
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
  
  Rtc::RTCStatus_t Rtc::HwInit(void *pInitStruct)
  {
    ErrorStatus status = ERROR;
    
    LL_RTC_InitTypeDef RTC_InitStruct = {0};
    LL_RTC_TimeTypeDef RTC_TimeStruct = {0};
    
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    
    LL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_BKP);
    /* Peripheral clock enable */
    LL_RCC_EnableRTC();
    
    /* RTC interrupt Init */
    NVIC_SetPriority(RTC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
    NVIC_EnableIRQ(RTC_IRQn);
    
    /* USER CODE BEGIN RTC_Init 1 */
    
    /* USER CODE END RTC_Init 1 */
    /** Initialize RTC and set the Time and Date 
    */
    RTC_InitStruct.AsynchPrescaler = 0x00007FFFU;
    // RTC_InitStruct.OutPutSource = LL_RTC_CALIB_OUTPUT_SECOND;
    LL_RTC_Init(RTC, &RTC_InitStruct);
    LL_RTC_SetAsynchPrescaler(RTC, 0x00007FFFU);
    /** Initialize RTC and set the Time and Date 
    */
    RTC_TimeStruct.Hours = 0;
    RTC_TimeStruct.Minutes = 0;
    RTC_TimeStruct.Seconds = 0;
    LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_TimeStruct);
    //    
    return status==ERROR ? RTC_ERROR : RTC_OK;
  } 
  
  Rtc::RTCStatus_t Rtc::SetTime(RtcTime_t* aRtcTime)
  {
    if(aRtcTime == nullptr) return RTC_ERROR;
    
    return SetTime(aRtcTime->Hours, aRtcTime->Minutes, aRtcTime->Seconds);
  }
  
  Rtc::RTCStatus_t Rtc::SetTime(uint8_t Hours, uint8_t Minutes, uint8_t Seconds)
  {
    ErrorStatus status = ERROR;
    uint32_t counter_time = 0U;
    
    /* Enter Initialization mode */
    if (LL_RTC_EnterInitMode(RTC) != ERROR)
    {
      /* Check the input parameters format */
      counter_time = (uint32_t)(((uint32_t)Hours * 3600U) + \
        ((uint32_t)Minutes * 60U) + \
          ((uint32_t)Seconds));
      
      LL_RTC_TIME_Set(RTC, counter_time);
      
      /* Exit Initialization mode */
      LL_RTC_ExitInitMode(RTC);
      
      status = SUCCESS;
    }
    
    
    return status==ERROR ? RTC_ERROR : RTC_OK;
  }
  
  Rtc::RTCStatus_t Rtc::GetTime(RtcTime_t* aRtcTime)
  {
    ErrorStatus status = ERROR;
    uint32_t counter_time = 0U;
    
    if(aRtcTime == nullptr) return RTC_ERROR;    
    
    /* Read the time counter*/
    counter_time = RTC_ReadTimeCounter();
    
    /* Fill the structure fields with the read parameters */
    aRtcTime->Hours = counter_time / 3600U;
    aRtcTime->Minutes  = (uint8_t)((counter_time % 3600U) / 60U);
    aRtcTime->Seconds  = (uint8_t)((counter_time % 3600U) % 60U);
    
    return status==ERROR ? RTC_ERROR : RTC_OK;
  }
  
  Rtc::RTCStatus_t Rtc::GetTime(char* timeStr)
  {
    uint8_t Hours,Minutes,Seconds;
    
    ErrorStatus status = ERROR;
    uint32_t counter_time = 0U;
    
    if(timeStr == nullptr) return RTC_ERROR; 
    
    /* Read the time counter*/
    counter_time = RTC_ReadTimeCounter();
    
    /* Fill the structure fields with the read parameters */
    Hours    = counter_time / 3600U;
    Minutes  = (uint8_t)((counter_time % 3600U) / 60U);
    Seconds  = (uint8_t)((counter_time % 3600U) % 60U);
    
    intToStr(Hours, &timeStr[0],2);
    timeStr[2] = ':';
    intToStr(Minutes, &timeStr[3],2);
    timeStr[5] = ':';
    intToStr(Seconds, &timeStr[6],2);
    
    return status==ERROR ? RTC_ERROR : RTC_OK;
  }
  
  /**
  * @brief  Read the time counter available in RTC_CNT registers.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval Time counter
  */
  uint32_t  Rtc::RTC_ReadTimeCounter()
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
  
  
  Rtc::RTCStatus_t Rtc::SetAlarm(RtcTime_t* AlarmTime, RTCCallback_t RTCCallback)
  {
    uint32_t counter_alarm = 0U, counter_time;
    
    if(AlarmTime == nullptr) return RTC_ERROR;    
    
    /* Enter Initialization mode */
    if (LL_RTC_EnterInitMode(RTC) != ERROR)
    {
      counter_time = RTC_ReadTimeCounter();
      
      counter_alarm = (uint32_t)(((uint32_t)AlarmTime->Hours * 3600U) + \
        ((uint32_t)AlarmTime->Minutes * 60U) + \
          ((uint32_t)AlarmTime->Seconds));  
      
      /* Check that requested alarm should expire in the same day (otherwise add 1 day) */
      if (counter_alarm < counter_time)
      {
        /* Add 1 day to alarm counter*/
        counter_alarm += (uint32_t)(24U * 3600U);
      }
      
      LL_RTC_ALARM_Set(RTC, counter_alarm);
      
      /* Exit Initialization mode */
      LL_RTC_ExitInitMode(RTC);
      
      /* Clear flag alarm A */
      // __HAL_RTC_ALARM_CLEAR_FLAG(hrtc, RTC_FLAG_ALRAF);
      LL_RTC_ClearFlag_ALR(RTC);
      
      /* Configure the Alarm interrupt */
      //__HAL_RTC_ALARM_ENABLE_IT(hrtc,RTC_IT_ALRA);
      LL_RTC_EnableIT_ALR(RTC);
      
      /* RTC Alarm Interrupt Configuration: EXTI configuration */
      //__HAL_RTC_ALARM_EXTI_ENABLE_IT();
      SET_BIT(EXTI->IMR, RTC_EXTI_LINE_ALARM_EVENT);
      
      //__HAL_RTC_ALARM_EXTI_ENABLE_RISING_EDGE();
      SET_BIT(EXTI->RTSR, RTC_EXTI_LINE_ALARM_EVENT);
      
    }   
    return RTC_OK;
  }
  
  Rtc::RTCStatus_t Rtc::SetAlarm(uint8_t Hours, uint8_t Minutes, uint8_t Seconds, RTCCallback_t RTCCallback)
  {
    uint32_t counter_alarm = 0U, counter_time;
    
    /* Enter Initialization mode */
    if (LL_RTC_EnterInitMode(RTC) != ERROR)
    {
      counter_time = RTC_ReadTimeCounter();
      
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
      SET_BIT(EXTI->IMR, RTC_EXTI_LINE_ALARM_EVENT);
      
      SET_BIT(EXTI->RTSR, RTC_EXTI_LINE_ALARM_EVENT);
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
      CLEAR_BIT(EXTI->IMR, RTC_EXTI_LINE_ALARM_EVENT);
      
      CLEAR_BIT(EXTI->RTSR, RTC_EXTI_LINE_ALARM_EVENT);
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
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
}