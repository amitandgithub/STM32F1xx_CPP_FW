/******************
** FILE: w25qxx_test.h
**
******************/

//#include"HAL_Objects.h"
#include "w25qxx.h"

#ifndef w25qxx_test_h
#define w25qxx_test_h

extern HAL::Spi spi1;

enum 
{
  w25qxx_POLL,
  w25qxx_INTR,
  w25qxx_DMA,
};

void w25qxx_Test()
{
  BSP::w25qxx<&spi1,A4> w25qxxDev;
  
  static bool InitDone;
  uint8_t test_id = 0,TxBuf[256],RxBuf[256];
  HAL::DigitalOut<A3> A3Pin;
  if(InitDone == false)
  {
    w25qxxDev.HwInit();
    //    W25qxx_Init();
    A3Pin.HwInit();
    for(uint16_t i = 0; i<sizeof(TxBuf); i++) TxBuf[i] = i;
    for(uint16_t i = 0; i<=sizeof(RxBuf); i++) RxBuf[i] = 0;
    
    InitDone = true;
  }
  
  
  while(1)
  {
    
    switch(test_id)
    { 
    case w25qxx_POLL:
      A3Pin.Toggle();
      w25qxxDev.EraseSector(0);
      A3Pin.Toggle();
      w25qxxDev.WritePage(TxBuf,0x400,0,256);   
      A3Pin.Toggle();
      w25qxxDev.ReadPage (RxBuf,0x400,0,256);
      A3Pin.Toggle();
      test_id = 255;
      break; 
    case w25qxx_INTR:
      A3Pin.Toggle();
      w25qxxDev.EraseSector(0);
      A3Pin.Toggle();
      w25qxxDev.WritePage(TxBuf,0x400,0,256);   
      A3Pin.Toggle();
      w25qxxDev.ReadPage (RxBuf,0x400,0,256);
      A3Pin.Toggle();
      test_id = 255;
      break;
    case w25qxx_DMA:
      A3Pin.Toggle();
      w25qxxDev.EraseSector(0);
      A3Pin.Toggle();
      w25qxxDev.WritePage(TxBuf,0x400,0,256);   
      A3Pin.Toggle();
      w25qxxDev.ReadPage (RxBuf,0x400,0,256);
      A3Pin.Toggle();
      test_id = 255;
      break; 
    default: break;      
    }
    memset(RxBuf,0,256);
    LL_mDelay(3000);    
    
  }
  
}


#endif // w25qxx_test_h