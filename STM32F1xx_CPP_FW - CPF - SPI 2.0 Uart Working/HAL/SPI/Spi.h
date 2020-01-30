/******************
** CLASS: Spi
**
** DESCRIPTION:
**  Implements the Spi driver
**
** CREATED: 8/11/2019, by Amit Chaudhary
**
** FILE: Spi.h
**
******************/
#ifndef Spi_h
#define Spi_h

#include"chip_peripheral.h"
#include"CPP_HAL.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Queue.h"
#include"DebugLog.h"
#include"DigitalOut.h"
#include"GpioOutput.h"
#include"GpioInput.h"
#include"DigitalIn.h"
#include"DMA.h"
#include <stdio.h>
extern HAL::DMA dma1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::InterruptManager InterruptManagerInstance;   


namespace HAL
{    
  
#define SPI_DEBUG 0
  
#define SPI_POLL                1  // 638
  
#define SPI_MASTER_Q            0  // 646
  
#define SPI_MASTER_INTR         0  // 1'724 , 1'892 Q
#define SPI_SLAVE_INTR          0  // 2'776, 2'824Q
#define SPI_MASTER_DMA          0  // 1'920, 2'238Q
#define SPI_SLAVE_DMA           0  // 2'620, 2'676Q - 1'456
  
  // Full SPI module - 4'646
  
#define SPI_LOG_STATES_SIZE 500
  
  
  
  class Spi : public InterruptSource
  {
  public:
    static const uint32_t SPI1_RX_DMA_CHANNEL = 2;
    static const uint32_t SPI1_TX_DMA_CHANNEL = 3;
    static const uint32_t SPI2_RX_DMA_CHANNEL = 4;
    static const uint32_t SPI2_TX_DMA_CHANNEL = 5;
    static const uint32_t SPI_TIMEOUT = 5000U;
    
    typedef enum  : uint8_t
    {
      SPI_OK,
      SPI_SLAVE_TX_DONE,
      SPI_SLAVE_RX_DONE,
      SPI_SLAVE_TXRX_DONE,
      SPI_SLAVE_TX_DMA_DONE,
      SPI_SLAVE_RX_DMA_DONE,
      SPI_SLAVE_TXRX_DMA_DONE,
      SPI_BUSY,
      SPI_INVALID_PARAMS,
      SPI_TXE_TIMEOUT,
      SPI_RXNE_TIMEOUT,
      SPI_BUSY_TIMEOUT,
      SPI_TXN_POSTED,
      SPI_TXN_QUEUE_ERROR,
    }SpiStatus_t;
    
    typedef uint32_t Hz_t ;
    
    typedef SPI_TypeDef* SPIx_t;    
    
    class spiCallback
    {
    public:
      virtual void CallbackFunction(SpiStatus_t SPIStatus) = 0;
    };
    
    typedef spiCallback* SPICallback_t;  
    
    typedef enum : uint8_t
    {
      SPI_BAUDRATE_DIV2         =  LL_SPI_BAUDRATEPRESCALER_DIV2,
      SPI_BAUDRATE_DIV4         =  LL_SPI_BAUDRATEPRESCALER_DIV4,
      SPI_BAUDRATE_DIV8         =  LL_SPI_BAUDRATEPRESCALER_DIV8,
      SPI_BAUDRATE_DIV16        =  LL_SPI_BAUDRATEPRESCALER_DIV16,
      SPI_BAUDRATE_DIV32        =  LL_SPI_BAUDRATEPRESCALER_DIV32,
      SPI_BAUDRATE_DIV64        =  LL_SPI_BAUDRATEPRESCALER_DIV64,
      SPI_BAUDRATE_DIV128       =  LL_SPI_BAUDRATEPRESCALER_DIV128,
      SPI_BAUDRATE_DIV256       =  LL_SPI_BAUDRATEPRESCALER_DIV256
    }Spi_Baudrate_t;  
    
