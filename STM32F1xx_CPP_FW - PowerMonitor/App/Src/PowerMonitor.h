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

class SDPressCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) 
    {
      SdOnOffCallback(UpdatedSettingValue);
    }
};





















#endif // PowerMonitor_h