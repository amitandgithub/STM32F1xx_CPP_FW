/******************
** FILE: w25qxx.cpp
**
** DESCRIPTION:
**  w25qxx implementation
**
** CREATED: 8/12/2019, by Amit Chaudhary
******************/

#include "w25qxx.h"

namespace BSP
{
  
  
  
  w25qxx::w25qxx(HAL::Port_t Port, HAL::PIN_t Pin, HAL::Spi* SpiDev) : m_SpiDev(SpiDev)
  {
    
    
    
  }
  
  bool w25qxx::HwInit()
  {
    spi1.HwInit();
  m_CSPin.HwInit();
  w25qxx.Lock=1;	
  while(HAL::GetTick()<100)
    W25qxx_Delay(1);
  m_CSPin.High();
  W25qxx_Delay(100);
  uint32_t	id;
#if (_W25QXX_DEBUG==1)
  printf("w25qxx Init Begin...\r\n");
#endif
  id=W25qxx_ReadID();
  
#if (_W25QXX_DEBUG==1)
  printf("w25qxx ID:0x%X\r\n",id);
#endif
  switch(id&0x0000FFFF)
  {
  case 0x401A:	// 	w25q512
    w25qxx.ID=W25Q512;
    w25qxx.BlockCount=1024;
#if (_W25QXX_DEBUG==1)
    printf("w25qxx Chip: w25q512\r\n");
#endif
    break;
  case 0x4019:	// 	w25q256
    w25qxx.ID=W25Q256;
    w25qxx.BlockCount=512;
#if (_W25QXX_DEBUG==1)
    printf("w25qxx Chip: w25q256\r\n");
#endif
    break;
  case 0x4018:	// 	w25q128
    w25qxx.ID=W25Q128;
    w25qxx.BlockCount=256;
#if (_W25QXX_DEBUG==1)
    printf("w25qxx Chip: w25q128\r\n");
#endif
    break;
  case 0x4017:	//	w25q64
    w25qxx.ID=W25Q64;
    w25qxx.BlockCount=128;
#if (_W25QXX_DEBUG==1)
    printf("w25qxx Chip: w25q64\r\n");
#endif
    break;
  case 0x4016:	//	w25q32
    w25qxx.ID=W25Q32;
    w25qxx.BlockCount=64;
#if (_W25QXX_DEBUG==1)
    printf("w25qxx Chip: w25q32\r\n");
#endif
    break;
  case 0x4015:	//	w25q16
    w25qxx.ID=W25Q16;
    w25qxx.BlockCount=32;
#if (_W25QXX_DEBUG==1)
    printf("w25qxx Chip: w25q16\r\n");
#endif
    break;
  case 0x4014:	//	w25q80
    w25qxx.ID=W25Q80;
    w25qxx.BlockCount=16;
#if (_W25QXX_DEBUG==1)
    printf("w25qxx Chip: w25q80\r\n");
#endif
    break;
  case 0x4013:	//	w25q40
    w25qxx.ID=W25Q40;
    w25qxx.BlockCount=8;
#if (_W25QXX_DEBUG==1)
    printf("w25qxx Chip: w25q40\r\n");
#endif
    break;
  case 0x4012:	//	w25q20
    w25qxx.ID=W25Q20;
    w25qxx.BlockCount=4;
#if (_W25QXX_DEBUG==1)
    printf("w25qxx Chip: w25q20\r\n");
#endif
    break;
  case 0x4011:	//	w25q10
    w25qxx.ID=W25Q10;
    w25qxx.BlockCount=2;
#if (_W25QXX_DEBUG==1)
    printf("w25qxx Chip: w25q10\r\n");
#endif
    break;
  default:
#if (_W25QXX_DEBUG==1)
    printf("w25qxx Unknown ID\r\n");
#endif
    w25qxx.Lock=0;	
    return false;

  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
}