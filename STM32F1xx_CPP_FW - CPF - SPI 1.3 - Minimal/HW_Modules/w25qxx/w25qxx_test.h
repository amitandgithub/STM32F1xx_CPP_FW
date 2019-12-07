/******************
** FILE: w25qxx_test.h
**
******************/

//#include"HAL_Objects.h"
#include "w25qxx.h"

#ifndef w25qxx_test_h
#define w25qxx_test_h

void w25qxx_Test()
{
  
  static bool InitDone;
  uint8_t test_id = 0,TxBuf[256],RxBuf[256];
  HAL::DigitalOut<A3> A3Pin;
  if(InitDone == false)
  {
    W25qxx_Init();
    A3Pin.HwInit();
    for(uint16_t i = 0; i<sizeof(TxBuf); i++) TxBuf[i] = i;
    for(uint16_t i = 0; i<=sizeof(RxBuf); i++) RxBuf[i] = 0;
    
    InitDone = true;
  }
  
  
  while(1)
  {
    
    switch(test_id)
    { 
    case 0:
      A3Pin.Toggle();
      W25qxx_EraseSector(0);
      A3Pin.Toggle();
      W25qxx_WritePage(TxBuf,0x400,0,256);   
      A3Pin.Toggle();
      W25qxx_ReadPage (RxBuf,0x400,0,256);
      A3Pin.Toggle();
      break; 
    case 1:
      A3Pin.Toggle();
      W25qxx_EraseSector(0);
      A3Pin.Toggle();
      writePageIntr(0,TxBuf);
      A3Pin.Toggle();
      W25qxx_ReadPage (RxBuf,0,0,256);
      A3Pin.Toggle();
    case 2:
      break;  
      
    }
    LL_mDelay(3000);    
    
  }
  
}


#endif // w25qxx_test_h