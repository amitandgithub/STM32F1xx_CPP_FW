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
#include"DigitalIn.h"
#include"DMA.h"

extern HAL::DMA dma1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::InterruptManager InterruptManagerInstance;

#define SPI_WAIT_FOR_TXE_FLAG_TO_SET(_SPI_,TIMEOUT) WaitOnFlag(&_SPI_->SR, SPI_SR_TXE, 0, TIMEOUT)

#define SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(_SPI_,TIMEOUT) WaitOnFlag(&_SPI_->SR, SPI_SR_BSY, SPI_SR_BSY, TIMEOUT)

#define SPI_WAIT_FOR_RXNE_FLAG_TO_SET(_SPI_,TIMEOUT) WaitOnFlag(&_SPI_->SR, SPI_SR_RXNE, 0, TIMEOUT)
 
#define SPI_GET_EVENT(_SPI_)    POSITION_VAL(_SPI_->SR)

namespace HAL
{    

#define SPI_DEBUG 0
  
#define SPI_POLL        1  
  
#define SPI_MASTER_Q    0 

#define SPI_MASTER_INTR     1  
#define SPI_SLAVE_INTR      0 
#define SPI_MASTER_DMA      0  
#define SPI_SLAVE_DMA       0  
  
// Full SPI module - 4'562 - 4'468
  
#define SPI_LOG_STATES_SIZE 500
  

  
  class Spi : public InterruptSource
  {
  public:
    
    static const uint32_t SPI_TIMEOUT           = 5000U;
    typedef enum  : uint8_t
    {
    SPI_OK,
    SPI_BUSY,
    SPI_INVALID_PARAMS,
    SPI_TXE_TIMEOUT,
    SPI_RXNE_TIMEOUT,
    SPI_BUSY_TIMEOUT,

  }SpiStatus_t;
  
    typedef uint32_t Hz_t ;
    typedef SPI_TypeDef* SPIx_t;    
    typedef Buf_t spiBuf_t;
    
    class spiCallback
    {
    public:
      spiCallback(){};
      virtual ~spiCallback(){};
      virtual void CallbackFunction(SpiStatus_t SPIStatus) = 0;
    };
    
    typedef Callback* SPICallback_t;
    
    
    typedef struct
    {
      uint8_t                 TxLen;
      uint8_t                 RxLen; 
      uint8_t*                TxBuf; 
      uint8_t*                RxBuf;
      SPICallback_t           XferDoneCallback;
    }MasterTxn_t;
    
    typedef struct
    {
      Buf_t*             TxBuf;
      Buf_t*             RxBuf;
      SPICallback_t         XferDoneCallback;
    }SlaveTxn_t;    
    
    typedef enum 
    {
      EVENT_RXNE    = 0U,
      EVENT_TXE     = 1U,
      EVENT_CHSIDE  = 2U,
      EVENT_UDR     = 3U,
      EVENT_CRCERR  = 4U,
      EVENT_MODF    = 5U,
      EVENT_OVR     = 6U,
      EVENT_BSY     = 7U,
    }SPI_Interrupts_t;
    

    typedef enum
    {
      SPI_RESET,
      SPI_READY,
      SPI_MASTER_TX,
      SPI_MASTER_RX,
      SPI_SLAVE_TX,
      SPI_SLAVE_RX,
      SPI_SLAVE_RX_LISTENING,
      SPI_MASTER_TX_DMA,
      SPI_MASTER_RX_DMA,
      SPI_SLAVE_TX_DMA,
      SPI_SLAVE_RX_DMA,
      SPI_ERROR_BUSY_TIMEOUT,
    }SPIState_t;      
    
    typedef struct
    {
      uint16_t          TxLen;
      uint16_t          RxLen; 
      uint8_t*          TxBuf; 
      uint8_t*          RxBuf;
      SPICallback_t     XferDoneCallback;
    }Transaction_t; // 20 bytes
    
    
#if SPI_MASTER_Q
    using SPITxnQueue_t = Utils::queue<Transaction_t const *,uint32_t,10U> ;
#endif     
    
    Spi(SpiPort_t SpiPort, Hz_t Hz = 100000U);
    
    ~Spi(){};
    
    SpiStatus_t HwInit(void *pInitStruct = nullptr);
    
    SpiStatus_t HwDeinit();
    
    void Enable(){LL_SPI_Enable(m_SPIx);}
    
    void Disable(){LL_SPI_Disable(m_SPIx);}
    
    void SetTransmissionMode(uint32_t TransmitMode){LL_SPI_SetTransferDirection(m_SPIx, TransmitMode);}
    
    bool WaitOnFlag(volatile uint32_t* reg, uint32_t bitmask, uint32_t status, uint32_t timeout);
    
    SPIState_t GetState(){return m_SPIState;}
    
    void EnableInterrupt(uint32_t InterruptFlags){SET_BIT(m_SPIx->CR2, InterruptFlags);}
    
    void DisableInterrupt(uint32_t InterruptFlags){CLEAR_BIT(m_SPIx->CR2, InterruptFlags);}
      
    void DMA_Tx_Done_Handler();
    
    void DMA_Rx_Done_Handler();
    
    void TxnDoneHandler(uint32_t StopFlag);
    
    void LoadNextTransaction();
    
     void LoadNextTransaction_Q();
    
    SpiStatus_t Post(Transaction_t const *pTransaction, uint32_t Mode = 0);    
    
