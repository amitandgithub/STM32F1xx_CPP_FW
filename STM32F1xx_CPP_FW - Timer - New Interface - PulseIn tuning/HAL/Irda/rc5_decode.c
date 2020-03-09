/**
  ******************************************************************************
  * @file    rc5_decode.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    08-April-2011
  * @brief   This file provides all the RC5 firmware functions.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
//#include "main.h"

#include "rc5_decode.h"
#include "ir_decode.h"
#include "sirc_decode.h"

    
#include"CPP_HAL.h"
#include"PulseIn.h"
#include"Timer.h"

extern uint32_t RCC_GetSystemClockFreq(void);
extern uint32_t RCC_GetHCLKClockFreq(uint32_t SYSCLK_Frequency);
extern uint32_t RCC_GetPCLK1ClockFreq(uint32_t HCLK_Frequency);
extern uint32_t RCC_GetPCLK2ClockFreq(uint32_t HCLK_Frequency);

#define BUFFER_SIZE  10


/* Logic table for rising edge: every line has values corresponding to previous bit.
   in columns are actual bit values for given bit time */
const tRC5_lastBitType RC5_logicTableRisingEdge[2][2] =
{
  {RC5_ZER ,RC5_INV}, /* lastbit = ZERO */
  {RC5_NAN ,RC5_ZER}, /* lastbit = ONE  */
};

/* Logic table for falling edge: every line has values corresponding to previous bit. 
   in columns are actual bit values for given bit time */
const tRC5_lastBitType RC5_logicTableFallingEdge[2][2] =
{
  {RC5_NAN ,RC5_ONE}, /* lastbit = ZERO */
  {RC5_ONE ,RC5_INV}, /* lastbit = ONE  */
};

/* RC5 address table */
uint8_t const* rc5_devices[32] = {
        "         TV1        ",                  /*  0 */
        "         TV2        ",                  /*  1 */
        "      Video Text    ",                  /*  2 */
        "     Extension TV   ",                  /*  3 */
        "   LaserVideoPlayer ",                  /*  4 */
        "         VCR1       ",                  /*  5 */
        "         VCR2       ",                  /*  6 */
        "       Reserved     ",                  /*  7 */
        "         Sat1       ",                  /*  8 */
        "     Extension VCR  ",                  /*  9 */
        "         Sat2       ",                  /* 10 */
        "       Reserved     ",                  /* 11 */
        "       CD Video     ",                  /* 12 */
        "       Reserved     ",                  /* 13 */
        "       CD Photo     ",                  /* 14 */
        "       Reserved     ",                  /* 15 */
        "   Preampli Audio 1 ",                  /* 16 */
        "        Tuner       ",                  /* 17 */
        "    Analog Magneto  ",                  /* 18 */
        "   Preampli Audio 2 ",                  /* 19 */
        "         CD         ",                  /* 20 */
        "      Rack Audio    ",                  /* 21 */
        " Audio Sat Receiver ",                  /* 22 */
        "     DDC Magneto    ",                  /* 23 */
        "      Reserved      ",                  /* 24 */
        "      Reserved      ",                  /* 25 */
        "       CDRW         ",                  /* 26 */
        "      Reserved      ",                  /* 27 */
        "      Reserved      ",                  /* 28 */
        "      Reserved      ",                  /* 29 */
        "      Reserved      ",                  /* 30 */
        "      Reserved      "                   /* 31 */
       };