    typedef enum
    {
      SPI_RESET,
      SPI_READY,
      SPI_MASTER_TX,
      SPI_MASTER_RX,
      SPI_MASTER_TXRX,
      SPI_SLAVE_TX,
      SPI_SLAVE_RX,
      SPI_SLAVE_TXRX,
      SPI_SLAVE_RX_LISTENING,
      SPI_MASTER_TX_DMA,
      SPI_MASTER_RX_DMA,
      SPI_SLAVE_TX_DMA,
      SPI_SLAVE_RX_DMA,
      SPI_MASTER_TXRX_DMA,
      SPI_ERROR_BUSY_TIMEOUT,
    }SPIState_t;      
    
    typedef enum : uint8_t
    {
      SPI_8BIT,
      SPI_16BIT,
    }XferMode_t; 
    
    typedef struct
    {
      uint16_t          TxLen;
      uint16_t          RxLen; 
      uint8_t*          TxBuf; 
      uint8_t*          RxBuf;      
      SPICallback_t     XferDoneCallback;
      HAL::GpioOutput*  pChipselect;
      Spi_Baudrate_t    Spi_Baudrate; 
      XferMode_t        Mode;
    }Transaction_t; 
    
    typedef void (Spi::*CurrentIsr)(); 
    
    Spi(SpiPort_t SpiPort, Hz_t Hz = 100000U);
    
    ~Spi(){};
    
    SpiStatus_t HwInit(bool Slave = false, XferMode_t mode = SPI_8BIT);
    
    void MasterPinsHwInit(Port_t ClkPort, PIN_t ClkPin,Port_t MisoPort, PIN_t MisoPin,Port_t MosiPort, PIN_t MosiPin);
    
    void SlavePinsHwInit(Port_t ChipSelectPort, PIN_t ChipSelectPin, Port_t ClkPort, PIN_t ClkPin,Port_t MisoPort, PIN_t MisoPin,Port_t MosiPort, PIN_t MosiPin);
    
    SpiStatus_t HwDeinit();
    
    void Enable(){LL_SPI_Enable(m_SPIx);}
    
    void Disable(){LL_SPI_Disable(m_SPIx);}
    
    void EnableInterrupt(uint32_t InterruptFlags){SET_BIT(m_SPIx->CR2, InterruptFlags);}
    
    void DisableInterrupt(uint32_t InterruptFlags){CLEAR_BIT(m_SPIx->CR2, InterruptFlags);}
    
    SPIState_t GetState(){return m_SPIState;}   
    
    SpiStatus_t CheckAndLoadTxn(Transaction_t const *pTransaction);
    
    SpiStatus_t CheckAndLoadTxn(uint8_t* TxBuf, uint32_t TxLen,uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS);   
    
    SpiStatus_t     Tx(uint8_t data);   
    
    SpiStatus_t     Tx(uint16_t data, XferMode_t mode = SPI_8BIT);
    
    SpiStatus_t     Tx(uint32_t data, XferMode_t mode = SPI_8BIT);
    
    SpiStatus_t     TxPoll(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode = SPI_8BIT);
    
    SpiStatus_t     RxPoll(uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode = SPI_8BIT);
    
    uint8_t         TxRxPoll8Bit(uint8_t data);
    
    SpiStatus_t     TxRxPoll(uint8_t* TxBuf, uint8_t* RxBuf, uint32_t Len, XferMode_t mode = SPI_8BIT);
    
    SpiStatus_t     XferPoll(Transaction_t const *pTransaction);
    
    SpiStatus_t     TxIntr(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode = SPI_8BIT, Spi_Baudrate_t Spi_Baudrate = SPI_BAUDRATE_DIV4, SPICallback_t XferDoneCallback = nullptr, GpioOutput* pCS = nullptr);
    
    SpiStatus_t     RxIntr(uint8_t* RxBuf, uint32_t RxLen,  XferMode_t mode = SPI_8BIT,Spi_Baudrate_t Spi_Baudrate = SPI_BAUDRATE_DIV4, SPICallback_t XferDoneCallback = nullptr, GpioOutput* pCS = nullptr);
    
    SpiStatus_t     TxRxIntr(uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen, Spi_Baudrate_t Spi_Baudrate = SPI_BAUDRATE_DIV4, SPICallback_t XferDoneCallback = nullptr);
    
