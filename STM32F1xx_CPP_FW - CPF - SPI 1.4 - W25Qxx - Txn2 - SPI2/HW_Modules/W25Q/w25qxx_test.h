/******************
** FILE: w25qxx_test.h
**
******************/

//#include"HAL_Objects.h"
#include "w25qxx.h"

#ifndef w25qxx_test_h
#define w25qxx_test_h

extern HAL::Spi spi1;
extern HAL::Spi spi2;
volatile bool XferDone = false;

class w25qxxCallback: public Callback
{
  public:
  virtual void CallbackFunction()
  {
    XferDone = true;
  }
};

w25qxxCallback w25qxxCb;

enum 
{
  w25qxx_POLL,
  w25qxx_INTR,
  w25qxx_DMA,
  w25qxx_OLD,
};

static uint8_t TxBuf[260],RxBuf[256];

void w25qxx_Test()
{
#if 0
  BSP::w25qxx<&spi2,B12> w25qxxDev;
  HAL::DigitalOut<A8> HoldPin;
#else
  BSP::w25qxx<&spi1,A4> w25qxxDev;
#endif
  
  static bool InitDone;
  uint8_t test_id = 0;
  HAL::DigitalOut<A3> A3Pin;

  if(InitDone == false)
  {
#if 0
    HoldPin.HwInit();
    HoldPin.High();
#endif
    w25qxxDev.HwInit();
    A3Pin.HwInit();
    for(uint16_t i = 0; i<sizeof(TxBuf); i++) TxBuf[i] = 0x9B;
    for(uint16_t i = 0; i<sizeof(RxBuf); i++) RxBuf[i] = 0;
    
    InitDone = true;
  }
  
  test_id = w25qxx_INTR;
  while(1)
  {
    
    switch(test_id)
    { 
    case w25qxx_POLL:
      w25qxxDev.EraseSector(0); //39.76ms
      XferDone = false;
      while( w25qxxDev.PageWrite(1,TxBuf) == BSP::w25qxx<&spi1,A4>::W25QXX_BUSY);  
      XferDone = false;
      while( w25qxxDev.PageRead(1,RxBuf) == BSP::w25qxx<&spi1,A4>::W25QXX_BUSY);
      test_id = 255;
      break; 
    case w25qxx_INTR:
#if 1
      w25qxxDev.EraseSector(0);
      XferDone = false;
      while( w25qxxDev.PageWrite(2,TxBuf,&w25qxxCb) == BSP::w25qxx<&spi1,A4>::W25QXX_BUSY); 
      while(!XferDone);  
#endif
      XferDone = false;
      while( w25qxxDev.PageRead(2,RxBuf,&w25qxxCb) == BSP::w25qxx<&spi1,A4>::W25QXX_BUSY);
      while(!XferDone);
      test_id = 255;
      break;
    case w25qxx_DMA:
      A3Pin.Toggle();
      w25qxxDev.EraseSector(0);
      A3Pin.Toggle();
      w25qxxDev.WritePage(TxBuf,0x400,0,256);   
      A3Pin.Toggle();
      w25qxxDev.ReadPage(RxBuf,0x400,0,256);
      A3Pin.Toggle();
      test_id = 255;
      break; 
  case w25qxx_OLD:
      A3Pin.Toggle();
      w25qxxDev.EraseSector(0);
      A3Pin.Toggle();
      w25qxxDev.WritePage(TxBuf,0,0,256,&w25qxxCb);   
      A3Pin.Toggle();
     w25qxxDev.ReadPage(RxBuf,0,0,256,&w25qxxCb);
      A3Pin.Toggle();
      test_id = 255;
      break; 
      
    default: break;      
    }
    memset(RxBuf,0,sizeof(RxBuf));
    LL_mDelay(3000);    
    
  }
  
}


#endif // w25qxx_test_h