/* RC5 commands table */
uint8_t const* rc5_Commands[128] = {
        "        Num0        ",                                       /* 0 */
        "        Num1        ",                                       /* 1 */
        "        Num2        ",                                       /* 2 */
        "        Num3        ",                                       /* 3 */
        "        Num4        ",                                       /* 4 */
        "        Num5        ",                                       /* 5 */
        "        Num6        ",                                       /* 6 */
        "        Num7        ",                                       /* 7 */
        "        Num8        ",                                       /* 8 */
        "        Num9        ",                                       /* 9 */
        "      TV Digits     ",                                       /* 10 */
        "       TV Freq      ",                                       /* 11 */
        "      TV StandBy    ",                                       /* 12 */
        "    TV Mute On-Off  ",                                       /* 13 */
        "    TV Preference   ",                                       /* 14 */
        "     TV Display     ",                                       /* 15 */
        "     Volume Up      ",                                       /* 16 */
        "    Volume Down     ",                                       /* 17 */
        "    Brightness Up   ",                                       /* 18 */
        "   Brightness Down  ",                                       /* 19 */
        " Color Saturation Up",                                       /* 20 */
        "ColorSaturation Down",                                       /* 21 */
        "       Bass Up      ",                                       /* 22 */
        "      Bass Down     ",                                       /* 23 */
        "      Treble Up     ",                                       /* 24 */
        "     Treble Down    ",                                       /* 25 */
        "    Balance Right   ",                                       /* 26 */
        "    BalanceLeft     ",                                       /* 27 */
        "   TV Contrast Up   ",                                       /* 28 */
        "  TV Contrast Down  ",                                       /* 29 */
        "   TV Search Up     ",                                       /* 30 */
        "  TV tint-hue Down  ",                                       /* 31 */
        "   TV CH Prog Up    ",                                       /* 32 */
        "   TV CH ProgDown   ",                                       /* 33 */
        "TV Last prog-channel",                                       /* 34 */
        " TV Select language ",                                       /* 35 */
        " TV Spacial Stereo  ",                                       /* 36 */
        "  TV Stereo Mono    ",                                       /* 37 */
        "  TV Sleep Timer    ",                                       /* 38 */
        "   TV tint-hue Up   ",                                       /* 39 */
        "   TV RF Switch     ",                                       /* 40 */
        "   TV Store-VOTE    ",                                       /* 41 */
        "      TV Time       ",                                       /* 42 */
        "   TV Scan Fwd Inc  ",                                       /* 43 */
        "    TV Decrement    ",                                       /* 44 */
        "      Reserved      ",                                       /* 45 */
        "   TV control-menu  ",                                       /* 46 */
        "    TV Show Clock   ",                                       /* 47 */
        "       TV Pause     ",                                       /* 48 */
        "   TV Erase Entry   ",                                       /* 49 */
        "     TV Rewind      ",                                       /* 50 */
        "      TV Goto       ",                                       /* 51 */
        "      TV Wind       ",                                       /* 52 */
        "      TV Play       ",                                       /* 53 */
        "      TV Stop       ",                                       /* 54 */
        "      TV Record     ",                                       /* 55 */
        "    TV External 1   ",                                       /* 56 */
        "    TV External 2   ",                                       /* 57 */
        "      Reserved      ",                                       /* 58 */
        "     TV Advance     ",                                       /* 59 */
        "   TV TXT-TV toggle ",                                       /* 60 */
        "  TV System StandBy ",                                       /* 61 */
        "TV Picture Crispener",                                       /* 62 */
        "    System Select   ",                                       /* 63 */
        "      Reserved      ",                                       /* 64 */
        "      Reserved      ",                                       /* 65 */
        "      Reserved      ",                                       /* 66 */
        "      Reserved      ",                                       /* 67 */
        "      Reserved      ",                                       /* 68 */
        "      Reserved      ",                                       /* 69 */
        "  TV Speech Music   ",                                       /* 70 */
        "  DIM Local Display ",                                       /* 71 */
        "      Reserved      ",                                       /* 72 */
        "      Reserved      ",                                       /* 73 */
        "      Reserved      ",                                       /* 74 */
        "      Reserved      ",                                       /* 75 */
        "      Reserved      ",                                       /* 76 */
        "Inc Control Setting ",                                       /* 77 */
        "Dec Control Setting ",                                       /* 78 */
        "   TV Sound Scroll  ",                                       /* 79 */
        "        Step Up     ",                                       /* 80 */
        "       Step Down    ",                                       /* 81 */
        "        Menu On     ",                                       /* 82 */
        "        Menu Off    ",                                       /* 83 */
        "       AV Status    ",                                       /* 84 */
        "      Step Left     ",                                       /* 85 */
        "      Step Right    ",                                       /* 86 */
        "     Acknowledge    ",                                       /* 87 */
        "      PIP On Off    ",                                       /* 88 */
        "       PIP Shift    ",                                       /* 89 */
        "    PIP Main Swap   ",                                       /* 90 */
        "    Strobe On Off   ",                                       /* 91 */
        "     Multi Strobe   ",                                       /* 92 */
        "      Main Frozen   ",                                       /* 93 */
        "  3div9 Multi scan  ",                                       /* 94 */
        "      PIPSelect     ",                                       /* 95 */
        "      MultiPIP      ",                                       /* 96 */
        "     Picture DNR    ",                                       /* 97 */
        "     Main Stored    ",                                       /* 98 */
        "     PIP Strobe     ",                                       /* 99 */
        "    Stored Picture  ",                                       /* 100 */
        "      PIP Freeze    ",                                       /* 101 */
        "      PIP Step Up   ",                                       /* 102 */
        "    PIP Step Down   ",                                       /* 103 */
        "    TV PIP Size     ",                                       /* 104 */
        "  TV Picture Scroll ",                                       /* 105 */
        " TV Actuate Colored ",                                       /* 106 */
        "        TV Red      ",                                       /* 107 */
        "       TV Green     ",                                       /* 108 */
        "      TV Yellow     ",                                       /* 109 */
        "       TV Cyan      ",                                       /* 110 */
        "    TV Index White  ",                                       /* 111 */
        "        TV Next     ",                                       /* 112 */
        "     TV Previous    ",                                       /* 113 */
        "       Reserved     ",                                       /* 114 */
        "       Reserved     ",                                       /* 115 */
        "       Reserved     ",                                       /* 116 */
        "       Reserved     ",                                       /* 117 */
        "       Sub Mode     ",                                       /* 118 */
        "   Option Sub Mode  ",                                       /* 119 */
        "       Reserved     ",                                       /* 120 */
        "       Reserved     ",                                       /* 121 */
        "TV Store Open Close ",                                       /* 122 */
        "       Connect      ",                                       /* 123 */
        "     Disconnect     ",                                       /* 124 */
        "       Reserved     ",                                       /* 125 */
        "  TV Movie Expand   ",                                       /* 126 */
        "  TV Parental Access"                                        /* 127 */
   };