    SpiStatus_t     TxRxIntr(Transaction_t const * pTransaction);    
    
    SpiStatus_t     XferIntr(Transaction_t const *pTransaction);
    
    void TxnDoneHandler_INTR();
    
    void Master_Tx_8bit_Isr();
    
    void Master_Rx_8bit_Isr();
    
    void Master_Tx_16bit_Isr();
    
    void Master_Rx_16bit_Isr();    
    
    void SrartListeningTXIntr8bit(uint8_t* Buf, uint16_t Len);
    
    void SrartListeningRXIntr8bit(uint8_t* Buf, uint16_t Len);
    
    void SrartListeningTXIntr16bit(uint8_t* Buf, uint16_t Len);
    
    void SrartListeningRXIntr16bit(uint8_t* Buf, uint16_t Len);  
    
    void Slave_Tx_8bit_Isr();
    
    void Slave_Rx_8bit_Isr();
    
    void Slave_Tx_16bit_Isr();
    
    void Slave_Rx_16bit_Isr();
    
    SpiStatus_t     RxDMA(uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode = SPI_8BIT,Spi_Baudrate_t Spi_Baudrate = SPI_BAUDRATE_DIV4, SPICallback_t XferDoneCallback = nullptr, GpioOutput* pCS = nullptr);
    
    SpiStatus_t     TxDMA(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode = SPI_8BIT,Spi_Baudrate_t Spi_Baudrate = SPI_BAUDRATE_DIV4, SPICallback_t XferDoneCallback = nullptr, GpioOutput* pCS = nullptr);
    
    SpiStatus_t     TxRxDMA(Transaction_t const * pTransaction);
    
    SpiStatus_t     XferDMA(Transaction_t const * pTransaction);
    
    void DMA_Tx_Done_Handler();
    
    void DMA_Rx_Done_Handler();    
    
    void TxnDoneHandler_DMA();
    
    void SPI1_DMA_MasterTx_Done_Handler();
    
    void SPI2_DMA_MasterTx_Done_Handler();     
    
    void SetBaudrate(Spi_Baudrate_t Spi_Baudrate) { if(m_Baudrate != Spi_Baudrate) { m_Baudrate = Spi_Baudrate; LL_SPI_SetBaudRatePrescaler(m_SPIx,(uint32_t)Spi_Baudrate);} }
    
#if SPI_MASTER_INTR || SPI_SLAVE_INTR || SPI_MASTER_DMA || SPI_SLAVE_DMA   
    void SetBaudrate() { if(m_Baudrate != m_Transaction.Spi_Baudrate) { m_Baudrate = m_Transaction.Spi_Baudrate; LL_SPI_SetBaudRatePrescaler(m_SPIx,(uint32_t)m_Transaction.Spi_Baudrate);} }
#endif    
    
    virtual void ISR();     
    
    using SPITxnQueue_t = Utils::queue<Transaction_t const *,uint32_t,10U> ;
    
    SpiStatus_t Post(Transaction_t const *pTransaction, uint32_t Mode = 0);
    
    void LoadRxDmaChannel(uint8_t* Buf, uint32_t len);
    
    void LoadTxDmaChannel(uint8_t* Buf, uint32_t len);  
    
    class SPI1_DMA_Tx_Callback : public InterruptSource
    {
    public:
      SPI1_DMA_Tx_Callback(Spi* This):_this(This){};
      virtual void ISR() ;
    private:
      Spi* _this;
    };
    
    class SPI2_DMA_Tx_Callback : public InterruptSource
    {
    public:
      SPI2_DMA_Tx_Callback(Spi* This):_this(This){};
      virtual void ISR() ;
    private:
      Spi* _this;
    };
    
    class SPI1_DMA_Rx_Callback : public InterruptSource
    {
    public:
      SPI1_DMA_Rx_Callback(Spi* This):_this(This){};
      virtual void ISR() ;
    private:
      Spi* _this;
    };      
    
    class SPI2_DMA_Rx_Callback : public InterruptSource
    {
    public:
      SPI2_DMA_Rx_Callback(Spi* This):_this(This){};
      virtual void ISR() ;
    private:
      Spi* _this;
    };
    
