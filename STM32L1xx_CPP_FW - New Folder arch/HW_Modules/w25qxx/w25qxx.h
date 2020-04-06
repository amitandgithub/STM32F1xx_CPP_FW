/******************
** CLASS: w25qxx
**
** DESCRIPTION:
**  Implements the w25qxx driver
**
** CREATED: 8/11/2019, by Amit Chaudhary
**
** FILE: m_DevInfo.h
**
******************/
#ifndef w25qxx_h
#define w25qxx_h

#include"chip_peripheral.h"
#include"CPP_HAL.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Spi.h"
#include"GpioOutput.h"

#define _W25QXX_DEBUG 1
#define W25QXX_DUMMY_BYTE         0xA5
#define	W25qxx_Delay(delay)		LL_mDelay(delay)

namespace BSP
{   
  //template<w25qxx_TEMPLATE_PARAMS>
  class w25qxx
  {
    
  public:
    
    typedef enum
    {
      W25Q10=1,
      W25Q20,
      W25Q40,
      W25Q80,
      W25Q16,
      W25Q32,
      W25Q64,
      W25Q128,
      W25Q256,
      W25Q512,
      
    }W25QXX_ID_t;
    
    typedef struct
    {
      W25QXX_ID_t	ID;
      uint8_t     UniqID[8];      
      uint16_t    PageSize;
      uint32_t    PageCount;
      uint32_t    SectorSize;
      uint32_t    SectorCount;
      uint32_t    BlockSize;
      uint32_t    BlockCount;      
      uint32_t    CapacityInKiloByte;      
      uint8_t     StatusRegister1;
      uint8_t     StatusRegister2;
      uint8_t     StatusRegister3;      
      uint8_t     Lock;      
    }w25qxx_t;
    
    typedef enum : uint8_t
    {
      W25QXX_SUCCESS,
      W25QXX_ERROR,
      W25QXX_BUSY,
    } Status_t;
    
    using Transaction_t = HAL::Spi::Transaction_t;
    
    using Callback_t = HAL::Spi::SPICallback_t;
    
    w25qxx(HAL::Spi*   pSpiDriver, Port_t CSPort, PIN_t CSPin);
    
    uint8_t SpiTxRxByte(uint8_t Data);
    
    uint32_t ReadID(void);
    
    void ReadUniqID(void);
    
    void WriteEnable(void);
    
    void WriteDisable(void);
    
    uint8_t ReadStatusRegister(uint8_t SelectStatusRegister_1_2_3);
    
    void WriteStatusRegister(uint8_t	SelectStatusRegister_1_2_3,uint8_t Data);
    
    void WaitForWriteEnd(void);
    
    void EraseChip(void);
    
    void EraseSector(uint32_t SectorAddr);
    
    void WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte = 0, uint32_t NumByteToWrite_up_to_PageSize = 256, HAL::Spi::spiCallback* Callback = nullptr );
    
    void ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte = 0, uint32_t NumByteToRead_up_to_PageSize = 256, HAL::Spi::spiCallback* Callback = nullptr );      
    
    bool HwInit();
    
    Status_t PageWrite(uint32_t Page_Address, uint8_t* pBuffer, HAL::Spi::spiCallback* Callback = nullptr, uint16_t len = 256);
    
    Status_t PageRead(uint32_t Page_Address, uint8_t* pBuffer, HAL::Spi::spiCallback* Callback = nullptr, uint16_t len = 256);
    
    Status_t PageWriteDMA(uint32_t Page_Address, uint8_t* pBuffer, HAL::Spi::spiCallback* Callback = nullptr, uint16_t len = 256);
    
    Status_t PageReadDMA(uint32_t Page_Address, uint8_t* pBuffer, HAL::Spi::spiCallback* Callback = nullptr, uint16_t len = 256);
    
    HAL::GpioOutput m_CSPin;
  public: 
    HAL::Spi*   m_SpiDriver;
    w25qxx_t  m_DevInfo;
  };
  
}



#endif