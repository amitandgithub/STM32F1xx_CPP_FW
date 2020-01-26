

#include"Power_Monitor.h"

#include "Nokia5110LCD.h"
#include "NokiaLCD.h"




//namespace App
//{

    
/* Globals for Current Sensor task*/
#define INA219_ADDR 0x80U
INA219::Power_t Power;
char PowerStr[25];
static NokiaLCD NokiaLCDObj1(&spi1,C5,B0,C4,C3);


char TimeStr[15];

/* Globals for Touch Button task*/
BSP::HwButton B9_HwBtnInt(B9);

ClickCallback ClickCb;

LongPressCallback LongPressCb;

LongLongPressCallback LongLongPressCb;

/* Globals for UI task*/
Screen::Event_t gEvent = Screen::MaxEvents;
BSP::UI MyUI(&NokiaLCDObj1);



#ifdef SD_CARD
/* Globals for SD_CARD task*/
#define SD_CARD_SAVE_FREQ 100
Peripherals::GpioOutput CS2(GPIOB,GPIO_PIN_12,Peripherals::GpioOutput::AF_PP, Peripherals::GpioOutput::HIGH, Peripherals::GpioOutput::PULL_UP);
Peripherals::SPI_Poll SPI2_Obj(Peripherals::SPI_Poll::SPI2_B12_B13_B14_B15, &CS2, 25000000);    
SD SD_Card(&SPI2_Obj,&CS2);
FATFS fs;
FIL fil;
FRESULT fr;
char FileName[26];
#endif

Screen HomeScreen( (char *)
                   "T = 00:00:00  "
                   "I =           "
                   "V =           "
                   "C =           "
                   "P =           "
                   "              "
                   );
ControlScreen Menu( (char *)
                   "Live Power    "
                   "Reset         "
                   "SD Card       "
                   "SD Save       "
                   "Back Light Off"
                   "              "
                   );

void Init_Power_Monitor()
{        

    rtc.HwInit();
    
    B9_HwBtnInt.HwInit(INPUT_PULLDOWN,NO_INTERRUPT,&ClickCb,&LongPressCb,&LongLongPressCb); //reinterpret_cast <Callback *>(&BtnOnRiseCallbackObj),reinterpret_cast <Callback *>(&BtnOnFallCallbackObj)
    
    INA219_Dev.HwInit(); 
    INA219_Dev.SetCalibration_32V_2A();
    
    MyUI.Init();
    MyUI.AddScreen(&HomeScreen);
    HomeScreen.SetScreenHandler(ScreenTouchHandler,Reset_ScreenTouchHandler,nullptr);
    MyUI.AddScreen(&Menu);
    Menu.SetScreenHandler(ScreenTouchHandler,ScreenLongTouchHandler,nullptr);
    Menu.AddHandler( 0, LivePower_ScreenTouchHandler,   nullptr);
    Menu.AddHandler( 1, Reset_ScreenTouchHandler,       nullptr);
    Menu.AddHandler( 2, SD_CARD_ScreenTouchHandler,     nullptr);
    Menu.AddHandler( 3, SD_Save_ScreenTouchHandler,     nullptr);
    Menu.AddHandler( 4, BackLight_ScreenTouchHandler,   nullptr);
    
    
#ifdef SD_CARD
    SD_Card.HwInit();
    
    mem_cpy(&FileName[0], "Power", 5);
    FileName[5] = '_';
    FileName[6] = __DATE__[0];
    FileName[7] = __DATE__[1];
    FileName[8] = __DATE__[2];
    FileName[9] = '_';
    FileName[10] = __DATE__[4];
    FileName[11] = __DATE__[5];
    FileName[12] = '_';
    FileName[13] = __TIME__[0];
    FileName[14] = __TIME__[1];
    FileName[15] = '_';
    FileName[16] = __TIME__[3];
    FileName[17] = __TIME__[4];
    FileName[18] = '_';
    FileName[19] = __TIME__[6];
    FileName[20] = __TIME__[7];  
    FileName[21] = '.';
    FileName[22] = 't';
    FileName[23] = 'x';
    FileName[24] = 't';
    FileName[25] = 0;
#endif   
}