__IO StatusYesOrNo RC5_FrameReceived = NO; /*!< RC5 Frame state */
__IO tRC5_packet   RC5_tmp_packet;   /*!< first empty packet */

/* RC5  bits time definitions */
uint16_t  RC5_T_Min = 0;
uint16_t  RC5_T_Max = 0;
uint16_t  RC5_2T_Min = 0;
uint16_t  RC5_2T_Max = 0;
uint32_t  RC5_Data =0;
__IO uint32_t RC5_TIM_CLKValueKHz; /*!< Timer clock */
uint16_t RC5_TimeOut = 0, count=0;
extern uint8_t Display;
uint16_t Tab[BUFFER_SIZE];
uint8_t i = 0;

static uint8_t IR_RC5_GetPulseLength (uint16_t pulseLength);
static void IR_RC5_modifyLastBit(tRC5_lastBitType bit);
static void IR_RC5_WriteBit(uint8_t bitVal);
static uint32_t TIM_GetCounterCLKValue(void);
  
/**
  * @brief  De-initializes the peripherals (RCC,GPIO, TIM)       
  * @param  None
  * @retval None
  */
void IR_RC5_DeInit(void)
{ 
  //TIM_DeInit(IR_TIM);
  //GPIO_DeInit(IR_GPIO_PORT);
}

/**
  * @brief  Initialize the RC5 decoder module ( Time range)
  * @param  None
  * @retval None
  */