    SpiStatus_t     TxPoll(uint8_t* TxBuf, uint32_t TxLen, uint8_t Options = 0);
    
    SpiStatus_t     TxOnlyPoll(uint8_t* TxBuf, uint32_t TxLen, uint8_t Options = 0);
    
    SpiStatus_t     RxPoll(uint8_t* RxBuf, uint32_t RxLen, uint8_t Options = 0);
    
    SpiStatus_t     TxRxPoll(uint8_t* TxBuf, uint8_t* RxBuf, uint32_t Len, uint8_t Options = 0);
    
    SpiStatus_t     TxIntr(uint8_t* TxBuf, uint32_t TxLen, SPICallback_t XferDoneCallback = nullptr);
    
    SpiStatus_t     XferPoll(Transaction_t const *pTransaction);
    
    SpiStatus_t     XferIntr(Transaction_t const *pTransaction);
    
    SpiStatus_t     XferDMA(Transaction_t const * pTransaction);
    
    inline void SoftReset();
    
    virtual void ISR();
    void LoadNextTransaction_MASTER_INTR(); 
    void LoadNextTransaction_MASTER_DMA();
    
#if (SPI_MASTER_DMA == 1) || (SPI_SLAVE_DMA == 1)
    void LoadRxDmaChannel(uint8_t* Buf, uint32_t len){dma1.Load(SPI1_RX_DMA_CHANNEL, (uint32_t)&(SPI_DATA_REG(m_I2Cx)),(uint32_t)Buf,len, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);}
    
    void LoadTxDmaChannel(uint8_t* Buf, uint32_t len){dma1.Load(SPI1_TX_DMA_CHANNEL, (uint32_t)&(SPI_DATA_REG(m_I2Cx)),(uint32_t)Buf,len, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);}
#endif
    
#if (SPI_MASTER_DMA == 1) 
    
     void SPI1_DMA_Tx_Done_Handler();
    
    void SPI1_DMA_Rx_Done_Handler();
    
    class SPI1_DMA_Rx_Callback : public InterruptSource
    {
    public:
      SPI1_DMA_Rx_Callback(Spi* This):_This(This){};
      virtual void ISR() ;
    private:
      Spi* _This;
    };
    
    class SPI1_DMA_Tx_Callback : public InterruptSource
    {
    public:
      SPI1_DMA_Tx_Callback(Spi* This):_This(This){};
      virtual void ISR() ;
    private:
      Spi* _This;
    };
    
    class SPI2_DMA_Rx_Callback : public InterruptSource
    {
    public:
      SPI2_DMA_Rx_Callback(Spi* This):_This(This){};
      virtual void ISR() ;
    private:
      Spi* _This;
    };
    
    class SPI2_DMA_Tx_Callback : public InterruptSource
    {
    public:
      SPI2_DMA_Tx_Callback(Spi* This):_This(This){};
      virtual void ISR( ) ;
    private:
      Spi* _This;
    };
#endif  
    
    //inline void LogStates(I2CLogs_t log);
    
  private:
    Hz_t                    m_hz;
    SPIx_t                  m_SPIx;            
    Transaction_t           m_Transaction;
    SlaveTxn_t              m_SlaveTxn;
#if SPI_MASTER_Q 
    SPITxnQueue_t           m_SPITxnQueue;
#endif
    
    /* It must be volatile becoz it is shared between ISR and main loop */
    volatile SPIState_t     m_SPIState; 
    /* It must be volatile becoz it is shared between ISR and main loop */
    volatile SpiStatus_t    m_SpiStatus;
    
#if (SPI_MASTER_DMA == 1) //|| (SPI_SLAVE_DMA == 1)
    SPI1_DMA_Rx_Callback m_SPI1_DMA_Rx_Callback;
    SPI1_DMA_Tx_Callback m_SPI1_DMA_Tx_Callback;
    SPI2_DMA_Rx_Callback m_SPI2_DMA_Rx_Callback;
    SPI2_DMA_Tx_Callback m_SPI2_DMA_Tx_Callback;
#endif
    
  public:         
#if SPI_DEBUG
    I2CLogs_t       I2CStates[SPI_LOG_STATES_SIZE];
    volatile uint32_t        I2CStates_Idx;
    //Utils::DebugLog<DBG_LOG_TYPE>* dbg_log_instance;
#endif
 
  };   
  
  inline void Spi::SoftReset()
  {
//    LL_SPI_EnableReset(m_I2Cx);
//    LL_SPI_DisableReset(m_I2Cx);
    HwInit();
  }  
  
#if SPI_DEBUG     
#pragma inline = forced
  void I2c::LogStates(I2CLogs_t log)
  {
#if 1
    I2CStates[I2CStates_Idx++] = log;
    if(I2CStates_Idx == SPI_LOG_STATES_SIZE )
      I2CStates_Idx = 0;
#else
    dbg_log_instance->Log(log);
#endif
  }
  
#define SPI_DEBUG_LOG(log) (m_I2CStatus = (log))  // 108 bytes of ROM
#define SPI_LOG_STATES(log) LogStates(log)
#define SPI_LOG_EVENTS(log) LogStates(log)
  
#else
  
#define SPI_LOG_STATES(log)
#define SPI_DEBUG_LOG(log)
#define SPI_LOG_EVENTS(log)
  
#endif    
  
}
#endif //SPI_h
