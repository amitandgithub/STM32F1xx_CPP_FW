


#include"PowerMonitor.h"

#include"Callback.h"
#include"CPP_HAL.h"



using namespace HAL;
using namespace BSP;


extern INA219 INA219_Dev;
extern AnalogIn ChipTemperature;
extern Rtc rtc;
extern Uart uart1;


extern HAL::AnalogIn adc1;
//static AnalogIn adc0(LL_ADC_CHANNEL_0);
static HAL::PulseOut<(uint32_t)TIM3,TIM3_CH3_PIN_B0,10000> Dimmer; //TIM3_CH1_PIN_A6 

static const uint32_t UartBaudRateTable[] = {4800,9600,19200,38400,115200,230400,460800};
PowerSettings_t PowerSettings;
BSP::INA219::Power_t Power;
uint8_t V_Len, I_Len, mA_Len;
uint32_t SD_Time,SdSampleTime=1000,SdPrevSampleTime;
static uint16_t TimerCount;
static uint32_t PrevTimerCount;

uint32_t Uart_Time,UartState,UartSampleTime = 1000,UartPrevSampleTime,UartBr = 1;

char SD_Power_Array[20];

#if SD_CARD
/* Globals for SD_CARD task*/
FATFS fs;
FIL fil;
FRESULT fr;
char FileName[32];
#endif


#define CHARS_IN_LINE 14
HMI::InputEvents_t InputEvents = HMI::NONE;

static volatile uint32_t sg_CurrentsettingValue;

class SettingCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue)
    {
      //printf("setting Done = %d \n\r",UpdatedSettingValue); 
      sg_CurrentsettingValue = UpdatedSettingValue;
    }
};

class DownCallback : public HAL::Callback
{
    void CallbackFunction()
    {
     InputEvents = HMI::DOWN;
    }
};
  
class PressCallback : public HAL::Callback
{
    void CallbackFunction()
    {
      InputEvents = HMI::PRESS;
    }
};

class LongPressCallback1 : public HAL::Callback
{
    void CallbackFunction()
    {
      InputEvents = HMI::LONGPRESS;
    }
};

static DownCallback DownCb;

static PressCallback PressCb;

static LongPressCallback1 LongPressCb;

static BSP::HwButton<HAL::DigitalIn<B11/*B11 A8*/,INPUT_PULLDOWN>,reinterpret_cast <HAL::Callback *>(&DownCb),reinterpret_cast <HAL::Callback *>(&PressCb),reinterpret_cast <HAL::Callback *>(&LongPressCb)> B9_HwBtnInt; 

enum
{
  Line1,
  Line2,
  Line3,
  Line4,
  Line5,
  Line6,
  Line7,
  Line8  
};

/*************************************************** NamedSetting Window *****************************************************************/
#if SD_CARD
SDPressCallback SDPressCb;