void IR_RC5_Init(void)
{ 
  static HAL::PulseIn<(uint32_t)TIM3,TIM3_CH1_PIN_A6,                      
                      HAL::Timer::FALLING_EDGE,
                      HAL::Timer::RISING_EDGE,
                      1> PWM_IR();
  
   /* Timer Clock */
  RC5_TIM_CLKValueKHz = TIM_GetCounterCLKValue()/1000; 
  RC5_TimeOut = RC5_TIM_CLKValueKHz * RC5_TIME_OUT_US/1000;
  
//  TIM_InitStruct.Prescaler         = TIM_PRESCALER;
//  TIM_InitStruct.CounterMode       = LL_TIM_COUNTERMODE_UP;
//  TIM_InitStruct.Autoreload        = RC5_TimeOut;
//  TIM_InitStruct.ClockDivision     = LL_TIM_CLOCKDIVISION_DIV1;
//  TIM_InitStruct.RepetitionCounter = (uint8_t)0x00;
  
  PWM_IR.HwInit(TIM_PRESCALER,RC5_TimeOut,true);
  
#if 0
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;

  /* Clock Configuration for TIMER */
  RCC_APB1PeriphClockCmd(IR_TIM_CLK , ENABLE);

  /* Enable Button GPIO clock */
  RCC_APB2PeriphClockCmd(IR_GPIO_PORT_CLK | RCC_APB2Periph_AFIO, ENABLE);
  
  /* Pin configuration: input floating */
  GPIO_InitStructure.GPIO_Pin = IR_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(IR_GPIO_PORT, &GPIO_InitStructure);

  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Enable the TIM global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = IR_TIM_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* TIMER frequency input */
  TIM_PrescalerConfig(IR_TIM, TIM_PRESCALER, TIM_PSCReloadMode_Immediate);
  /* TIM configuration */
  TIM_ICInitStructure.TIM_Channel = IR_TIM_Channel;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(IR_TIM, &TIM_ICInitStructure); 
  /* Timer Clock */
  RC5_TIM_CLKValueKHz = TIM_GetCounterCLKValue()/1000; 
  /* Select the TIM3 Input Trigger: TI1FP1 */
  TIM_SelectInputTrigger(IR_TIM, TIM_TS_TI1FP1);
  /* Select the slave Mode: Reset Mode */
  TIM_SelectSlaveMode(IR_TIM, TIM_SlaveMode_Reset);
  /* Enable the Master/Slave Mode */
  TIM_SelectMasterSlaveMode(IR_TIM, TIM_MasterSlaveMode_Enable);
  /* Configures the TIM Update Request Interrupt source: counter overflow */
  TIM_UpdateRequestConfig(IR_TIM,  TIM_UpdateSource_Regular);

  RC5_TimeOut = RC5_TIM_CLKValueKHz * RC5_TIME_OUT_US/1000;
  /* Set the TIM auto-reload register for each IR protocol */
  IR_TIM->ARR = RC5_TimeOut;

  /* Clear update flag */
  TIM_ClearFlag(IR_TIM, TIM_FLAG_Update);
  /* Enable TIM3 Update Event Interrupt Request */
  TIM_ITConfig(IR_TIM, TIM_IT_Update, ENABLE);
  /* Enable the CC2/CC1 Interrupt Request */
  TIM_ITConfig(IR_TIM, TIM_IT_CC2, ENABLE);
  TIM_ITConfig(IR_TIM, TIM_IT_CC1, ENABLE);
  /* Enable the timer */
  TIM_Cmd(IR_TIM, ENABLE); 
#endif
  
  /* Bit time range */
  RC5_T_Min = (RC5_T_US - RC5_T_TOLERANCE_US) * RC5_TIM_CLKValueKHz / 1000;
  RC5_T_Max = (RC5_T_US + RC5_T_TOLERANCE_US) * RC5_TIM_CLKValueKHz / 1000;
  RC5_2T_Min = (2 * RC5_T_US - RC5_T_TOLERANCE_US) * RC5_TIM_CLKValueKHz / 1000;
  RC5_2T_Max = (2 * RC5_T_US + RC5_T_TOLERANCE_US) * RC5_TIM_CLKValueKHz / 1000;
  /* Default state */
  IR_RC5_ResetPacket();
}

