
#ifndef __CPP_HAL_H__
#define __CPP_HAL_H__

#include"CPP_HAL.h"

#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

#if USB_DEVICE



uint8_t Buf[20] = "Amit Chaudhary\n\r"; // len - 16

static void MX_GPIO_Init(void);
uint32_t Debug_Write(uint8_t* buf, uint16_t len);
void Parse_Command(uint8_t *buf, uint32_t len);
void CDC_ReceiveCallback(uint8_t *buf, uint32_t len);
extern void usb_device_init();

void CDC_ReceiveCallback(uint8_t *buf, uint32_t len)
{
  Parse_Command(buf,len);
}


void Parse_Command(uint8_t *buf, uint32_t len)
{
  if(buf == NULL || len ==0 ) return;
  
  switch(buf[0])
  {
  case 'A': Debug_Write((uint8_t*)"Amit\n\r",6); break;
  case 'a': Debug_Write((uint8_t*)"Avni\n\r",6); break;
  case 's': Debug_Write((uint8_t*)"Shiwani\n\r",9); break;
  default:  Debug_Write((uint8_t*)"??\n\r",4); break;
  }  
}


uint32_t Debug_Write(uint8_t* buf, uint16_t len)
{
  CDC_Transmit_FS(buf,len);
  return 0;
}

void usb_device_init()
{  
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  MX_USB_DEVICE_Init();  
}

#endif


#endif