char SdCardText[20] = "SD Card  ";       
uint32_t SdCardState = 0;                             
const HMI::WindowContext_t  NamedWindowContextSdCard   = {0,127,CURRENT_FONT_H*Line1,CURRENT_FONT_H*Line2,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::NamedSettingTable_t NamedSettingTableSdCard[] = { {0,(char*)"OFF"}, {1,(char*)"ON"}};
const HMI::NamedSettingContext_t NamedSettingContextSdCard  = {&SdCardState,SdCardText,&SDPressCb,14,8,2,NamedSettingTableSdCard};
#else
char SdCardText[20] = "SD Card  ";       
uint32_t SdCardState = 0;                             
const HMI::WindowContext_t  NamedWindowContextSdCard   = {0,127,CURRENT_FONT_H*Line1,CURRENT_FONT_H*Line2,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::NamedSettingTable_t NamedSettingTableSdCard[] = { {0,(char*)"OFF"}, {1,(char*)"ON"}};
const HMI::NamedSettingContext_t NamedSettingContextSdCard  = {&SdCardState,SdCardText,nullptr,14,8,2,NamedSettingTableSdCard};
#endif
static HMI::NamedSettingWindow NamedSettingWindowSdCard(&NamedWindowContextSdCard,&NamedSettingContextSdCard);

UartStateCallback UartStateCb;
char UartText[20] = "UART        ";                                  
const HMI::WindowContext_t  NamedWindowContextUart   = {0,127,CURRENT_FONT_H*Line3,CURRENT_FONT_H*Line4,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::NamedSettingTable_t NamedSettingTableUart[] = { {0,(char*)"OFF"}, {1,(char*)"ON"}};
const HMI::NamedSettingContext_t NamedSettingContextUart  = {&UartState,UartText,&UartStateCb,14,8,2,NamedSettingTableUart};
static HMI::NamedSettingWindow NamedSettingWindowUart(&NamedWindowContextUart,&NamedSettingContextUart);


/*************************************************** Setting Window *****************************************************************/
UartDelayCallback UartDelayCb;
char SDDelayText[20] = "SD delay           ";
const HMI::WindowContext_t  WindowContextSDDelay   = {0,127,CURRENT_FONT_H*Line2,CURRENT_FONT_H*Line3,&CURRENT_FONT,GREEN,BLACK,RED}; 
const HMI::SettingContext_t SettingContextSDDelay  = {&SdSampleTime,SDDelayText,&UartDelayCb,14,4,9};
static HMI::SettingWindow SettingWindowSDDelay(&WindowContextSDDelay,&SettingContextSDDelay); 

char UartDelayText[20] = "Uart dly           ";                            
const HMI::WindowContext_t  WindowContextUartDelay   = {0,127,CURRENT_FONT_H*Line4,CURRENT_FONT_H*Line5,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t SettingContextUartDelay  = {&UartSampleTime,UartDelayText,&UartDelayCb,14,4,9};
static HMI::SettingWindow SettingWindowUartDelay(&WindowContextUartDelay,&SettingContextUartDelay);

UartBrCallback UartBaudrateCallback;
char TextBr[20] = "Uart Br           ";                            
const HMI::WindowContext_t  WindowContextBr   = {0,127,CURRENT_FONT_H*Line5,CURRENT_FONT_H*Line6,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t SettingContextBr  = {&UartBr,TextBr,&UartBaudrateCallback,14,6,8};
static HMI::SettingWindow SettingWindowBr(&WindowContextBr,&SettingContextBr); 

char UartBrText[20] = "UART Br 460800";                                  
const HMI::WindowContext_t  UartBrWindowContext   = {0,127,CURRENT_FONT_H*Line5,CURRENT_FONT_H*Line6,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::NamedSettingTable_t UartBrSettingTable[] = { {0,(char*)"4800"}, {1,(char*)"9600"}, {2,(char*)"19200"}, {3,(char*)"38400"}, {4,(char*)"115200"}, {5,(char*)"230400"}, {6,(char*)"460800"} };
const HMI::NamedSettingContext_t UartBrSettingContext  = {&UartBr,UartBrText,&UartBaudrateCallback,14,8,7,UartBrSettingTable};
static HMI::NamedSettingWindow UartBrNamedSettingWindow(&UartBrWindowContext,&UartBrSettingContext);


char RTC_date_Text[20] = "SetDate yymmdd ";       
uint32_t RTC_date;                             
const HMI::WindowContext_t  RTC_date_WindowContext        = {0,127,CURRENT_FONT_H*Line1,CURRENT_FONT_H*Line2,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t RTC_date_Text_SettingContext  = {&RTC_date,RTC_date_Text,nullptr,14,6,8};
static HMI::SettingWindow SettingWindowRTC_date(&RTC_date_WindowContext,&RTC_date_Text_SettingContext); 

RtcTimeSetCallback RtcTimeSetCb;
char RTC_time_Text[20] = "SetTime hhmmss ";       
uint32_t RTC_time;                             
const HMI::WindowContext_t  RTC_time_WindowContext        = {0,127,CURRENT_FONT_H*Line2,CURRENT_FONT_H*Line3,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t RTC_time_Text_SettingContext  = {&RTC_time,RTC_time_Text,&RtcTimeSetCb,14,6,8};
static HMI::SettingWindow SettingWindowRTC_time(&RTC_time_WindowContext,&RTC_time_Text_SettingContext); 

AddSecondsCallback AddSecondsCb;
uint32_t AddSeconds;
char AddSecondsText[20] = "RTC Sec+           ";                            
const HMI::WindowContext_t  WindowContextAddSeconds   = {0,127,CURRENT_FONT_H*Line3,CURRENT_FONT_H*Line4,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t SettingContextAddSeconds  = {&AddSeconds,AddSecondsText,&AddSecondsCb,14,4,9};
static HMI::SettingWindow SettingWindowAddSeconds(&WindowContextAddSeconds,&SettingContextAddSeconds);

SubSecondsCallback SubSecondsCb;
uint32_t SubSeconds;
char SubSecondsText[20] = "RTC Sec-           ";                            
const HMI::WindowContext_t  WindowContextSubSeconds   = {0,127,CURRENT_FONT_H*Line4,CURRENT_FONT_H*Line5,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t SettingContextSubSeconds  = {&SubSeconds,SubSecondsText,&SubSecondsCb,14,4,9};
static HMI::SettingWindow SettingWindowSubSeconds(&WindowContextSubSeconds,&SettingContextSubSeconds);

DisplayBrightnessCallback DisplayBrightnessCb;
char Brightness_Text[15] = "Brightness 100";       
uint32_t Brightness = 100;                             
const HMI::WindowContext_t  BrightnessWindowContext        = {0,127,CURRENT_FONT_H*Line5,CURRENT_FONT_H*Line6,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t BrightnessText_SettingContext  = {&Brightness,Brightness_Text,&DisplayBrightnessCb,14,3,11};
static HMI::SettingWindow SettingWindowBrightness(&BrightnessWindowContext,&BrightnessText_SettingContext);



Ina219ModeCallback Ina219ModeCb;
uint32_t CurrentIna219Mode;
char Ina219ModeText[20] = "Mode        ";                                  
const HMI::WindowContext_t  NamedWindowContextIna219Mode   = {0,127,CURRENT_FONT_H*Line6,CURRENT_FONT_H*Line7,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::NamedSettingTable_t NamedSettingTableIna219Mode[] = { {0,(char*)"400ma,16V"}, {1,(char*)"3200ma,26V"}};
const HMI::NamedSettingContext_t NamedSettingContextIna219Mode  = {&CurrentIna219Mode,Ina219ModeText,&Ina219ModeCb,14,5,2,NamedSettingTableIna219Mode};
static HMI::NamedSettingWindow NamedSettingWindowIna219Mode(&NamedWindowContextIna219Mode,&NamedSettingContextIna219Mode);

/*************************************************** Text Window *****************************************************************/
class TextUpCallback : public Callback
{
    void CallbackFunction()
    {
      HMI::UI::GoToScreen(2);
    }
};
class TextDownCallback : public Callback
{
    void CallbackFunction()
    {
      HMI::UI::GoToScreen(2);
    }
};

class TextLPressCallback : public Callback
{
    void CallbackFunction()
    {
      ResetPowerData();
    }
};

TextLPressCallback LPresscb;
char PowerText[4*14+1] = "T = 00:00:00  "
                         "V =           "
                         "I =           "
                         "C =           ";
                         
const HMI::WindowContext_t  PowerWindowContext   = {0,127,0,159,&POWER_SCREEN_FONT,GREEN,BLACK,RED}; 
const HMI::TextWindowContext_t PowerTextContext  = {(char*)PowerText,nullptr,nullptr,&LPresscb,&LPresscb};
static HMI::TextWindow PowerWindow(&PowerWindowContext,&PowerTextContext);

/*************************** TIME  ******************************************************************/
char TimeText[10]      =  "10:20:20 ";                         
const HMI::WindowContext_t  TimeWindowContext   = {0,127,0,TIME_FONT_H*Line1,&TIME_FONT,GREEN,BLACK,RED}; 
const HMI::TextWindowContext_t TimeTextContext  = {(char*)TimeText,nullptr,nullptr,nullptr,nullptr};
static HMI::TextWindow TimeWindow(&TimeWindowContext,&TimeTextContext);

/*************************** Date  ******************************************************************/
char DateText[] =  "Mon,24 May 20 "
                   "25'c          "
                   "Bat 12.0v     ";

const HMI::WindowContext_t  DateWindowContext   = {0,127,TIME_FONT_H*Line2,TIME_FONT_H*Line3,&POWER_SCREEN_FONT,GREEN,BLACK,RED}; 
const HMI::TextWindowContext_t DateTextContext  = {(char*)DateText,nullptr,nullptr,nullptr,nullptr};
static HMI::TextWindow DateWindow(&DateWindowContext,&DateTextContext);

HMI::Screen ClockScreen;
HMI::Screen HomeScreen;
HMI::Screen TimenDateScreen;
HMI::Screen LivePowerScreen;

static HMI::UI MyUI;

static volatile uint32_t t1;
 
  void PowerMonitorInit()
  {
    Dimmer.HwInit();  
    Dimmer.SetDutyCycle(100); //1-24.6mv 2-52.5 10-276mv  50-1.39v 99 - 2.75v 100 2.77v
    
    // Power up Backup Domain for accessing RTC NV RAM
    rtc.PowerBackupDomain();
    adc1.HwInit(A1,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    
    RestoreSettings();
    // Initialize Current sensor
    INA219_Dev.HwInit();
    
    // Initialize On chip Temperature sensor
    ChipTemperature.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    
    TFT_1_8.HwInit();
    TFT_1_8.Rotate(BSP::ST7735::ROTATE_270_DEG);
    TFT_1_8.FillScreen(BLACK);
    B9_HwBtnInt.HwInit();    
    
    ClockScreen.Register(&TimeWindow);
    ClockScreen.Register(&DateWindow);
    
    HomeScreen.Register(&NamedSettingWindowSdCard);    
    HomeScreen.Register(&SettingWindowSDDelay); 
    HomeScreen.Register(&NamedSettingWindowUart);    
    HomeScreen.Register(&SettingWindowUartDelay); 
    HomeScreen.Register(&UartBrNamedSettingWindow);

    LivePowerScreen.Register(&PowerWindow);
    
    TimenDateScreen.Register(&SettingWindowRTC_time); 
    TimenDateScreen.Register(&SettingWindowRTC_date); 
    TimenDateScreen.Register(&SettingWindowAddSeconds); 
    TimenDateScreen.Register(&SettingWindowSubSeconds);
    TimenDateScreen.Register(&SettingWindowBrightness); 
    TimenDateScreen.Register(&NamedSettingWindowIna219Mode);

    MyUI.Register(&ClockScreen);
    MyUI.Register(&LivePowerScreen);  
    MyUI.Register(&HomeScreen);
    MyUI.Register(&TimenDateScreen);
  
  }
  static uint32_t TempPrevSampleTime,TempSampleTime = 5000;
  void PowerMonitorRun()
  {  
    uint32_t Millis;
    
   // adc0.HwInit(A0,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    B9_HwBtnInt.Run();  
    PowerMonitor_UI();
    B9_HwBtnInt.Run();  
    
    if(InputEvents != HMI::NONE)
    {
      MyUI.EventHandler(InputEvents);
      InputEvents = HMI::NONE;
    }
    Millis = HAL::GetTick();
    MyUI.Display(HMI::Screen::WINDOW_HIGHLIGHT_COLOR);
    t1 = HAL::GetTick() - Millis;   
    
    Millis = HAL::GetTick();
    if(UartState && (Millis >= (UartPrevSampleTime + UartSampleTime)) )
    {
      UartPrevSampleTime = Millis;
      PowerOutputUART();
    }    
    
    Millis = HAL::GetTick();
    if(UartState && (Millis >= (TempPrevSampleTime + TempSampleTime)) )
    {
       TempPrevSampleTime = Millis;
       intToStr(__LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(4300,1400,25,3300,ChipTemperature.Read(),LL_ADC_RESOLUTION_12B), (char*)&DateText[CHARS_IN_LINE*Line2], 1,'\'');
       DateText[CHARS_IN_LINE*Line2 + 3] = 'c';
       
       ftoa(__LL_ADC_CALC_DATA_TO_VOLTAGE(3300,adc1.Read(),LL_ADC_RESOLUTION_12B)/100.0, (char*)&DateText[CHARS_IN_LINE*Line3 + 4], 1,'v');
       
    }    
    
   
    
#if SD_CARD
    
   Millis = HAL::GetTick();
   if(SdCardState && (Millis >= (SdPrevSampleTime + SdSampleTime)) )
   {
     SdPrevSampleTime = Millis;
     memcpy(&SD_Power_Array[0],&PowerText[CHARS_IN_LINE*Line2 + 4],V_Len);
     SD_Power_Array[V_Len++] = ',';
     
     memcpy(&SD_Power_Array[V_Len],&PowerText[CHARS_IN_LINE*Line3 + 4],I_Len);
     SD_Power_Array[V_Len + I_Len++] = ',';
     
     memcpy(&SD_Power_Array[V_Len + I_Len],&PowerText[CHARS_IN_LINE*Line4 + 4],mA_Len);
     SD_Power_Array[V_Len + I_Len + mA_Len++] = '\n';
     SD_Power_Array[V_Len + I_Len + mA_Len] = '\0';
     
     SdAppendData(&fil,FileName,SD_Power_Array,V_Len + I_Len + mA_Len);
   }
   SD_Time = HAL::GetTick() - Millis;
   
#endif  
    
  }

void PowerMonitor_UI()
{ 
  INA219_Dev.Run(&Power); 
  
  rtc.GetTime((char*)&TimeText[0]);
    
  rtc.GetDate((char*)&DateText[0]);  
  DateText[13] = ' ';
  
  V_Len = ftoa(Power.Voltage, (char*)&PowerText[CHARS_IN_LINE*Line2 + 4],2,' ');    
  
  I_Len = ftoa(Power.Current, (char*)&PowerText[CHARS_IN_LINE*Line3 + 4], 1,' '); 
  
  if(Power.Current > 0.1)
  { 
    if(PrevTimerCount != rtc.ReadTimeCounter())
    {
      PrevTimerCount = rtc.ReadTimeCounter();
      mA_Len = ftoa(Power.mAH, (char*)&PowerText[CHARS_IN_LINE*Line4 + 4], 2,' ');    
      rtc.CountertoTimeStr(TimerCount++,(char*)&PowerText[CHARS_IN_LINE*Line1 + 4]);
      PowerText[CHARS_IN_LINE*Line1 + 12] = ' ';
      
      // Store current values to RTC NV RAM
      rtc.WriteBackupReg(RTC_POWER_TIMER_COUNT,TimerCount);
      rtc.WriteBackupReg(RTC_POWER_MAH_VALUE,(uint32_t)Power.mAH);
    }
  }
  //intToStr((uint32_t)adc0.ReadVoltage(), (char*)&PowerText[CHARS_IN_LINE*Line7 + 5], 4); 
}

void ResetPowerData()
{
  TimerCount = 0;
  Power.mAH = 0;
  memset(&PowerText[CHARS_IN_LINE*Line4 + 4],' ', 9);
}

void PowerOutputUART()
{
  PowerText[CHARS_IN_LINE*Line2 + 10] = 0;
  printf("%s",&PowerText[CHARS_IN_LINE*Line2]);
  PowerText[CHARS_IN_LINE*Line2 + 10] = ' ';
  
  PowerText[CHARS_IN_LINE*Line3 + 8] = '\n';
  PowerText[CHARS_IN_LINE*Line3 + 9] = '\r';
  PowerText[CHARS_IN_LINE*Line3 + 10] = 0;
  printf("%s",&PowerText[CHARS_IN_LINE*Line3]);
  PowerText[CHARS_IN_LINE*Line3 + 8] = ' ';
  PowerText[CHARS_IN_LINE*Line3 + 9] = ' ';
  PowerText[CHARS_IN_LINE*Line3 + 10] = ' ';
}

#if SD_CARD

void BuildName(char* Name)
{
  if(Name)
  {
    memcpy(Name, "Power", 5);
    Name[5] = '_';
    rtc.GetTime(&Name[6]);
    Name[8] = '_';
    Name[11] = '_';
    Name[14] = '_';
    rtc.GetDate(&Name[15]);
    Name[28] = '.';
    Name[29] = 't';
    Name[30] = 'x';
    Name[31] = 't';
    Name[32] = 0;
  }
}
bool SdStart(FIL* fil, const char* fileName, uint8_t fileoptions)
{  
  if( (fileName!= nullptr) && (fil != nullptr) )
  {
    if (f_mount(&fs, "", 1) == FR_OK) // 1861648
    {      
      if(f_open(fil, fileName,fileoptions) == FR_OK) 
      {        
        return true;
      }      
    }
  } 
 return false;  
}

bool SdAppendData(FIL* fil, const char* FileName, char* pData, uint32_t len)
{
  uint32_t Writtenlen;
  if( (fil != nullptr) && (FileName != nullptr) && (pData != nullptr) )
  {
    /* Seek to end of the file to append data */
    if( f_lseek(fil, f_size(fil)) == FR_OK)
    {  
       if(f_write(fil,pData,len,&Writtenlen) == FR_OK)
       {
         return true;
       }
    }
    f_close(fil);      
  }
  return false;
}

bool SdStop(FIL* fil)
{  
  if(fil != nullptr)
  {
    if(f_close(fil) == FR_OK) // 1143384
    {      
      if(f_mount(0, "", 0) == FR_OK)  // 117
      {        
        return true;
      }      
    }
  } 
 return false;  
}

bool SD_Sucess;
void SdOnOffCallback(uint32_t UpdatedSettingValue)
{
    if(UpdatedSettingValue)
    {
      BuildName(FileName);
      SD_Sucess = SdStart(&fil,FileName,FA_WRITE | FA_OPEN_ALWAYS);
      SdCardState = 1;
      //PowerSettings.bits.SdCardState = 1;
    }
    else
    {
      SD_Sucess = SdStop(&fil);
      SdCardState = 0;
      //PowerSettings.bits.SdCardState = 0;
    } 
    //rtc.WriteBackupReg(RTC_POWER_SETTINGS,PowerSettings.Word16);
}
#endif

void UartStatefx(uint32_t UpdatedSettingValue)
{
  UartState = UpdatedSettingValue;
  PowerSettings.bits.UartState = UpdatedSettingValue?1:0;
  rtc.WriteBackupReg(RTC_POWER_SETTINGS,PowerSettings.Word16);
  //Flash_Write_Data(UART_STATE_ADDRESS,&UartState,1);
}

void UartDelayfx(uint32_t UpdatedSettingValue)
{
  UartSampleTime = UpdatedSettingValue;
  rtc.WriteBackupReg(RTC_POWER_LOG_INTERVAL,UpdatedSettingValue);
  //Flash_Write_Data(UART_DELAY_ADDRESS,&UartSampleTime,1);
}

void UartBaudratefx(uint32_t UpdatedSettingValue)
{
  //Decode Baudrate
  UartBr = UpdatedSettingValue; //UartBaudRateTable[UpdatedSettingValue%7];
  //Set Baudrate
  POWER_MONITOR_UART.SetBaudrate(UartBaudRateTable[UpdatedSettingValue%7]);
  //Encode Baudrate
  PowerSettings.bits.UartBaudrate = UpdatedSettingValue%7;
  //Store Baudrate
  rtc.WriteBackupReg(RTC_POWER_SETTINGS,PowerSettings.Word16);
}

void RtcTimeSetfx(uint32_t UpdatedSettingValue)
{
  uint8_t year,month,day,hour,min,sec;
  
  // Extract date from RTC_date
  year = (uint8_t)(RTC_date/10000);
  month = (uint8_t)((RTC_date%10000)/100);
  day = (uint8_t)(RTC_date%100);
  
  // Extract time from RTC_date
  hour = (uint8_t)(UpdatedSettingValue/10000);
  min = (uint8_t)((UpdatedSettingValue%10000)/100);
  sec = (uint8_t)(UpdatedSettingValue%100);
  
  rtc.Set(2000+year,month,day,hour,min,sec);
  //rtc.WriteBackupReg(RTC_COUNTER_SET,rtc.ReadTimeCounter);
}

void DisplayBrightness(uint32_t brightness)
{
  if(brightness == 0) brightness = 1;
  Dimmer.SetDutyCycle(brightness); //1-24.6mv 2-52.5 10-276mv  50-1.39v 99 - 2.75v 100 2.77v
}

void Ina219ModeFx(uint32_t UpdatedSettingValue)
{
  if(UpdatedSettingValue == 0)
  {
    INA219_Dev.SetCalibration_16V_400mA();
  }
  else if(UpdatedSettingValue == 1)
  {
    INA219_Dev.SetCalibration_32V_2A();
  }
}

void AddSecondsFx(uint32_t UpdatedSettingValue)
{
  uint32_t TotalCounts;  
  TotalCounts = rtc.ReadTimeCounter() + UpdatedSettingValue;
  rtc.SetCounter(TotalCounts);  
}

void SubSecondsFx(uint32_t UpdatedSettingValue)
{
  uint32_t TotalCounts;  
  TotalCounts = rtc.ReadTimeCounter() - UpdatedSettingValue;
  rtc.SetCounter(TotalCounts);  
}

void RestoreSettings()
{
  if( rtc.ReadBackupReg(RTC_BKP_ACCESS_CODE) != 0xBABE) return;
  
  // Retore previous values to RTC NV RAM
  TimerCount = rtc.ReadBackupReg(RTC_POWER_TIMER_COUNT);
  Power.mAH  = rtc.ReadBackupReg(RTC_POWER_MAH_VALUE);
  
  PowerSettings.Word16 = rtc.ReadBackupReg(RTC_POWER_SETTINGS);
  UartState   = PowerSettings.bits.UartState;
  //SdCardState = PowerSettings.bits.SdCardState;
  
  //Restore Baudrate
  UartBr = PowerSettings.bits.UartBaudrate;
  //Set Baudrate
  POWER_MONITOR_UART.SetBaudrate(UartBaudRateTable[PowerSettings.bits.UartBaudrate]);
  
  UartSampleTime = rtc.ReadBackupReg(RTC_POWER_LOG_INTERVAL);
  SdSampleTime   = rtc.ReadBackupReg(RTC_POWER_LOG_INTERVAL);
   
#if 0
  // Retore uart state from flash
  UartState = Flash_Read32(UART_STATE_ADDRESS);
  
  // Retore uart state from flash
  UartSampleTime = Flash_Read32(UART_DELAY_ADDRESS);
  
  // Retore uart baudrate from flash
  UartBr = Flash_Read32(UART_BAUD_RATE_ADDRESS);
  POWER_MONITOR_UART.SetBaudrate(UartBr);
#endif
  
}