/**
  * @brief  Display the RC5 field frame ( ADDRESS, COMMAND)
  * @param  RC5 structure
  * @retval None
  */
void IR_RC5_Decode(RC5Frame_TypeDef *rc5_frame)
{ 
  /* If frame received */
  if(RC5_FrameReceived != NO)
  {
    /* RC5 frame field decoding */
    RC5_Data = Tab[i];
    rc5_frame->FieldBit = (Tab[i] >> 12) & 0x1;
    rc5_frame->ToggleBit = (Tab[i] >> 11) & 0x1;
    rc5_frame->Address = (Tab[i] >> 6) & 0x1F;
    rc5_frame->Command = (Tab[i]) & 0x3F;    
    i++;
    if (i == BUFFER_SIZE)
      i = 0;

    /* Default state */
    RC5_FrameReceived = NO;
    IR_RC5_ResetPacket();
    
    /* Display RC5 message */
    //LCD_DisplayStringLine(LCD_LINE_5, rc5_Commands[rc5_frame->Command]); 
    //LCD_DisplayStringLine(LCD_LINE_6, rc5_devices[rc5_frame->Address]);
    HAL::DBG_PRINT((uint8_t*)rc5_Commands[rc5_frame->Command],20+2);
    HAL::DBG_PRINT_NL();
    HAL::DBG_PRINT((uint8_t*)rc5_devices[rc5_frame->Address],20+2);
    HAL::DBG_PRINT_NL();
         
  }
}
/**
  * @brief  Set the incoming packet structure to default state
  * @param  None
  * @retval None
  */
void IR_RC5_ResetPacket(void)
{
  RC5_tmp_packet.data = 0;
  RC5_tmp_packet.bitCount = RC5_PACKET_BIT_COUNT - 1;
  RC5_tmp_packet.lastBit = RC5_ONE;
  RC5_tmp_packet.status = RC5_PACKET_STATUS_EMPTY;
}
/**
  * @brief  Identify the RC5 data bits
  * @param  rawPulseLength: low/high pulse duration
            edge: '1' for Rising  or '0' for falling edge
  * @retval None
  */
void IR_RC5_DataSampling(uint16_t rawPulseLength, uint8_t edge)
{ 
  uint8_t pulse;
  tRC5_lastBitType tmpLastBit;
  
  /* Decode the pulse length in protocol units */
  pulse = IR_RC5_GetPulseLength(rawPulseLength);
  /* On Rising Edge */
  if (edge == 1)
  { 
    if (pulse <= RC5_2T_TIME) 
    { 
      /* Bit dertermination by the rising edge */
      tmpLastBit = RC5_logicTableRisingEdge[RC5_tmp_packet.lastBit][pulse];
      IR_RC5_modifyLastBit (tmpLastBit);
    }
    else
    {
      IR_RC5_ResetPacket();
    }
  } 
  else  /* On Falling Edge */
  {
    /* If this is the first falling edge - don't compute anything */
    if (RC5_tmp_packet.status & RC5_PACKET_STATUS_EMPTY)
    { 
      RC5_tmp_packet.status &= (uint8_t)~RC5_PACKET_STATUS_EMPTY;
    }
    else
    {
      if (pulse <= RC5_2T_TIME) 
      { 
        /* Bit determination by the falling edge */
        tmpLastBit = RC5_logicTableFallingEdge[RC5_tmp_packet.lastBit][pulse];
        IR_RC5_modifyLastBit (tmpLastBit);
      }
      else
      {
        IR_RC5_ResetPacket();
      }
    }
  }
}

/**
  * @brief  Convert raw pulse length expressed in timer ticks to protocol bit times.
  * @param  pulseLength:pulse duration
  * @retval bit time value
  */
