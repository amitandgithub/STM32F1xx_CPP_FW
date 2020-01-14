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
static Spi::Transaction_t Transaction;


#define SPI_PRINT printf
#define NOKIA_BOARD 0

#if NOKIA_BOARD
#define SPI spi2
  BSP::w25qxx w25qxxDev(&SPI,B11);
  HAL::DigitalOut<A8> HoldPin;
#else
  #define SPI spi1
  BSP::w25qxx w25qxxDev(&SPI,A4);
#endif

#define _8_BIT Spi::SPI_8BIT
  
static uint8_t TxBuf[261],RxBuf[256];
#define SLAVE_RX_LEN 15
#define SLAVE_TX_LEN 16
SpiBuf_t SPI_Slave_TX_Buf = {TxBuf,sizeof("Shiwani Sangwan"),0};
SpiBuf_t SPI_Slave_RX_Buf = {RxBuf,sizeof("Amit Chaudhary"),0};

bool Test_Condition(bool condition, char* PassStr, char* FailStr);
uint8_t index__=0x01;
class SpiSlaveCallback : public HAL::Spi::spiCallback
{
public:
  virtual void CallbackFunction(HAL::Spi::SpiStatus_t SPIStatus)
  {
#if 1
    if(SPIStatus == Spi::SPI_SLAVE_RX_DONE )
    {
      SPI.SrartListeningTXIntr(TxBuf,SLAVE_TX_LEN);
    }
    else  if(SPIStatus == Spi::SPI_SLAVE_TX_DONE )
    {
      SPI.SrartListeningRXIntr(RxBuf,SLAVE_RX_LEN);
    }  
#else
    SPI.SrartListeningTXIntr(TxBuf,SLAVE_TX_LEN);
#endif
    
  }
};

SpiSlaveCallback SpiSlaveCb;

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
  POLL_TX_16BIT,
  w25qxx_POLL_TX,
  w25qxx_POLL_RX,
  w25qxx_POLL_TX_RX,
  w25qxx_POLL_TXN,
  INTR_TX_16BIT,
  w25qxx_INTR_TX,
  w25qxx_INTR_RX,
  w25qxx_INTR_TX_RX,
  w25qxx_INTR_TXN,
  w25qxx_INTR_TXN_Q,  
  DMA_TX_16BIT,
  w25qxx_DMA_TX,
  w25qxx_DMA_RX,
  w25qxx_DMA_TX_RX,
  w25qxx_DMA_TXN,
  w25qxx_DMA_TXN_Q,
  POLL_TX_SlaveBrd,
  SLAVE_INTR_RX,
  SLAVE_INTR_TX,
  SLAVE_DMA_RX,
  SLAVE_DMA_TX,
  POLL_SLAVE_TEST
};

uint32_t BusyCount = 0;
uint8_t *pBuffer;
uint16_t len;

