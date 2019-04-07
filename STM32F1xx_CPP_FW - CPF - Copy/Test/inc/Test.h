
#include<cstdint>
#include<stddef.h>
#include<stdio.h>
#include"stm32f1xx_hal.h"
#include"GpioOutput.h"
#include"Led.h"
#include"GpioInput.h"
#include"HwButton.h"
#include"BtnPoll.h"
#include"RotaryEncoderPoll.h"
#include"RotaryEncoderIntr.h"
#include"Callback.h"
#include"BtnInt.h"

#include "CPP_HAL.h"
using namespace HAL;


extern void Gpio_Output_Test();
extern void Led_Test();
extern void Button_ISR();
extern void Gpio_Intput_Test();
extern void Gpio_Intr_Intput_Test();
extern void LED_Toggle();
extern void Hw_Button_Test();
extern void Hw_ButtonIntr_Test();
extern void Hw_BtnPoll_Test();
extern void Btn_Low_Handler();
extern void Btn_High_Handler();
extern void btn_A1_High_Handler();
extern void btn_A1_Low_Handler();
extern void Rotary_Encoder_Test();
extern void CW_Handler();
extern void CCW_Handler();
extern void Rising();
extern void Falling();
extern void BtnInt_Test();
extern void Rotary_Encoder_Intr_Test();

extern void I2C_Master_Test();
extern void I2C_Slave_Test();
extern void INA219_Test();
extern void SPI_Poll_Test();
extern void SPI_Int_Test();
extern void SPI1_DMA_Test();
extern void SPI2_DMA_Test();
extern void SPI1_DMA_Tx_Complete_Callback();
extern void SPI2_DMA_Tx_Complete_Callback();
extern void Nokia5110LCD_SPI1_Test();
extern void Nokia5110LCD_SPI2_Test(); 
extern void Nokia5110LCD_Dual_Test();
extern void Queue_Test();
extern void Nokia5110LCD_SPI1_Dual_Display_Test();
extern void RTC_Test();
extern void Printf_Test();
extern void UI_Test();
extern void ClickEvent(void);
extern void LongPressEvent(void);
extern void LongLongPressEvent(void);
extern void Display_Buffer_Test();
extern void ScreenTouchHandler(void);
extern void ScreenLongTouchHandler(void);
extern void SD_Test();
extern void SD1_Test();
