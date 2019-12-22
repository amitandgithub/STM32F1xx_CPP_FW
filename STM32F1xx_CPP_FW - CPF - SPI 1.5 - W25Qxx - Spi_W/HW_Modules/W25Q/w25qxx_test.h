/******************
** FILE: w25qxx_test.h
**
******************/

#include "w25qxx.h"

#ifndef w25qxx_test_h
#define w25qxx_test_h

extern HAL::Spi spi1;
extern HAL::Spi spi2;
volatile bool XferDone = false;
volatile uint32_t DMAXferDone;


#define SPI_PRINT printf
#define NOKIA_BOARD 1

#if NOKIA_BOARD
#define SPI spi2
  #define FLASH_PARAMS &SPI,B11
  BSP::w25qxx<FLASH_PARAMS> w25qxxDev;
  HAL::DigitalOut<A8> HoldPin;
#else
  #define SPI spi1
  #define FLASH_PARAMS &SPI,A4
  BSP::w25qxx<FLASH_PARAMS> w25qxxDev;
#endif

static uint8_t TxBuf[261],RxBuf[256];

bool Test_Condition(bool condition, char* PassStr, char* FailStr);

class w25qxxCallback : public HAL::Spi::spiCallback
{
public:
  virtual void CallbackFunction(HAL::Spi::SpiStatus_t SPIStatus)
  {
    XferDone = true;
  }
};
w25qxxCallback w25qxxCb;

class w25qxxDMACallback: public HAL::Spi::spiCallback
{
  public:
  virtual void CallbackFunction(HAL::Spi::SpiStatus_t SPIStatus)
  {
    Test_Condition( !(std::memcmp( (const void*) &TxBuf[5],(const void*) RxBuf, 256 )), STR("w25qxx_INTR_TXN_Q = Pass"), STR("w25qxx_INTR_TXN_Q = Fail"));
    memset(RxBuf,0,sizeof(RxBuf));
    DMAXferDone++;
  }
};
w25qxxDMACallback w25qxxDMACb;

enum 
{
  w25qxx_POLL,
  w25qxx_POLL_TX,
  w25qxx_POLL_RX,
  w25qxx_POLL_TX_RX,
  w25qxx_POLL_TXN,
  w25qxx_INTR,
  w25qxx_INTR_TX,
  w25qxx_INTR_RX,
  w25qxx_INTR_TX_RX,
  w25qxx_INTR_TXN,
  w25qxx_INTR_TXN_Q,  
  w25qxx_DMA,
  w25qxx_DMA_TX,
  w25qxx_DMA_RX,
  w25qxx_DMA_TX_RX,
  w25qxx_DMA_TXN,
  w25qxx_DMA_TXN_Q,
  w25qxx_OLD,
};



