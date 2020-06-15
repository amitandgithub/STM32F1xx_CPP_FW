/*
* PowerMonitor.h
*
*  Created on: 12-May-2020
*      Author: Amit Chaudhary
*  
*/

#include"stdint.h"
#include<cstring>

#include"GpioOutput.h"
#include"ST7735_defs.h"
#include"Utils.h"
#include"Window.h" 
#include"TextWindow.h" 
#include"SettingWindow.h"  
#include"NamedSettingWindow.h"  
#include"Screen.h" 
#include"UI.h" 
#include"AnalogIn.h"

#include"Callback.h"
#include"ST7735.h"
#include"HwButton.h"
#include"INA219.h"
#include"RTC.h"
#include"Uart.h"
#include"PulseOut.h"

#include"ff.h"
#include"diskio.h"

#include"FLASH_PAGE.h"

#ifndef PowerMonitor_h
#define PowerMonitor_h

#define SETTINGS_DATA_FLASH_ADDRESS 0x0801FC00  // Last flash page of STM32F103xB devices
#define POWER_MONITOR_UART uart1

//st7735_Font_7x10  st7735_Font_11x18 st7735_Font_16x26 
#define CURRENT_FONT st7735_Font_11x18
#define CURRENT_FONT_H 18

#define TIME_FONT_H 26
#define TIME_FONT st7735_Font_16x26

#define POWER_SCREEN_FONT CURRENT_FONT

void RestoreSettings();


void PowerMonitor_UI();

void PowerMonitorInit();

void PowerMonitorRun();

void PowerOutputUART();

//#define TimerCount_Word_2 2
//#define Power_mAH_Word_3  3

// Enum for the settings data Address
enum
{
  //UART Settings
  UART_STATE_ADDRESS = SETTINGS_DATA_FLASH_ADDRESS + 0*4, 
  UART_DELAY_ADDRESS = SETTINGS_DATA_FLASH_ADDRESS + 1*4,
  UART_BAUD_RATE_ADDRESS = SETTINGS_DATA_FLASH_ADDRESS + 2*4,
  
  //SD Card Settings
  SD_CARD_STATE_ADDRESS = SETTINGS_DATA_FLASH_ADDRESS + 3*4, 
  SD_CARD_DELAY_ADDRESS = SETTINGS_DATA_FLASH_ADDRESS + 4*4, 
  
  // RTC Setting time  
  RTC_SETTING_TIME_ADDRESS = SETTINGS_DATA_FLASH_ADDRESS + 5*4, 
  
};

enum
{
  RTC_BKP_REG0,
  RTC_BKP_ACCESS_CODE,
  RTC_POWER_TIMER_COUNT,
  RTC_POWER_MAH_VALUE,
  RTC_POWER_SETTINGS,
  RTC_POWER_LOG_INTERVAL, 
  RTC_COUNTER_SET
};

typedef union 
{
  struct 
  {
    uint16_t UartState   :1;
    uint16_t UartBaudrate:3; 
    uint16_t SdCardState :1; 
    uint16_t Brightness  :7;
  }bits;
  
  uint16_t Word16;
}PowerSettings_t;

/*****************************SD CARD ********************************/
#define SD_CARD 0
#define SD_CARD_SAVE_FREQ 100

void BuildName(char* Name);
bool SdStart(FIL* fil, const char* fileName, uint8_t fileoptions);
bool SdAppendData(FIL* fil, const char* FileName, char* pData, uint32_t len);
bool SdStop(FIL* fil);
bool Save_To_SD_Card1(const char* FileName, char* pData, uint32_t len);
bool Save_To_SD_Card(const char* FileName, char* pData, uint32_t len);
void SdOnOffCallback(uint32_t UpdatedSettingValue);

void ResetPowerData();

#if SD_CARD

class SDPressCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) 
    {
      SdOnOffCallback(UpdatedSettingValue);
    }
};
#endif

void UartStatefx(uint32_t UpdatedSettingValue);
class UartStateCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) 
    {
      UartStatefx(UpdatedSettingValue);
    }
};

void UartDelayfx(uint32_t UpdatedSettingValue);
class UartDelayCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) 
    {
      UartDelayfx(UpdatedSettingValue);
    }
};

void UartBaudratefx(uint32_t UpdatedSettingValue);
class UartBrCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) 
    {
      UartBaudratefx(UpdatedSettingValue);
    }
};

void RtcTimeSetfx(uint32_t UpdatedSettingValue);
class RtcTimeSetCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) 
    {
      RtcTimeSetfx(UpdatedSettingValue);
    }
};

void DisplayBrightness(uint32_t brightness);
class DisplayBrightnessCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) 
    {
      DisplayBrightness(UpdatedSettingValue);
    }
};

void Ina219ModeFx(uint32_t UpdatedSettingValue);
class Ina219ModeCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) 
    {
      Ina219ModeFx(UpdatedSettingValue);
    }
};

void AddSecondsFx(uint32_t UpdatedSettingValue);
class AddSecondsCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) 
    {
      AddSecondsFx(UpdatedSettingValue);
    }
};

void SubSecondsFx(uint32_t UpdatedSettingValue);
class SubSecondsCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) 
    {
      SubSecondsFx(UpdatedSettingValue);
    }
};


#endif // PowerMonitor_h