static uint8_t IR_RC5_GetPulseLength (uint16_t pulseLength)
{ 
  /* Valid bit time */
  if ((pulseLength > RC5_T_Min) && (pulseLength < RC5_T_Max))
  { 
    /* We've found the length */
    return (RC5_1T_TIME); /* Return the correct value */
  }
  else if ((pulseLength > RC5_2T_Min) && (pulseLength < RC5_2T_Max))
  {
    /* We've found the length */
    return (RC5_2T_TIME); /* Return the correct value */
  }
  return RC5_WRONG_TIME; /* Error */
}

/**
  * @brief  Perform checks if the last bit was not incorrect. 
  * @param  bit: where bit can be  RC5_NAN or RC5_INV or RC5_ZER or RC5_ONE
  * @retval None
  */
static void IR_RC5_modifyLastBit(tRC5_lastBitType bit)
{
  if (bit != RC5_NAN)
  {
    if (RC5_tmp_packet.lastBit != RC5_INV)
    { 
      /* Restore the last bit */
      RC5_tmp_packet.lastBit = bit;
      /* Insert one bit into the RC5 Packet */
      IR_RC5_WriteBit(RC5_tmp_packet.lastBit);
    }
    else 
    {
      IR_RC5_ResetPacket();
    }
  }
}



/**
  * @brief  Insert one bit into the final data word 
  * @param  bitVal: bit value 'RC5_ONE' or 'RC5_ZER'
  * @retval None
  */
static void IR_RC5_WriteBit(uint8_t bitVal)
{
  /* First convert RC5 symbols to ones and zeros */
  if (bitVal == RC5_ONE)
  { 
    bitVal = 1;
  }
  else if (bitVal == RC5_ZER)
  {
    bitVal = 0;
  }
  else
  {
    IR_RC5_ResetPacket();
    return;
  }
  /* Write this particular bit to data field */
  RC5_tmp_packet.data |=  bitVal; 
  
  /* Test the bit number determined */ 
  if (RC5_tmp_packet.bitCount != 0)  /* If this is not the last bit */
  {
    /* Shift the data field */
    RC5_tmp_packet.data = RC5_tmp_packet.data << 1;
    /* And decrement the bitCount */
    RC5_tmp_packet.bitCount--;
  }
  else
  {
    Tab[i] = RC5_tmp_packet.data;
    RC5_FrameReceived = YES; 
  }
}

/**
  * @brief  Identify TIM clock
  * @param  None
  * @retval Timer clock
  */
static uint32_t TIM_GetCounterCLKValue(void)
{
  uint32_t apbprescaler = 0, apbfrequency = 0;
  uint32_t timprescaler = 0;
  
  /* This function fills the RCC_ClockFreq structure with the current
  frequencies of different on chip clocks */
 //RCC_GetClocksFreq((RCC_ClocksTypeDef*)&RCC_ClockFreq);
  
  if((IR_TIM == TIM1))
  {    
    /* Get the clock prescaler of APB2 */
    apbprescaler = ((RCC->CFGR >> 11) & 0x7);
    apbfrequency = RCC_GetPCLK2ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq())); //RCC_ClockFreq.PCLK2_Frequency;
    timprescaler = TIM_PRESCALER;
  }
  else if((IR_TIM == TIM2) || (IR_TIM == TIM3) || (IR_TIM == TIM4) )
  {
    /* Get the clock prescaler of APB1 */
    apbprescaler = ((RCC->CFGR >> 8) & 0x7);
    apbfrequency = RCC_GetPCLK1ClockFreq(RCC_GetHCLKClockFreq(RCC_GetSystemClockFreq())); // RCC_ClockFreq.PCLK1_Frequency; 
    timprescaler = TIM_PRESCALER;
  }

  /* If APBx clock div >= 4 */
  if (apbprescaler >= 4)
  {
    return ((apbfrequency * 2)/(timprescaler + 1));
  }
  else
  {
    return (apbfrequency/(timprescaler+ 1));
  }
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