uint32_t BusyCount = 0;
uint8_t *pBuffer;
uint16_t len;
void w25qxx_Test()
{
  
  static bool InitDone;
  uint8_t test_id = 0;
  uint32_t Page = 9,TxLen;
  uint32_t Page_Address = 0;
  uint32_t posts=0;
  Spi::Transaction_t Transaction;
  
  HAL::DigitalOut<A3> A3Pin;

  if(InitDone == false)
  {
    w25qxxDev.HwInit();
    A3Pin.HwInit();
    for(uint16_t i = 0; i<sizeof(TxBuf); i++) TxBuf[i] = 0xBB;
    for(uint16_t i = 0; i<sizeof(RxBuf); i++) RxBuf[i] = 0;
    
#if NOKIA_BOARD
    HoldPin.HwInit();
    HoldPin.High();
#else
    SPI.SetBaudrate(Spi::SPI_BAUDRATE_DIV4);
#endif    
    InitDone = true;
  } 
  len = 256;
  TxLen = len;
  pBuffer = TxBuf;
  Page_Address = Page*w25qxxDev.m_DevInfo.PageSize+0;
  
  Transaction.CSPort = w25qxxDev.m_CSPin.GetPort();
  Transaction.CSPinmask = w25qxxDev.m_CSPin.GetPinmask();
        
  test_id = w25qxx_POLL_TX;
  
  while(1)
  {
    
    switch(test_id)
    { 
    case w25qxx_POLL_TX:      
      w25qxxDev.EraseSector(0);
      w25qxxDev.m_CSPin.Low();
      w25qxxDev.SpiTxRxByte(0x06);
      w25qxxDev.m_CSPin.High();
      w25qxxDev.m_CSPin.Low();      
      pBuffer[0] = 0x02;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;  
      pBuffer[4] = pBuffer[5];
      SPI.TxPoll(pBuffer,len+4,100);
      w25qxxDev.m_CSPin.High();
      LL_mDelay(4);      
      while( w25qxxDev.PageRead(Page,RxBuf) == BSP::w25qxx<FLASH_PARAMS>::W25QXX_BUSY);    
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[4],(const void*) RxBuf, TxLen )), STR("w25qxx_POLL_TX = Pass"), STR("w25qxx_POLL_TX = Fail"));
      test_id = w25qxx_POLL_RX;      
      break;      
    case w25qxx_POLL_RX:      
      w25qxxDev.m_CSPin.Low();      
      pBuffer[0] = 0x0B;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;
      pBuffer[4] = 0x00;      
      SPI.TxPoll(pBuffer,5,100);
      SPI.RxPoll(RxBuf,len,100);
      w25qxxDev.m_CSPin.High();  
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[5],(const void*) RxBuf, TxLen )), STR("w25qxx_POLL_RX = Pass"), STR("w25qxx_POLL_RX = Fail"));
      test_id = w25qxx_POLL_TXN;      
      break;
    case w25qxx_POLL_TXN:      
      w25qxxDev.m_CSPin.Low();      
      Transaction.TxBuf = pBuffer;
      Transaction.TxLen = 5;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = len;
      Transaction.XferDoneCallback = &w25qxxCb;     
      SPI.XferPoll(&Transaction);
      w25qxxDev.m_CSPin.High();        
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[5],(const void*) RxBuf, TxLen )), STR("w25qxx_POLL_TXN = Pass"), STR("w25qxx_POLL_TXN = Fail"));
      test_id = w25qxx_INTR_TX;      
      break;  
  case w25qxx_INTR_TX:  
      w25qxxDev.EraseSector(0);
      w25qxxDev.m_CSPin.Low();
      w25qxxDev.SpiTxRxByte(0x06);
      w25qxxDev.m_CSPin.High();    
      pBuffer[0] = 0x02;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;   
      pBuffer[4] = pBuffer[5];
      SPI.TxIntr(pBuffer,len+4,&w25qxxCb,Transaction.CSPort,Transaction.CSPinmask);
      while(!XferDone);  
      XferDone = false;     
      while( w25qxxDev.PageRead(Page,RxBuf) == BSP::w25qxx<FLASH_PARAMS>::W25QXX_BUSY);    
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[4],(const void*) RxBuf, TxLen )), STR("w25qxx_INTR_TX = Pass"), STR("w25qxx_INTR_TX = Fail"));
      test_id = w25qxx_INTR_RX;      
      break;      
    case w25qxx_INTR_RX:    
      w25qxxDev.m_CSPin.Low();      
      pBuffer[0] = 0x0B;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;
      pBuffer[4] = 0x00;      
      SPI.TxPoll(pBuffer,5,100);
      SPI.RxIntr(RxBuf,len,&w25qxxCb,Transaction.CSPort,Transaction.CSPinmask);
      while(!XferDone);  
      XferDone = false; 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[5],(const void*) RxBuf, TxLen )), STR("w25qxx_INTR_RX = Pass"), STR("w25qxx_INTR_RX = Fail"));
      test_id = w25qxx_INTR_TXN;      
      break;
    case w25qxx_INTR_TXN:           
      Transaction.TxBuf = pBuffer;
      Transaction.TxLen = 5;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = len;
      Transaction.XferDoneCallback = &w25qxxCb;     
      SPI.XferIntr(&Transaction);
      while(!XferDone);  
      XferDone = false;       
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[5],(const void*) RxBuf, TxLen )), STR("w25qxx_INTR_TXN = Pass"), STR("w25qxx_INTR_TXN = Fail"));
      test_id = w25qxx_INTR_TXN_Q;      
      break;
  case w25qxx_INTR_TXN_Q:      
      DMAXferDone = 0; 
      posts = 0;
      pBuffer[0] = 0x0B;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;
      pBuffer[4] = 0x00;      
      Transaction.TxBuf = pBuffer;
      Transaction.TxLen = 5;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = len;
      Transaction.XferDoneCallback = &w25qxxDMACb;     
      while(SPI.Post(&Transaction,0) != Spi::SPI_TXN_QUEUE_ERROR) posts++;  // 0.45 ms for reading one page in INTR
      while(DMAXferDone < posts);  
      test_id = w25qxx_DMA_TX;
      break;
  case w25qxx_DMA_TX:  
      w25qxxDev.EraseSector(0);
      w25qxxDev.m_CSPin.Low();
      w25qxxDev.SpiTxRxByte(0x06);
      w25qxxDev.m_CSPin.High();   
      pBuffer[0] = 0x02;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;  
      pBuffer[4] = pBuffer[5];
      SPI.TxDMA(pBuffer,len+4,&w25qxxCb,Transaction.CSPort,Transaction.CSPinmask);
      while(!XferDone);  
      XferDone = false;     
      while( w25qxxDev.PageRead(Page,RxBuf) == BSP::w25qxx<FLASH_PARAMS>::W25QXX_BUSY);    
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[4],(const void*) RxBuf, TxLen )), STR("w25qxx_DMA_TX = Pass"), STR("w25qxx_DMA_TX = Fail"));
      test_id = w25qxx_DMA_RX;      
      break;      
    case w25qxx_DMA_RX:      
      w25qxxDev.m_CSPin.Low();      
      pBuffer[0] = 0x0B;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;
      pBuffer[4] = 0x00;      
      SPI.TxPoll(pBuffer,5,100);
      SPI.RxDMA(RxBuf,len,&w25qxxCb,Transaction.CSPort,Transaction.CSPinmask); 
      while(!XferDone);  
      XferDone = false; 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[5],(const void*) RxBuf, TxLen )), STR("w25qxx_DMA_RX = Pass"), STR("w25qxx_DMA_RX = Fail"));
      test_id = w25qxx_DMA_TXN;      
      break;
    case w25qxx_DMA_TXN:        
      pBuffer[0] = 0x0B;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;
      pBuffer[4] = 0x00;   
      Transaction.TxBuf = pBuffer;
      Transaction.TxLen = 5;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = len;
      Transaction.XferDoneCallback = &w25qxxCb;     
      SPI.XferDMA(&Transaction);
      while(!XferDone);  
      XferDone = false;       
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[5],(const void*) RxBuf, TxLen )), STR("w25qxx_DMA_TXN = Pass"), STR("w25qxx_DMA_TXN = Fail"));
      test_id = w25qxx_DMA_TXN_Q;      
      break;
   case w25qxx_DMA_TXN_Q:      
      DMAXferDone = 0; 
      posts = 0;
      pBuffer[0] = 0x0B;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;
      pBuffer[4] = 0x00;      
      Transaction.TxBuf = pBuffer;
      Transaction.TxLen = 5;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = len;
      Transaction.XferDoneCallback = &w25qxxDMACb;     
      while(SPI.Post(&Transaction,1) != Spi::SPI_TXN_QUEUE_ERROR) posts++;  // 0.13 ms for reading one page in DMA
      while(DMAXferDone < posts);  
      test_id = 255;      
      break;
    case w25qxx_INTR:
      w25qxxDev.EraseSector(0);
      XferDone = false;
      while( w25qxxDev.PageWrite(2,TxBuf,&w25qxxCb) == BSP::w25qxx<FLASH_PARAMS>::W25QXX_BUSY); 
      while(!XferDone);  
      XferDone = false;
      while( w25qxxDev.PageRead(2,RxBuf,&w25qxxCb) == BSP::w25qxx<FLASH_PARAMS>::W25QXX_BUSY);
      while(!XferDone);
      test_id = w25qxx_DMA;
      break;
    case w25qxx_DMA:
      BusyCount = 0;
      A3Pin.Toggle();
      w25qxxDev.EraseSector(0);
      A3Pin.Toggle(); 
      XferDone = false;
      while( w25qxxDev.PageWriteDMA(5,TxBuf,&w25qxxCb) == BSP::w25qxx<FLASH_PARAMS>::W25QXX_BUSY); 
      while(!XferDone);
      A3Pin.Toggle();
      XferDone = false;
     // LL_mDelay(300);  
      while( w25qxxDev.PageReadDMA(5,RxBuf,&w25qxxCb) == BSP::w25qxx<FLASH_PARAMS>::W25QXX_BUSY)BusyCount++;
      while(!XferDone);
      A3Pin.Toggle();
      test_id = 255;
      break;
    
    default: break;      
    }
    memset(RxBuf,0,sizeof(RxBuf));
    LL_mDelay(1000);        
  }  
}


#endif // w25qxx_test_h