float mWH;
void Run_Power_Monitor()
{    
    int len_I,len_V,len_C,len_W;
    
    rtc.GetTime(TimeStr);
   // HomeScreen.DrawStr(DATE_ROW,0,TimeStr);

    HomeScreen.DrawStr(TIMER_ROW,4,TimeStr);
    
    B9_HwBtnInt.Run();    
    
    INA219_Dev.Run(&Power);

    len_I = ftoa(Power.Current, &PowerStr[0], 1);
    HomeScreen.DrawStr(I_ROW,4,&PowerStr[0],len_I);
    HomeScreen.DrawStr(I_ROW, 4 + len_I, "mA             ",10 - len_I );
    PowerStr[len_I++] = ',';
    
    len_V = ftoa(Power.Voltage, &PowerStr[len_I], 2);
    HomeScreen.DrawStr(V_ROW,4,&PowerStr[len_I],len_V);
    HomeScreen.DrawStr(V_ROW, 4 + len_V, "V             ",10 - len_V);
    PowerStr[len_I + len_V++] = ',';
    
    len_C = ftoa(Power.mAH, &PowerStr[len_I + len_V], 2);
    HomeScreen.DrawStr(C_ROW,4,&PowerStr[len_I + len_V]);
    HomeScreen.DrawStr(C_ROW, 4 + len_C, "mAH           ",10 - len_C);    
    PowerStr[len_I + len_V + len_C++] = ',';
    
    len_W = ftoa(mWH, &PowerStr[len_I + len_V + len_C], 2);
    HomeScreen.DrawStr(W_ROW,4,&PowerStr[len_I + len_V + len_C]);
    HomeScreen.DrawStr(W_ROW, 4 + len_W, "WH");    
    PowerStr[len_I + len_V + len_C + len_W++] = '\n';

    MyUI.EventHamdler(gEvent);
    
    MyUI.Run();

#ifdef SD_CARD
    Save_To_SD_Card(FileName,&PowerStr[0],len_I + len_V + len_C + len_W);
#endif
}


void HwButton_ClickEvent()
{
    gEvent = Screen::Touch;
}

void HwButton_LongPressEvent()
{
    gEvent = Screen::LongTouch;
}

void HwButton_LongLongPressEvent()
{
    gEvent = Screen::LongLongTouch;
}

void ScreenTouchHandler()
{
	BSP::UI::GoToNextScreen();
}

void ScreenLongTouchHandler()
{
	BSP::UI::GoToPreviousScreen();
}

void LivePower_ScreenTouchHandler()
{
	BSP::UI::GoToScreen(&HomeScreen);
}

void Reset_ScreenTouchHandler()
{
	Reset_Power_Readings();
}
void SD_CARD_ScreenTouchHandler()
{
	
}
void SD_Save_ScreenTouchHandler()
{
	
}
void BackLight_ScreenTouchHandler()
{
    if(NokiaLCDObj1.GetBackLightState())
    {
        NokiaLCDObj1.BackLightOFF();
        Menu.DrawStr(BACK_LIGHT_ROW,11,"OFF",3,0x0D);
    }
    else
    {
        NokiaLCDObj1.BackLightON();
        Menu.DrawStr(BACK_LIGHT_ROW,11,"ON ",3,0x0D);
    }
}


void Reset_Power_Readings()
{
    Power.mAH = 0;
    mWH = 0;
    rtc.HwInit();    
}

#ifdef SD_CARD

bool Save_To_SD_Card(const char* FileName, char* pData, uint32_t len)
{ 
#ifndef SD_CARD_SAVE_FREQ_MAX
    
    static uint32_t Previous_Ticks;
    uint32_t Current_Ticks = HAL_GetTick()  ;
    
    if( Current_Ticks < Previous_Ticks + SD_CARD_SAVE_FREQ ) 
        return false;
    
    Previous_Ticks = Current_Ticks;
#endif
    
    if(pData == nullptr)
        return false;
    
    // Mark the end of the string to be saved on SD card
    pData[len] = '\0';
    
    fr = SD_Card.mount(&fs, "", 1); // 1861648
	if (fr != FR_OK) 
	{
		return false;
	}
    fr = SD_Card.open(&fil, FileName, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) 
	{
		/* Seek to end of the file to append data */
		fr = f_lseek(&fil, f_size(&fil)); 
        
		if (fr != FR_OK) SD_Card.close(&fil);
                    
		SD_Card.print(&fil,pData);
        
		SD_Card.close(&fil); // 1143384

		
		fr = SD_Card.mount(0, "", 0); // 117
		if (fr != FR_OK) 
		{
			return false;
		}
        return true;
    }
    return false;
    
    
}
#endif

bool Time_Elapsed(uint32_t Freq, uint32_t CurrentTicks,uint32_t *pPreviousTicks)
{
    if(CurrentTicks > Freq + *pPreviousTicks)
    {
        *pPreviousTicks = CurrentTicks;
        return true;
    }    
    return false;
}


//}
