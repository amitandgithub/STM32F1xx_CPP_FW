/******************
** CLASS: SpiBase
**
** DESCRIPTION:
**  Implements the Spi Base class
**
** CREATED: 13/1/2020, by Amit Chaudhary
**
** FILE: SpiBase.h
**
******************/
#ifndef SpiBase_h
#define SpiBase_h


#include"chip_peripheral.h"
#include"CPP_HAL.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Queue.h"
#include"DebugLog.h"
#include"DigitalOut.h"
#include"GpioOutput.h"
#include"DigitalIn.h"
#include"DMA.h"
#include <stdio.h>

extern HAL::DMA dma1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::InterruptManager InterruptManagerInstance;   


namespace HAL
{    

#define SPI_DEBUG 0
  
#define SPI_POLL        1  
  
#define SPI_MASTER_Q    1 

#define SPI_MASTER_INTR     1  
#define SPI_SLAVE_INTR      1 
#define SPI_MASTER_DMA      1
#define SPI_SLAVE_DMA       1  
  
  class SpiBase 
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
    
    
    typedef struct
    {
      uint8_t           TxLen;
      uint8_t           RxLen; 
      uint8_t*          TxBuf; 
      uint8_t*          RxBuf;
      SPICallback_t     XferDoneCallback;
    }MasterTxn_t;
    
    typedef struct
    {
      SpiBuf_t*            TxBuf;
      SpiBuf_t*            RxBuf;
      SPICallback_t     XferDoneCallback;
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
      SPI_FULL_DUPLEX_TX_8_BIT,
      SPI_FULL_DUPLEX_RX_8_BIT,
      SPI_FULL_DUPLEX_TXRX_8_BIT,
      SPI_FULL_DUPLEX_TX_16_BIT,
      SPI_FULL_DUPLEX_RX_16_BIT,
      SPI_FULL_DUPLEX_TXRX_16_BIT,
      SPI_FULL_HALF_DUPLEX_TX_8_BIT,
      SPI_FULL_HALF_DUPLEX_RX_8_BIT,
      SPI_FULL_HALF_DUPLEX_TX_16_BIT,
      SPI_FULL_HALF_DUPLEX_RX_16_BIT,
      SPI_XFER_MODE_DEFAULT,
    }SPI_Xfer_Mode_t;    
    
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
    
    SpiBase(SpiPort_t SpiPort, Hz_t Hz = 100000U);

    SpiStatus_t HwInit(bool Slave = false, XferMode_t mode = SPI_8BIT);
    
    template<Port_t ClkPort, PIN_t ClkPin,Port_t MisoPort, PIN_t MisoPin,Port_t MosiPort, PIN_t MosiPin>
    void MasterPinsHwInit();
    
    template<Port_t ChipSelectPort, PIN_t ChipSelectPin, Port_t ClkPort, PIN_t ClkPin,Port_t MisoPort, PIN_t MisoPin,Port_t MosiPort, PIN_t MosiPin>
    void SlavePinsHwInit();
    
    SpiStatus_t HwDeinit();
    
    void Enable(){LL_SPI_Enable(m_SPIx);}
    
    void Disable(){LL_SPI_Disable(m_SPIx);}
    
    SPIState_t GetState(){return m_SPIState;}   
    
    SpiStatus_t CheckAndLoadTxn(Transaction_t const *pTransaction);
   
    SpiStatus_t CheckAndLoadTxn(uint8_t* TxBuf, uint32_t TxLen,uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS);   
    
  
    void SetBaudrate(Spi_Baudrate_t Spi_Baudrate) { if(m_Baudrate != Spi_Baudrate) { m_Baudrate = Spi_Baudrate; LL_SPI_SetBaudRatePrescaler(m_SPIx,(uint32_t)Spi_Baudrate);} }
    
    void SetBaudrate() { if(m_Baudrate != m_Transaction.Spi_Baudrate) { m_Baudrate = m_Transaction.Spi_Baudrate; LL_SPI_SetBaudRatePrescaler(m_SPIx,(uint32_t)m_Transaction.Spi_Baudrate);} }    
    
  private:
    SPIx_t                  m_SPIx;
    Hz_t                    m_hz;     
    Spi_Baudrate_t          m_Baudrate;
    Transaction_t           m_Transaction;
    
    /* It must be volatile becoz it is shared between ISR and main loop */
    volatile SPIState_t     m_SPIState; 
    /* It must be volatile becoz it is shared between ISR and main loop */
    volatile SpiStatus_t    m_SpiStatus;
    
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

#endif //SpiBase_h