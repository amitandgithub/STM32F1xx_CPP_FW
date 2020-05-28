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

#ifndef PowerMonitor_h
#define PowerMonitor_h

#define POWER_MONITOR_UART uart1

//st7735_Font_7x10  st7735_Font_11x18 st7735_Font_16x26 
#define CURRENT_FONT st7735_Font_11x18
#define CURRENT_FONT_H 18
#define POWER_SCREEN_FONT CURRENT_FONT



void PowerMonitor_UI();

void PowerMonitorInit();

void PowerMonitorRun();

void PowerOutputUART();


/*****************************SD CARD ********************************/
#define SD_CARD 1
#define SD_CARD_SAVE_FREQ 100

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

void UartOnOffCallback(uint32_t UpdatedSettingValue);
class UartPressCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) 
    {
      UartOnOffCallback(UpdatedSettingValue);
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

#endif // PowerMonitor_h