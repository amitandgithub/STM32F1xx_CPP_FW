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
#include"DigitalIn.h"
#include"DigitalOut.h"

#define W25QXX_DUMMY_BYTE         0xA5
#define	W25qxx_Delay(delay)		LL_mDelay(delay)

namespace BSP
{ 
  
#define w25qxx_TEMPLATE_PARAMS HAL::Spi*   pSpiDriver, Port_t CSPort, PIN_t CSPin
#define w25qxx_TEMPLATE_T pSpiDriver,CSPort,CSPin
  
  template<w25qxx_TEMPLATE_PARAMS>
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
      
      
      
//      class w25qxxCallback : public HAL::Spi::spiCallback
//      {
//      public:
//        w25qxxCallback(w25qxx* This):_this(This){};
//        virtual void CallbackFunction(HAL::Spi::SpiStatus_t SPIStatus)
//        {
//          _this->m_CSPin.High();
//          if(_this->m_CurrentCallback)
//            _this->m_CurrentCallback->CallbackFunction();
//        }
//      private:
//        w25qxx* _this;
//      };
     
    public:      
      w25qxx_t  m_DevInfo;
      HAL::DigitalOut<CSPort,CSPin> m_CSPin;
      uint16_t CSPinmask;
      //HAL::Spi::spiCallback* m_CurrentCallback;
//      w25qxxCallback w25qxxCb;
    };
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    uint8_t w25qxx<w25qxx_TEMPLATE_T>::SpiTxRxByte(uint8_t Data)
    {
      uint8_t	data,ret;
      data = Data;
      //HAL_SPI_TransmitReceive(&_SpiTxRxByte,&Data,&ret,1,100);
      pSpiDriver->TxRxPoll(&data,&ret,1);
      return ret;	
    }
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    uint32_t w25qxx<w25qxx_TEMPLATE_T>::ReadID(void)
    {
      uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
      m_CSPin.Low();
      
      SpiTxRxByte(0x9F);
      Temp0 = SpiTxRxByte(W25QXX_DUMMY_BYTE);
      Temp1 = SpiTxRxByte(W25QXX_DUMMY_BYTE);
      Temp2 = SpiTxRxByte(W25QXX_DUMMY_BYTE);
      m_CSPin.High();
      Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
      return Temp;
    }
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    void w25qxx<w25qxx_TEMPLATE_T>::ReadUniqID(void)
    {
      m_CSPin.Low();
      SpiTxRxByte(0x4B);
      for(uint8_t	i=0;i<4;i++)
        SpiTxRxByte(W25QXX_DUMMY_BYTE);
      for(uint8_t	i=0;i<8;i++)
        m_DevInfo.UniqID[i] = SpiTxRxByte(W25QXX_DUMMY_BYTE);
      m_CSPin.High();
    }
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    void w25qxx<w25qxx_TEMPLATE_T>::WriteEnable(void)
    {
      m_CSPin.Low();
      SpiTxRxByte(0x06);
      m_CSPin.High();
      W25qxx_Delay(1);
    }
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    void w25qxx<w25qxx_TEMPLATE_T>::WriteDisable(void)
    {
      m_CSPin.Low();
      SpiTxRxByte(0x04);
      m_CSPin.High();
      W25qxx_Delay(1);
    }
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    uint8_t w25qxx<w25qxx_TEMPLATE_T>::ReadStatusRegister(uint8_t	SelectStatusRegister_1_2_3)
    {
      uint8_t	status=0;
      m_CSPin.Low();
      if(SelectStatusRegister_1_2_3==1)
      {
        SpiTxRxByte(0x05);
        status=SpiTxRxByte(W25QXX_DUMMY_BYTE);	
        m_DevInfo.StatusRegister1 = status;
      }
      else if(SelectStatusRegister_1_2_3==2)
      {
        SpiTxRxByte(0x35);
        status=SpiTxRxByte(W25QXX_DUMMY_BYTE);	
        m_DevInfo.StatusRegister2 = status;
      }
      else
      {
        SpiTxRxByte(0x15);
        status=SpiTxRxByte(W25QXX_DUMMY_BYTE);	
        m_DevInfo.StatusRegister3 = status;
      }	
      m_CSPin.High();
      return status;
    }
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    void w25qxx<w25qxx_TEMPLATE_T>::WriteStatusRegister(uint8_t	SelectStatusRegister_1_2_3,uint8_t Data)
    {
      m_CSPin.Low();
      if(SelectStatusRegister_1_2_3==1)
      {
        SpiTxRxByte(0x01);
        m_DevInfo.StatusRegister1 = Data;
      }
      else if(SelectStatusRegister_1_2_3==2)
      {
        SpiTxRxByte(0x31);
        m_DevInfo.StatusRegister2 = Data;
      }
      else
      {
        SpiTxRxByte(0x11);
        m_DevInfo.StatusRegister3 = Data;
      }
      SpiTxRxByte(Data);
      m_CSPin.High();
    }
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    void w25qxx<w25qxx_TEMPLATE_T>::WaitForWriteEnd(void)
    {
      W25qxx_Delay(1);
      m_CSPin.Low();
      SpiTxRxByte(0x05);
      do
      {
        m_DevInfo.StatusRegister1 = SpiTxRxByte(W25QXX_DUMMY_BYTE);
        W25qxx_Delay(1);
      }
      while ((m_DevInfo.StatusRegister1 & 0x01) == 0x01);
      m_CSPin.High();
    }  
  
  //###################################################################################################################  
  template<w25qxx_TEMPLATE_PARAMS>
    bool w25qxx<w25qxx_TEMPLATE_T>::HwInit()
    {
      pSpiDriver->HwInit();
      m_CSPin.HwInit();
      CSPinmask = m_CSPin.GetPinmask();
      m_DevInfo.Lock=1;	
      while(HAL::GetTick()<100)
        W25qxx_Delay(1);
      m_CSPin.High();
      W25qxx_Delay(100);
      uint32_t	id;
#if (_W25QXX_DEBUG==1)
      printf("w25qxx Init Begin...\r\n");
#endif
      id=ReadID();
      
#if (_W25QXX_DEBUG==1)
      printf("w25qxx ID:0x%X\r\n",id);
#endif
      switch(id&0x0000FFFF)
      {
      case 0x401A:	// 	w25q512
        m_DevInfo.ID=W25Q512;
        m_DevInfo.BlockCount=1024;
#if (_W25QXX_DEBUG==1)
        printf("w25qxx Chip: w25q512\r\n");
#endif
        break;
      case 0x4019:	// 	w25q256
        m_DevInfo.ID=W25Q256;
        m_DevInfo.BlockCount=512;
#if (_W25QXX_DEBUG==1)
        printf("w25qxx Chip: w25q256\r\n");
#endif
        break;
      case 0x4018:	// 	w25q128
        m_DevInfo.ID=W25Q128;
        m_DevInfo.BlockCount=256;
#if (_W25QXX_DEBUG==1)
        printf("w25qxx Chip: w25q128\r\n");
#endif
        break;
      case 0x4017:	//	w25q64
        m_DevInfo.ID=W25Q64;
        m_DevInfo.BlockCount=128;
#if (_W25QXX_DEBUG==1)
        printf("w25qxx Chip: w25q64\r\n");
#endif
        break;
      case 0x4016:	//	w25q32
        m_DevInfo.ID=W25Q32;
        m_DevInfo.BlockCount=64;
#if (_W25QXX_DEBUG==1)
        printf("w25qxx Chip: w25q32\r\n");
#endif
        break;
      case 0x4015:	//	w25q16
        m_DevInfo.ID=W25Q16;
        m_DevInfo.BlockCount=32;
#if (_W25QXX_DEBUG==1)
        printf("w25qxx Chip: w25q16\r\n");
#endif
        break;
      case 0x4014:	//	w25q80
        m_DevInfo.ID=W25Q80;
        m_DevInfo.BlockCount=16;
#if (_W25QXX_DEBUG==1)
        printf("w25qxx Chip: w25q80\r\n");
#endif
        break;
      case 0x4013:	//	w25q40
        m_DevInfo.ID=W25Q40;
        m_DevInfo.BlockCount=8;
#if (_W25QXX_DEBUG==1)
        printf("w25qxx Chip: w25q40\r\n");
#endif
        break;
      case 0x4012:	//	w25q20
        m_DevInfo.ID=W25Q20;
        m_DevInfo.BlockCount=4;
#if (_W25QXX_DEBUG==1)
        printf("w25qxx Chip: w25q20\r\n");
#endif
        break;
      case 0x4011:	//	w25q10
        m_DevInfo.ID=W25Q10;
        m_DevInfo.BlockCount=2;
#if (_W25QXX_DEBUG==1)
        printf("w25qxx Chip: w25q10\r\n");
#endif
        break;
      default:
#if (_W25QXX_DEBUG==1)
        printf("w25qxx Unknown ID\r\n");
#endif
        m_DevInfo.Lock=0;	
        return false;
        
      }    
      
      m_DevInfo.PageSize=256;
      m_DevInfo.SectorSize=0x1000;
      m_DevInfo.SectorCount=m_DevInfo.BlockCount*16;
      m_DevInfo.PageCount=(m_DevInfo.SectorCount*m_DevInfo.SectorSize)/m_DevInfo.PageSize;
      m_DevInfo.BlockSize=m_DevInfo.SectorSize*16;
      m_DevInfo.CapacityInKiloByte=(m_DevInfo.SectorCount*m_DevInfo.SectorSize)/1024;
      ReadUniqID();
      ReadStatusRegister(1);
      ReadStatusRegister(2);
      ReadStatusRegister(3);
#if (_W25QXX_DEBUG==1)
      printf("w25qxx Page Size: %d Bytes\r\n",m_DevInfo.PageSize);
      printf("w25qxx Page Count: %d\r\n",m_DevInfo.PageCount);
      printf("w25qxx Sector Size: %d Bytes\r\n",m_DevInfo.SectorSize);
      printf("w25qxx Sector Count: %d\r\n",m_DevInfo.SectorCount);
      printf("w25qxx Block Size: %d Bytes\r\n",m_DevInfo.BlockSize);
      printf("w25qxx Block Count: %d\r\n",m_DevInfo.BlockCount);
      printf("w25qxx Capacity: %d KiloBytes\r\n",m_DevInfo.CapacityInKiloByte);
      printf("w25qxx Init Done\r\n");
#endif
      m_DevInfo.Lock=0;	
      return true;
    }  
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    void w25qxx<w25qxx_TEMPLATE_T>::EraseChip(void)
    {
      while(m_DevInfo.Lock==1)
        W25qxx_Delay(1);
      m_DevInfo.Lock=1;	
#if (_W25QXX_DEBUG==1)
      uint32_t	StartTime=HAL::GetTick();	
      printf("w25qxx EraseChip Begin...\r\n");
#endif
      WriteEnable();
      m_CSPin.Low();
      SpiTxRxByte(0xC7);
      m_CSPin.High();
      WaitForWriteEnd();
#if (_W25QXX_DEBUG==1)
      printf("w25qxx EraseBlock done after %d ms!\r\n",HAL::GetTick()-StartTime);
#endif
      W25qxx_Delay(10);
      m_DevInfo.Lock=0;	
    }
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    void w25qxx<w25qxx_TEMPLATE_T>::EraseSector(uint32_t SectorAddr)
    {
      while(m_DevInfo.Lock==1)
        W25qxx_Delay(1);
      m_DevInfo.Lock=1;	
#if (_W25QXX_DEBUG==1)
      uint32_t	StartTime=HAL::GetTick();	
      printf("w25qxx EraseSector %d Begin...\r\n",SectorAddr);
#endif
      WaitForWriteEnd();
      SectorAddr = SectorAddr * m_DevInfo.SectorSize;
      WriteEnable();
      m_CSPin.Low();
      SpiTxRxByte(0x20);
      if(m_DevInfo.ID>=W25Q256)
        SpiTxRxByte((SectorAddr & 0xFF000000) >> 24);
      SpiTxRxByte((SectorAddr & 0xFF0000) >> 16);
      SpiTxRxByte((SectorAddr & 0xFF00) >> 8);
      SpiTxRxByte(SectorAddr & 0xFF);
      m_CSPin.High();
      WaitForWriteEnd();
#if (_W25QXX_DEBUG==1)
      printf("w25qxx EraseSector done after %d ms\r\n",HAL::GetTick()-StartTime);
#endif
      W25qxx_Delay(1);
      m_DevInfo.Lock=0;
    }
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    void w25qxx<w25qxx_TEMPLATE_T>::WritePage(uint8_t *pBuffer	,uint32_t Page_Address,uint32_t OffsetInByte,uint32_t NumByteToWrite_up_to_PageSize,HAL::Spi::spiCallback* Callback)
    {
      while(m_DevInfo.Lock==1)
        W25qxx_Delay(1);
      m_DevInfo.Lock=1;
      if(((NumByteToWrite_up_to_PageSize+OffsetInByte)>m_DevInfo.PageSize)||(NumByteToWrite_up_to_PageSize==0))
        NumByteToWrite_up_to_PageSize=m_DevInfo.PageSize-OffsetInByte;
      if((OffsetInByte+NumByteToWrite_up_to_PageSize) > m_DevInfo.PageSize)
        NumByteToWrite_up_to_PageSize = m_DevInfo.PageSize-OffsetInByte;
#if (_W25QXX_DEBUG==1)
      printf("w25qxx WritePage:%d, Offset:%d ,Writes %d Bytes, begin...\r\n",Page_Address,OffsetInByte,NumByteToWrite_up_to_PageSize);
      W25qxx_Delay(100);
      uint32_t	StartTime=HAL::GetTick();
#endif	
      WaitForWriteEnd();
      WriteEnable();
      m_CSPin.Low();
      SpiTxRxByte(0x02);
      Page_Address = (Page_Address*m_DevInfo.PageSize)+OffsetInByte;	
      if(m_DevInfo.ID>=W25Q256)
        SpiTxRxByte((Page_Address & 0xFF000000) >> 24);
      SpiTxRxByte((Page_Address & 0xFF0000) >> 16);
      SpiTxRxByte((Page_Address & 0xFF00) >> 8);
      SpiTxRxByte(Page_Address&0xFF);
      
      if(Callback)
      {
        pSpiDriver->TxIntr(pBuffer, NumByteToWrite_up_to_PageSize);
        while(pSpiDriver->GetState() != HAL::Spi::SPI_READY);
      }
      else
      {
        pSpiDriver->TxPoll(pBuffer,NumByteToWrite_up_to_PageSize,100);
      }
      
      m_CSPin.High();
      WaitForWriteEnd();
#if (_W25QXX_DEBUG==1)
      StartTime = HAL::GetTick()-StartTime; 
      for(uint32_t i=0;i<NumByteToWrite_up_to_PageSize ; i++)
      {
        if((i%8==0)&&(i>2))
        {
          printf("\r\n");
          W25qxx_Delay(10);			
        }
        printf("0x%02X,",pBuffer[i]);		
      }	
      printf("\r\n");
      printf("w25qxx WritePage done after %d ms\r\n",StartTime);
      W25qxx_Delay(100);
#endif	
      W25qxx_Delay(1);
      m_DevInfo.Lock=0;
    }
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    void w25qxx<w25qxx_TEMPLATE_T>::ReadPage(uint8_t *pBuffer,uint32_t Page_Address,uint32_t OffsetInByte,uint32_t NumByteToRead_up_to_PageSize,HAL::Spi::spiCallback* Callback)
    {
      while(m_DevInfo.Lock==1)
        W25qxx_Delay(1);
      m_DevInfo.Lock=1;
      if((NumByteToRead_up_to_PageSize>m_DevInfo.PageSize)||(NumByteToRead_up_to_PageSize==0))
        NumByteToRead_up_to_PageSize=m_DevInfo.PageSize;
      if((OffsetInByte+NumByteToRead_up_to_PageSize) > m_DevInfo.PageSize)
        NumByteToRead_up_to_PageSize = m_DevInfo.PageSize-OffsetInByte;
#if (_W25QXX_DEBUG==1)
      printf("w25qxx ReadPage:%d, Offset:%d ,Read %d Bytes, begin...\r\n",Page_Address,OffsetInByte,NumByteToRead_up_to_PageSize);
      W25qxx_Delay(100);
      uint32_t	StartTime=HAL::GetTick();
#endif	
      Page_Address = Page_Address*m_DevInfo.PageSize+OffsetInByte;
      m_CSPin.Low();
      SpiTxRxByte(0x0B);
      if(m_DevInfo.ID>=W25Q256)
        SpiTxRxByte((Page_Address & 0xFF000000) >> 24);
      SpiTxRxByte((Page_Address & 0xFF0000) >> 16);
      SpiTxRxByte((Page_Address& 0xFF00) >> 8);
      SpiTxRxByte(Page_Address & 0xFF);
      SpiTxRxByte(0);
      
      if(Callback)
      {
        pSpiDriver->RxIntr(pBuffer, NumByteToRead_up_to_PageSize);
        while(pSpiDriver->GetState() != HAL::Spi::SPI_READY);
      }
      else
      {
        pSpiDriver->RxPoll(pBuffer,NumByteToRead_up_to_PageSize,100);
      }
      m_CSPin.High();
#if (_W25QXX_DEBUG==1)
      StartTime = HAL::GetTick()-StartTime; 
      for(uint32_t i=0;i<NumByteToRead_up_to_PageSize ; i++)
      {
        if((i%8==0)&&(i>2))
        {
          printf("\r\n");
          W25qxx_Delay(10);
        }
        printf("0x%02X,",pBuffer[i]);		
      }	
      printf("\r\n");
      printf("w25qxx ReadPage done after %d ms\r\n",StartTime);
      W25qxx_Delay(100);
#endif	
      W25qxx_Delay(1);
      m_DevInfo.Lock=0;
    }  

  
   //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    w25qxx<w25qxx_TEMPLATE_T>::
    Status_t w25qxx<w25qxx_TEMPLATE_T>::PageWrite(uint32_t Page_Address, uint8_t* pBuffer, HAL::Spi::spiCallback* Callback, uint16_t len )
    {
      if(pBuffer == nullptr) return W25QXX_ERROR;
      
      Page_Address = Page_Address*m_DevInfo.PageSize+0;
      
      /* Check for Flash Busy status*/
      m_CSPin.Low();      
      SpiTxRxByte(0x05);
      if(SpiTxRxByte(W25QXX_DUMMY_BYTE) & 0x01) 
        return W25QXX_BUSY;      
      m_CSPin.High();
      
      /* Send Write Enable*/
      m_CSPin.Low();
      SpiTxRxByte(0x06);
      m_CSPin.High();
      
      m_CSPin.Low();     

      pBuffer[0] = 0x02;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;
      
      if(Callback)
      {
        pSpiDriver->TxIntr(pBuffer,len+4,Callback);
        //pSpiDriver->TxDMA(pBuffer,len+4,&w25qxxCb);
      }
      else
      {
        pSpiDriver->TxPoll(pBuffer,len+4,100);
        m_CSPin.High();
      }      
      return W25QXX_SUCCESS;
    }  
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS> 
    w25qxx<w25qxx_TEMPLATE_T>::
    Status_t w25qxx<w25qxx_TEMPLATE_T>::PageRead(uint32_t Page_Address, uint8_t* pBuffer, HAL::Spi::spiCallback* Callback, uint16_t len )
    {
      Transaction_t Transaction;
      if(pBuffer == nullptr) return W25QXX_ERROR;
      
      m_CSPin.Low();      
      SpiTxRxByte(0x05);
      if(SpiTxRxByte(W25QXX_DUMMY_BYTE) & 0x01) 
        return W25QXX_BUSY;      
      m_CSPin.High();

      Page_Address = Page_Address*m_DevInfo.PageSize+0;
      m_CSPin.Low();
       
      pBuffer[0] = 0x0B;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;
      pBuffer[4] = 0x00;
      
      Transaction.TxBuf = pBuffer;
      Transaction.TxLen = 5;
      Transaction.RxBuf = pBuffer;
      Transaction.RxLen = len;
      Transaction.CSPort = CSPort;
      Transaction.CSPinmask = CSPinmask;
      Transaction.XferDoneCallback = Callback;
      
      if(Callback)
      {       
        pSpiDriver->XferIntr(&Transaction);       
      }
      else
      {
          pSpiDriver->XferPoll(&Transaction);
          m_CSPin.High();
      }      
      return W25QXX_SUCCESS;
    }  
  
     //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS>
    w25qxx<w25qxx_TEMPLATE_T>::
  Status_t w25qxx<w25qxx_TEMPLATE_T>::PageWriteDMA(uint32_t Page_Address, uint8_t* pBuffer, HAL::Spi::spiCallback* Callback, uint16_t len )
  {
    Transaction_t Transaction;
    if(pBuffer == nullptr) return W25QXX_ERROR;
    
    Page_Address = Page_Address*m_DevInfo.PageSize+0;
    
    /* Check for Flash Busy status*/
    m_CSPin.Low();      
    SpiTxRxByte(0x05);
    if(SpiTxRxByte(W25QXX_DUMMY_BYTE) & 0x01) return W25QXX_BUSY;      
    m_CSPin.High();
    
    /* Send Write Enable*/
    m_CSPin.Low();
    SpiTxRxByte(0x06);
    m_CSPin.High();
    
    m_CSPin.Low();     
    
    pBuffer[0] = 0x02;
    pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
    pBuffer[2] = (Page_Address& 0xFF00) >> 8;
    pBuffer[3] = Page_Address & 0xFF;
    
    Transaction.TxBuf = pBuffer;
    Transaction.TxLen = len+4;
    Transaction.RxBuf = pBuffer;
    Transaction.RxLen = 0;
    Transaction.CSPort = CSPort;
    Transaction.CSPinmask = CSPinmask;
    Transaction.XferDoneCallback = Callback;
    
#if 1   
    pSpiDriver->XferDMA(&Transaction);   
#else
     pSpiDriver->TxDMA(pBuffer,len+4,Callback);
#endif    
    return W25QXX_SUCCESS;
  }  
  
  //###################################################################################################################
  template<w25qxx_TEMPLATE_PARAMS> 
    w25qxx<w25qxx_TEMPLATE_T>::
  Status_t w25qxx<w25qxx_TEMPLATE_T>::PageReadDMA(uint32_t Page_Address, uint8_t* pBuffer, HAL::Spi::spiCallback* Callback, uint16_t len )
  {
    Transaction_t Transaction;
    if(pBuffer == nullptr) return W25QXX_ERROR;
#if 1
    m_CSPin.Low();      
    SpiTxRxByte(0x05);
    if(SpiTxRxByte(W25QXX_DUMMY_BYTE) & 0x01) 
      return W25QXX_BUSY;      
    m_CSPin.High();
#else
     WaitForWriteEnd();
#endif
    Page_Address = Page_Address*m_DevInfo.PageSize+0;
    m_CSPin.Low();
    
    pBuffer[0] = 0x0B;
    pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
    pBuffer[2] = (Page_Address& 0xFF00) >> 8;
    pBuffer[3] = Page_Address & 0xFF;
    pBuffer[4] = 0x00;
    
    Transaction.TxBuf = pBuffer;
    Transaction.TxLen = 5;
    Transaction.RxBuf = pBuffer;
    Transaction.RxLen = len;
    Transaction.CSPort = CSPort;
    Transaction.CSPinmask = CSPinmask;
    Transaction.XferDoneCallback = Callback;
    
//    if(Callback) m_CurrentCallback = Callback; 
#if 1   
    pSpiDriver->XferDMA(&Transaction);    
#else
    pSpiDriver->TxPoll(pBuffer,5,100);
    pSpiDriver->RxDMA(pBuffer,len,Callback);
#endif    
    return W25QXX_SUCCESS;
  }  
}


#endif