HAL::Spi::SlaveTxn_t SlaveTxn = { &SPI_Slave_TX_Buf, &SPI_Slave_RX_Buf, &SpiSlaveCb};
uint8_t test_id;
volatile uint32_t Pc_Count;
void w25qxx_Test()
{  
  static bool InitDone;  
  
  uint32_t Page = 12,TxLen;
  uint32_t Page_Address = 0;
  uint32_t posts=0;
  
  
  HAL::DigitalOut<A3> A3Pin;
  HAL::DigitalOut<B3> B3PinCS;
  if(InitDone == false)
  {
    w25qxxDev.HwInit();
    A3Pin.HwInit();
    for(uint16_t i = 0; i<sizeof(TxBuf); i++) TxBuf[i] = 0xBD;
    for(uint16_t i = 0; i<sizeof(RxBuf); i++) RxBuf[i] = 0;
    
#if NOKIA_BOARD
    HoldPin.HwInit();
    HoldPin.High();
#else
   // SPI.SetBaudrate(Spi::SPI_BAUDRATE_DIV4);
#endif    
  B3PinCS.HwInit();
  test_id = w25qxx_POLL_TX;
    InitDone = true;
  } 
  len = 256;
  TxLen = len;
  pBuffer = TxBuf;
  Page_Address = Page*w25qxxDev.m_DevInfo.PageSize+0;
  
  Transaction.Mode = _8_BIT;
  Transaction.pChipselect = &w25qxxDev.m_CSPin;
  test_id = w25qxx_POLL_TX;//POLL_TX_SlaveBrd;
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
      SPI.TxPoll(pBuffer,len+4);
      w25qxxDev.m_CSPin.High();
      LL_mDelay(4);      
      while( w25qxxDev.PageRead(Page,RxBuf) == BSP::w25qxx::W25QXX_BUSY);    
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
      SPI.TxPoll(pBuffer,5);
      SPI.RxPoll(RxBuf,len);
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
      SPI.TxIntr(pBuffer,len+4,_8_BIT,Spi::SPI_BAUDRATE_DIV2,&w25qxxCb,&w25qxxDev.m_CSPin);
      while(!XferDone);  
      XferDone = false;     
      while( w25qxxDev.PageRead(Page,RxBuf) == BSP::w25qxx::W25QXX_BUSY);    
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
      SPI.TxPoll(pBuffer,5);
      SPI.RxIntr(RxBuf,len,_8_BIT,Spi::SPI_BAUDRATE_DIV2,&w25qxxCb,&w25qxxDev.m_CSPin);
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
#if SPI_MASTER_Q
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
      Transaction.Spi_Baudrate = Spi::SPI_BAUDRATE_DIV4;
      Transaction.XferDoneCallback = &w25qxxDMACb;    
      while(SPI.Post(&Transaction,0) != Spi::SPI_TXN_QUEUE_ERROR) posts++;  // 0.45 ms for reading one page in INTR
      while(DMAXferDone < posts);
      Transaction.Spi_Baudrate = Spi::SPI_BAUDRATE_DIV2;
#endif
      test_id = w25qxx_DMA_TX;
      break;
  case w25qxx_DMA_TX:  
      SPI.SetBaudrate(Spi::SPI_BAUDRATE_DIV2);
      w25qxxDev.EraseSector(0);
      w25qxxDev.m_CSPin.Low();
      w25qxxDev.SpiTxRxByte(0x06);
      w25qxxDev.m_CSPin.High();   
      pBuffer[0] = 0x02;
      pBuffer[1] = (Page_Address & 0xFF0000) >> 16;
      pBuffer[2] = (Page_Address& 0xFF00) >> 8;
      pBuffer[3] = Page_Address & 0xFF;  
      pBuffer[4] = pBuffer[5];
      SPI.TxDMA(pBuffer,len+4,_8_BIT,Spi::SPI_BAUDRATE_DIV2,&w25qxxCb,&w25qxxDev.m_CSPin);
      while(!XferDone);  
      XferDone = false;     
      while( w25qxxDev.PageRead(Page,RxBuf) == BSP::w25qxx::W25QXX_BUSY);    
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
      SPI.TxPoll(pBuffer,5);
      SPI.RxDMA(RxBuf,len,_8_BIT,Spi::SPI_BAUDRATE_DIV2,&w25qxxCb,&w25qxxDev.m_CSPin); 
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
#if SPI_MASTER_Q     
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
      //Transaction.Spi_Baudrate = Spi::SPI_BAUDRATE_DIV8;
      Transaction.XferDoneCallback = &w25qxxDMACb;     
      while(SPI.Post(&Transaction,1) != Spi::SPI_TXN_QUEUE_ERROR) posts++;  // 0.13 ms for reading one page in DMA
      while(DMAXferDone < posts);  
#endif
      test_id = 255; 
      break;
    case POLL_TX_16BIT:
      SPI.SetBaudrate(Spi::SPI_BAUDRATE_DIV128);
      pBuffer[0] = 0xBB;
      pBuffer[1] = 0xAA;
      pBuffer[2] = 0x22;
      pBuffer[3] = 0x11;
      pBuffer[4] = 0x44;
      pBuffer[5] = 0x33;
      w25qxxDev.m_CSPin.Low(); 
      SPI.TxPoll(pBuffer,3,Spi::SPI_16BIT);
      w25qxxDev.m_CSPin.High(); 
      test_id = INTR_TX_16BIT;
      break;
    case INTR_TX_16BIT:
      Transaction.Spi_Baudrate = Spi::SPI_BAUDRATE_DIV2;
      XferDone = false;
      SPI.TxIntr(pBuffer,3,Spi::SPI_16BIT,Spi::SPI_BAUDRATE_DIV128,&w25qxxCb,&w25qxxDev.m_CSPin);
      while(!XferDone); 
      XferDone = false; 
      test_id = DMA_TX_16BIT;
      break;
      
    case DMA_TX_16BIT:
      XferDone = false; 
      w25qxxDev.m_CSPin.Low(); 
      SPI.TxDMA(pBuffer,3,Spi::SPI_16BIT,Spi::SPI_BAUDRATE_DIV128,&w25qxxCb,&w25qxxDev.m_CSPin);
      while(!XferDone);  
      XferDone = false; 
      w25qxxDev.m_CSPin.High(); 
      test_id = POLL_TX_16BIT;
      break;
    
    case POLL_TX_SlaveBrd:
      memcpy(TxBuf,"Shiwani Sangwan",sizeof("Shiwani Sangwan"));
      test_id = POLL_TX_SlaveBrd;
      Transaction.pChipselect = nullptr;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = SLAVE_TX_LEN;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = SLAVE_RX_LEN;
      Transaction.XferDoneCallback = &SpiSlaveCb;
      SPI.HwInit(true);
      SPI.CheckAndLoadTxn(&Transaction);
      SPI.SrartListeningRXIntr(RxBuf,SLAVE_RX_LEN);
      //SPI.SrartListeningTXIntr(TxBuf,SLAVE_TX_LEN);
      while(1) Pc_Count++;
      break;
    case POLL_SLAVE_TEST:      
      B3PinCS.Low(); 
      SPI.RxPoll(pBuffer,4,Spi::SPI_8BIT);
      B3PinCS.High();
      LL_mDelay(1100);   
      break;
    
    default: break;      
    }
    memset(RxBuf,0,sizeof(RxBuf));
    //LL_mDelay(1000);        
  }  
}


#endif // w25qxx_test_h