    void SlaveHwInit(XferMode_t mode = SPI_8BIT);
    
    void SPI1_DMA_SlaveTx_Done_Handler();
    
    void SPI1_DMA_SlaveRx_Done_Handler();
    
    void SPI2_DMA_SlaveTx_Done_Handler();
    
    void SPI2_DMA_SlaveRx_Done_Handler();  
    
    void SrartListeningTXDma(uint8_t* Buf, uint16_t Len);
    
    void SrartListeningRXDma(uint8_t* Buf, uint16_t Len);       
    
  private:   
    SPIx_t                  m_SPIx;    
    Spi_Baudrate_t          m_Baudrate;
    volatile SPIState_t     m_SPIState;         /* It must be volatile becoz it is shared between ISR and main loop */   
    volatile SpiStatus_t    m_SpiStatus;        /* It must be volatile becoz it is shared between ISR and main loop */
    
#if SPI_MASTER_INTR || SPI_SLAVE_INTR || SPI_MASTER_DMA || SPI_SLAVE_DMA    
    Transaction_t           m_Transaction;
    CurrentIsr              m_CurrentIsr;
#endif 
    
#if SPI_MASTER_Q 
    SPITxnQueue_t           m_SPITxnQueue;
#endif 
    
#if (SPI_MASTER_DMA == 1) || (SPI_SLAVE_DMA == 1)
    uint8_t m_Curent_DMA_Tx_Channel;
    uint8_t m_Curent_DMA_Rx_Channel;
    SPI1_DMA_Tx_Callback m_SPI1_DMA_Tx_Callback;
    SPI2_DMA_Tx_Callback m_SPI2_DMA_Tx_Callback;
#endif
    
#if SPI_SLAVE_DMA   
    SPI1_DMA_Rx_Callback m_SPI1_DMA_Rx_Callback;
    SPI2_DMA_Rx_Callback m_SPI2_DMA_Rx_Callback;
#endif 
    
  };     
  
#define SPI_WAIT_FOR_TXE_FLAG_TO_SET(_SPI_,TIMEOUT) WaitOnFlag(&_SPI_->SR, SPI_SR_TXE, 0, TIMEOUT)
  
#define SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(_SPI_,TIMEOUT) WaitOnFlag(&_SPI_->SR, SPI_SR_BSY, SPI_SR_BSY, TIMEOUT)
  
#define SPI_WAIT_FOR_RXNE_FLAG_TO_SET(_SPI_,TIMEOUT) WaitOnFlag(&_SPI_->SR, SPI_SR_RXNE, 0, TIMEOUT)
  
#define SPI_GET_EVENT(_SPI_)    POSITION_VAL(_SPI_->SR)
  
#define SPI_TXE(_SPI_) _SPI_->SR & SPI_SR_TXE
  
#define SPI_RXNE(_SPI_) _SPI_->SR & SPI_SR_RXNE
  
#define SPI_BYTE_IN_8_BIT(__BUF)      *__BUF.RxBuf++ = m_SPIx->DR; __BUF.RxLen--
  
#define SPI_BYTE_OUT_8_BIT(__BUF)     m_SPIx->DR = *__BUF.TxBuf++; __BUF.TxLen--
  
#define SPI_BYTE_IN_16_BIT(__BUF)      *((uint16_t*)__BUF.RxBuf) = m_SPIx->DR; __BUF.RxBuf += sizeof(uint16_t);  __BUF.RxLen--
  
#define SPI_BYTE_OUT_16_BIT(__BUF)     m_SPIx->DR = *((uint16_t*)__BUF.TxBuf); __BUF.TxBuf += sizeof(uint16_t); __BUF.TxLen--
  
#define SPI_DMA_DISABLE(_SPI_)  
  
#define SPI_CS_LOW()     if(m_Transaction.pChipselect) m_Transaction.pChipselect->Low();
  
#define SPI_CS_HIGH()   if(m_Transaction.pChipselect) m_Transaction.pChipselect->High();
  
  
}
#endif //SPI_h
