/******************
** FILE: Spi.cpp
**
** DESCRIPTION:
**  Spi implementation
**
** CREATED: 8/11/2019, by Amit Chaudhary
******************/

#include "Spi.h"

namespace HAL
{
  
  Spi::Spi(SpiPort_t SpiPort, Hz_t Hz) :
    m_hz(Hz),
    m_SPIState(SPI_RESET)
#if SPI_MASTER_DMA
      ,m_SPI1_DMA_Tx_Callback(this),
      m_SPI2_DMA_Tx_Callback(this)
#endif
      {
        m_Transaction.TxLen = 0;
        m_Transaction.RxLen = 0;
        m_Transaction.Spi_Baudrate = SPI_BAUDRATE_DIV2;
        
        if(SpiPort  == SPI1_A5_A6_A7)
        {
          m_SPIx = SPI1;    
          m_Curent_DMA_Tx_Channel = SPI1_TX_DMA_CHANNEL;
          m_Curent_DMA_Rx_Channel = SPI1_RX_DMA_CHANNEL;
        }
        else if(SpiPort  == SPI2_B13_B14_B15)
        {
          m_SPIx = SPI2;
          m_Curent_DMA_Tx_Channel = SPI2_TX_DMA_CHANNEL;
          m_Curent_DMA_Rx_Channel = SPI2_RX_DMA_CHANNEL;
        }
        else
        {
          while(1); // Fatal Error
        }  
        
        m_SpiStatus = SPI_OK;
      }    
    
    Spi::SpiStatus_t Spi::HwInit(void *pInitStruct)
    {     
      LL_SPI_InitTypeDef SPI_InitStruct;
#if SPI_DEBUG
      DebugLogInstance.Enable(Utils::DebugLog<DBG_LOG_TYPE>::DBG_LOG_MODULE_ID_SPI);
#endif            
      /* Set SPI_InitStruct fields to default values */
      SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;//LL_SPI_HALF_DUPLEX_TX;//LL_SPI_FULL_DUPLEX;
      SPI_InitStruct.Mode              = LL_SPI_MODE_MASTER;
      SPI_InitStruct.DataWidth         = LL_SPI_DATAWIDTH_8BIT;
      SPI_InitStruct.ClockPolarity     = LL_SPI_POLARITY_LOW;
      SPI_InitStruct.ClockPhase        = LL_SPI_PHASE_1EDGE;
      SPI_InitStruct.NSS               = LL_SPI_NSS_SOFT;//LL_SPI_NSS_HARD_OUTPUT;
      SPI_InitStruct.BaudRate          = LL_SPI_BAUDRATEPRESCALER_DIV2;
      SPI_InitStruct.BitOrder          = LL_SPI_MSB_FIRST;
      SPI_InitStruct.CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
      SPI_InitStruct.CRCPoly           = 10U;         
      
      if(m_SPIx == SPI1)
      {
        DigitalOut<A5,OUTPUT_AF_PUSH_PULL,LL_GPIO_MODE_OUTPUT_50MHz> sck;
        DigitalIn<A6,INPUT_PULLUP>                                  miso;
        DigitalOut<A7,OUTPUT_AF_PUSH_PULL,LL_GPIO_MODE_OUTPUT_50MHz> mosi;
        sck.HwInit();
        miso.HwInit();
        mosi.HwInit();
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
      }
      else if(m_SPIx == SPI2)
      {
        DigitalOut<B13,OUTPUT_AF_PUSH_PULL,LL_GPIO_MODE_OUTPUT_50MHz> sck;
        DigitalOut<B14,INPUT_PULLUP>      miso;
        DigitalOut<B15,OUTPUT_AF_PUSH_PULL,LL_GPIO_MODE_OUTPUT_50MHz> mosi;
        sck.HwInit();
        miso.HwInit();
        mosi.HwInit();
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
      }
      else
      {
        while(1);
      }
      
      if(m_SPIx == nullptr)
      {
        // Remap 
      }
      
      if(pInitStruct == nullptr)
      {
        LL_SPI_Init(m_SPIx, &SPI_InitStruct);
      }
      else
      {
        LL_SPI_Init(m_SPIx, (LL_SPI_InitTypeDef*)pInitStruct);
      }     
      
#if (SPI_MASTER_DMA == 1) || (SPI_SLAVE_DMA == 1)			
      DMA::DMAConfig_t DMAConfig; 
      
      /* Set DMA_InitStruct fields to default values */
      DMAConfig.PeriphOrM2MSrcAddress   = 0;
      DMAConfig.MemoryOrM2MDstAddress   = 0;
      DMAConfig.Direction               = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
      DMAConfig.Mode                    = LL_DMA_MODE_NORMAL;//LL_DMA_MODE_NORMAL;
      DMAConfig.PeriphOrM2MSrcIncMode   = LL_DMA_MEMORY_NOINCREMENT;
      DMAConfig.MemoryOrM2MDstIncMode   = LL_DMA_MEMORY_INCREMENT;
      DMAConfig.PeriphOrM2MSrcDataSize  = LL_DMA_PDATAALIGN_BYTE;
      DMAConfig.MemoryOrM2MDstDataSize  = LL_DMA_MDATAALIGN_BYTE;
      DMAConfig.NbData                  = 0;
      DMAConfig.Priority                = LL_DMA_PRIORITY_LOW;
#endif            
      
      if(m_SPIx == SPI1)
      {
#if SPI_MASTER_INTR || SPI_MASTER_DMA || SPI_SLAVE_INTR || SPI_SLAVE_DMA       
        InterruptManagerInstance.RegisterDeviceInterrupt(SPI1_IRQn,0,this);
#endif
#if SPI_MASTER_DMA   
        dma1.HwInit();
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel3_IRQn,0,&m_SPI1_DMA_Tx_Callback);
        dma1.XferConfig(&DMAConfig, m_Curent_DMA_Tx_Channel);
        dma1.XferConfig(&DMAConfig, m_Curent_DMA_Rx_Channel);  
        dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
        dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Rx_Channel);
#endif
#if SPI_SLAVE_DMA     
        dma1.HwInit();
        dma1.XferConfig(&DMAConfig, m_Curent_DMA_Tx_Channel);
        dma1.XferConfig(&DMAConfig, m_Curent_DMA_Rx_Channel);  
#endif        
      }                
      else if(m_SPIx == SPI2)
      {
#if SPI_MASTER_INTR || SPI_MASTER_DMA || SPI_SLAVE_INTR || SPI_SLAVE_DMA
        InterruptManagerInstance.RegisterDeviceInterrupt(SPI2_IRQn,0,this);
#endif
#if (SPI_MASTER_DMA == 1) 
        dma1.HwInit();
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel5_IRQn,0,&m_SPI2_DMA_Tx_Callback);
        dma1.XferConfig(&DMAConfig, m_Curent_DMA_Tx_Channel);
        dma1.XferConfig(&DMAConfig, m_Curent_DMA_Rx_Channel); 
        dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
        dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Rx_Channel);
#endif
      }   
      else
      {
        while(1); // Fatal Error
      }
      
      Enable();
      
      //m_SpiStatus = SPI_OK;
      m_SPIState = SPI_READY;
      
      return SPI_OK;            
    }
    
    
#if SPI_POLL    
    
    Spi::SpiStatus_t Spi::TxPoll(uint8_t* TxBuf, uint32_t TxLen, uint8_t Options)
    {      
      if(TxBuf == nullptr)  return SPI_INVALID_PARAMS;           
      
      while(TxLen != 0)
      {        
        m_SPIx->DR = *TxBuf++;
        TxLen--;
        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;   
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    Spi::SpiStatus_t Spi::RxPoll(uint8_t* RxBuf, uint32_t RxLen, uint8_t Options)
    {      
      if(RxBuf == nullptr) return SPI_INVALID_PARAMS;
      
      LL_SPI_ClearFlag_OVR(m_SPIx);
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
      
      while(RxLen != 0)
      {
        m_SPIx->DR = 0xFF;
        
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_RXNE_TIMEOUT; 
        
        *RxBuf++ = m_SPIx->DR;        
        RxLen--;
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    Spi::SpiStatus_t Spi::TxRxPoll(uint8_t* TxBuf, uint8_t* RxBuf, uint32_t Len, uint8_t Options)
    {      
      if((TxBuf == nullptr) || (RxBuf == nullptr)) return SPI_INVALID_PARAMS;        
      
      while(Len != 0)
      {        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
        
        m_SPIx->DR = *TxBuf++;
        
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_RXNE_TIMEOUT; 
        
        *RxBuf++ = m_SPIx->DR;        
        Len--;
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    Spi::SpiStatus_t Spi::XferPoll(Transaction_t const *pTransaction)
    {
      SpiStatus_t SpiStatus;
      
      if(pTransaction == nullptr) return SPI_INVALID_PARAMS;
      
      if(pTransaction->TxLen)
      {
        SpiStatus = TxPoll(pTransaction->TxBuf,pTransaction->TxLen);
      }
      
      if(pTransaction->RxLen)
      {
        SpiStatus =  RxPoll(pTransaction->RxBuf,pTransaction->RxLen);
      }
      return SpiStatus;
    }
    
#endif // SPI_POLL
    
#if SPI_MASTER_INTR
    
    Spi::SpiStatus_t Spi::TxIntr(uint8_t* TxBuf, uint32_t TxLen, SPICallback_t XferDoneCallback,Port_t CSPort, uint16_t CSPinmask)
    {      
      if(TxBuf == nullptr) return SPI_INVALID_PARAMS;
      
      m_Transaction.TxBuf = TxBuf;
      m_Transaction.TxLen = TxLen;
      m_Transaction.RxBuf = nullptr;
      m_Transaction.RxLen = 0;
      m_Transaction.CSPort = CSPort;
      m_Transaction.CSPinmask = CSPinmask;
      m_Transaction.XferDoneCallback = XferDoneCallback;
      
      if(TxLen > 0)
      {        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
        
        m_SPIState = SPI_MASTER_TX;  
        
        SPI_BYTE_OUT_8_BIT(m_Transaction); 
        
        EnableInterrupt(SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
        
        return SPI_OK;        
      }      
      return SPI_INVALID_PARAMS;
    }
    
    Spi::SpiStatus_t Spi::RxIntr(uint8_t* RxBuf, uint32_t RxLen,SPICallback_t XferDoneCallback,Port_t CSPort, uint16_t CSPinmask)
    {      
      if(RxBuf == nullptr) return SPI_INVALID_PARAMS;
      
      m_Transaction.TxBuf = nullptr;
      m_Transaction.TxLen = 0;
      m_Transaction.RxBuf = RxBuf;
      m_Transaction.RxLen = RxLen;
      m_Transaction.CSPort = CSPort;
      m_Transaction.CSPinmask = CSPinmask;
      m_Transaction.XferDoneCallback = XferDoneCallback;
      
      LL_SPI_ClearFlag_OVR(m_SPIx);
      
      if(RxLen > 0)
      {       
        m_SPIState = SPI_MASTER_RX;
        
        m_SPIx->DR = 0xff;
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
        
        EnableInterrupt(SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
        
        return SPI_OK;        
      }      
      return SPI_INVALID_PARAMS;
    }
    
    Spi::SpiStatus_t Spi::XferIntr(Transaction_t const *pTransaction)
    {
      if(pTransaction == nullptr) return SPI_INVALID_PARAMS;
      
      // 13'035
      //      m_Transaction.TxBuf = pTransaction->TxBuf;
      //      m_Transaction.TxLen = pTransaction->TxLen;
      //      m_Transaction.RxBuf = pTransaction->RxBuf;
      //      m_Transaction.RxLen = pTransaction->RxLen;
      //      m_Transaction.XferDoneCallback = pTransaction->XferDoneCallback;
      
      memcpy(&m_Transaction,pTransaction,sizeof(Transaction_t)); // 13'031
      
      if((m_Transaction.TxLen) || (m_Transaction.RxLen))
      {
        if(m_Transaction.TxLen)
        {
          if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
          
          m_SPIState = SPI_MASTER_TX;            
          // This clears the TXE flag      
          SPI_BYTE_OUT_8_BIT(m_Transaction);           
        }
        else
        {
          m_SPIState = SPI_MASTER_RX;
          
          m_SPIx->DR = 0xff;
          if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;                
        }
        
        EnableInterrupt(SPI_CR2_RXNEIE | SPI_CR2_ERRIE );  
        return SPI_OK;
      }
      
      return SPI_INVALID_PARAMS; 
    }    
    
    void Spi::ISR()
    {             
      
      if( SPI_RXNE(m_SPIx) )
      {
        if(m_SPIState == SPI_MASTER_TX)
        {
          if(m_Transaction.TxLen > 0)
          {
            SPI_BYTE_OUT_8_BIT(m_Transaction);       
          }
          else
          {            
            
            if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
            {
              m_SPIState = SPI_READY;
              
              if( m_Transaction.XferDoneCallback) 
                m_Transaction.XferDoneCallback->CallbackFunction(SPI_BUSY_TIMEOUT); // should pass endstatus into callback here
            }
            else
            {              
              // if something to receive, start Rx mode here, else end the transaction
              if(m_Transaction.RxLen > 0)
              {
                m_SPIState = SPI_MASTER_RX;
                LL_SPI_ClearFlag_OVR(m_SPIx);
                m_SPIx->DR = 0xff; // clock out to receive data
              }
              else               
              {
                m_SPIState = SPI_READY;           
                
                if( m_Transaction.XferDoneCallback) 
                  m_Transaction.XferDoneCallback->CallbackFunction(SPI_OK); // should pass endstatus into callback here
                
                SPI_CS_HIGH();
                
                DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
              }
            }            
          }
        }
        else  if(m_SPIState == SPI_MASTER_RX)
        {
          if(m_Transaction.RxLen > 0)
          {            
            SPI_BYTE_IN_8_BIT(m_Transaction); 
            m_SPIx->DR = 0xff; // dummy write
          }
          else
          {
            SPI_CS_HIGH();
            
            if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
            {
              if( m_Transaction.XferDoneCallback) 
                m_Transaction.XferDoneCallback->CallbackFunction(SPI_BUSY_TIMEOUT);
            }
            else
            {              
              if( m_Transaction.XferDoneCallback) 
                m_Transaction.XferDoneCallback->CallbackFunction(SPI_OK);
#if SPI_MASTER_Q   
              // Check if any transaction pending in Txn Queue, if yes then load the next transaction
              Transaction_t const* _pCurrentTxn;
              if(m_SPITxnQueue.AvailableEnteries())
              {
                m_SPITxnQueue.Read(&_pCurrentTxn);
                memcpy(&m_Transaction,_pCurrentTxn,sizeof(Transaction_t));   
                
                SPI_CS_LOW();
                
                if(m_Transaction.TxLen)
                {                  
                  m_SPIState = SPI_MASTER_TX;                  
                  SPI_BYTE_OUT_8_BIT(m_Transaction);           
                }
                else
                {
                  m_SPIState = SPI_MASTER_RX;                
                  m_SPIx->DR = 0xff;                
                }
                return;
              }             
#endif             
            }
            m_SPIState = SPI_READY; 
            DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
          }              
        }
        else  if(m_SPIState == SPI_MASTER_TXRX)
        {
          if(m_Transaction.TxLen > 0) 
          {
            *m_Transaction.RxBuf++ = m_SPIx->DR;  // read received data
            m_SPIx->DR = *m_Transaction.TxBuf++;  // transmit next data
            m_Transaction.TxLen--;
          }
          else
          {
            SPI_CS_HIGH();
            
            if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
            {
              if( m_Transaction.XferDoneCallback) 
                m_Transaction.XferDoneCallback->CallbackFunction(SPI_BUSY_TIMEOUT);
            }
            else
            {                      
              if( m_Transaction.XferDoneCallback) 
                m_Transaction.XferDoneCallback->CallbackFunction(SPI_OK);
            }
            m_SPIState = SPI_READY; 
            DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
          }        
        }
      }// if( SPI_RXNE(m_SPIx) )     
      else
      {
        if(LL_SPI_IsActiveFlag_OVR(m_SPIx))
        {
          LL_SPI_ClearFlag_OVR(m_SPIx);
        }
      }
    }
#endif // SPI_MASTER_INTR
    
    
    
#if SPI_MASTER_Q && (SPI_MASTER_DMA || SPI_MASTER_INTR)
    
    Spi::SpiStatus_t Spi::Post(Transaction_t const *pTransaction, uint32_t Mode)
    {      
      if(m_SPIState == SPI_READY)
      {  
        if(Mode == 0)
        {
#if (SPI_MASTER_INTR == 1)
          return XferIntr(pTransaction); 
#else
          return SPI_INVALID_PARAMS; 
#endif          
        }
        else
        {
#if (SPI_MASTER_DMA == 1)
          return XferDMA(pTransaction);
#else
          return SPI_INVALID_PARAMS; 
#endif          
        }
      }
      else
      {      
        if( (pTransaction == nullptr) || ((pTransaction->TxBuf == nullptr) && (pTransaction->RxBuf == nullptr))  )
        {          
          SPI_DEBUG_LOG(SPI_INVALID_PARAMS);                
          return SPI_INVALID_PARAMS;                
        }        
        if( !m_SPITxnQueue.Write(pTransaction) )
        {
          SPI_LOG_STATES(SPI_LOG_TXN_QUEUED);
          return SPI_TXN_POSTED;
        }
        else 
        {
          SPI_LOG_STATES(SPI_LOG_TXN_QUEUE_ERROR);                    
          return SPI_TXN_QUEUE_ERROR;
        }           
      }
    }
#endif //SPI_MASTER_Q
    
    void Spi::TxnDoneHandler()
    {            
      if(m_Transaction.RxLen != 0)
      {              
        
      }
      else 
      {                
        
        // Transaction ended here, call the completion callback
        if(m_Transaction.XferDoneCallback)
          m_Transaction.XferDoneCallback->CallbackFunction(SPI_OK);
        
        //Load next transaction from Txn queue if any
        //#if  SPI_MASTER_INTR
        //        LoadNextTransaction_MASTER_INTR();
        //#elif SPI_MASTER_DMA
        //        LoadNextTransaction_MASTER_DMA();
        //#endif
      }                         
    }
    
    Spi::SpiStatus_t Spi::CheckAndLoadTxn(Transaction_t const *pTransaction)
    {
      if(m_SPIState != SPI_READY)
        return SPI_BUSY;
      
      if((pTransaction == 0) || ( (!pTransaction->TxBuf) && (!pTransaction->RxBuf) ))
      {
        return SPI_INVALID_PARAMS;
      }
      
      
      /* Wait while BUSY flag is set */
      if (SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
      {
        SPI_LOG_STATES(SPI_LOG_BUSY_TIMEOUT);
        return SPI_BUSY_TIMEOUT; 
      }
      
      m_Transaction.TxBuf              = pTransaction->TxBuf;
      m_Transaction.TxLen              = pTransaction->TxLen;
      m_Transaction.RxBuf              = pTransaction->RxBuf;
      m_Transaction.RxLen              = pTransaction->RxLen;   
      m_Transaction.XferDoneCallback   = pTransaction->XferDoneCallback;
      
      return SPI_OK;      
    }    
    
    
#if SPI_MASTER_DMA
    
    Spi::SpiStatus_t Spi::TxDMA(uint8_t* TxBuf, uint32_t TxLen, SPICallback_t XferDoneCallback,Port_t CSPort, uint16_t CSPinmask)
    {      
      if( (TxBuf == nullptr) || (TxLen == 0) ) return SPI_INVALID_PARAMS;
      
      m_Transaction.TxBuf = TxBuf;
      m_Transaction.TxLen = TxLen;
      m_Transaction.RxLen = 0;
      m_Transaction.CSPort = CSPort;
      m_Transaction.CSPinmask = CSPinmask;
      m_Transaction.XferDoneCallback = XferDoneCallback;
      
      if(TxLen > 0)
      {        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
        
        m_SPIState = SPI_MASTER_TX_DMA;          
        
        LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen); 
        LL_SPI_EnableDMAReq_TX(m_SPIx);
        
        return SPI_OK;        
      }
      
      return SPI_INVALID_PARAMS;
    }
    
    Spi::SpiStatus_t Spi::RxDMA(uint8_t* RxBuf, uint32_t RxLen,SPICallback_t XferDoneCallback,Port_t CSPort, uint16_t CSPinmask)
    {      
      if( (RxBuf == nullptr) || (RxLen == 0) ) return SPI_INVALID_PARAMS;
      
      //m_Transaction.TxBuf = nullptr;
      m_Transaction.TxLen = 0;
      m_Transaction.RxBuf = RxBuf;
      m_Transaction.RxLen = RxLen;
      m_Transaction.CSPort = CSPort;
      m_Transaction.CSPinmask = CSPinmask;
      m_Transaction.XferDoneCallback = XferDoneCallback;
      
      LL_SPI_ClearFlag_OVR(m_SPIx);
      
      if(RxLen > 0)
      {       
        m_SPIState = SPI_MASTER_RX_DMA;        
        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;        
        
        /* Load DMA Rx transaction*/    
        LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
        LL_SPI_EnableDMAReq_RX(m_SPIx);
        
        LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);   

        LL_SPI_EnableDMAReq_TX(m_SPIx);
        
        return SPI_OK;        
      }      
      return SPI_INVALID_PARAMS;
    }
    
    Spi::SpiStatus_t Spi::TxRxDMA(Transaction_t const * pTransaction)
    {     
      if( (pTransaction == nullptr) || (pTransaction->TxLen != pTransaction->RxLen) )  return SPI_INVALID_PARAMS;        
      
      memcpy(&m_Transaction,pTransaction,sizeof(Transaction_t));
      
      LL_SPI_ClearFlag_OVR(m_SPIx);
      
      m_SPIState = SPI_MASTER_TXRX_DMA;     
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;     
      
      /* Load DMA Tx transaction*/  
      LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);
      
      /* Load DMA Rx transaction*/    
      LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
      
      SPI_CS_LOW();
      
      LL_SPI_EnableDMAReq_TX(m_SPIx);
      LL_SPI_EnableDMAReq_RX(m_SPIx);
      
      return SPI_OK;        
    } 
    
    Spi::SpiStatus_t Spi::XferDMA(Transaction_t const * pTransaction)
    {     
      if(pTransaction == nullptr) return SPI_INVALID_PARAMS;
      
      memcpy(&m_Transaction,pTransaction,sizeof(Transaction_t));
      
      if(pTransaction->TxLen > 0)
      {       
        LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);            
        m_SPIState = SPI_MASTER_TX_DMA;
      }
      else if(pTransaction->RxLen > 0)
      {  
        LL_SPI_ClearFlag_OVR(m_SPIx);
        
        /* Load Tx DMA for generating clock for Rx*/ 
        LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
        
        /* Load DMA Rx transaction*/    
        LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
        LL_SPI_EnableDMAReq_RX(m_SPIx);
        
        m_SPIState = SPI_MASTER_RX_DMA;
      }
      else
      {
        return SPI_INVALID_PARAMS;
      }      
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;        
      
      SPI_CS_LOW();
      
      LL_SPI_EnableDMAReq_TX(m_SPIx);
      
      return SPI_OK;        
    } 
    
    void Spi::SPI1_DMA_Tx_Done_Handler()
    {		
      SpiStatus_t SpiStatus;
      /* Transfer Complete Interrupt management ***********************************/
      if (LL_DMA_IsActiveFlag_TC3(dma1.m_DMAx))
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Tx_Channel) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable all the DMA interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(Spi::m_Curent_DMA_Tx_Channel);
          dma1.DisableTransferCompleteInterrupt(Spi::m_Curent_DMA_Tx_Channel);
          dma1.DisableTransferErrorInterrupt(Spi::m_Curent_DMA_Tx_Channel);
        } 
        
        if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Rx_Channel) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable all the DMA interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(m_Curent_DMA_Rx_Channel);
          dma1.DisableTransferCompleteInterrupt(m_Curent_DMA_Rx_Channel);
          dma1.DisableTransferErrorInterrupt(m_Curent_DMA_Rx_Channel);
        } 
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_TC2(dma1.m_DMAx);
        LL_DMA_ClearFlag_TC3(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT2(dma1.m_DMAx); 
        LL_DMA_ClearFlag_HT3(dma1.m_DMAx);
        
        LL_SPI_DisableDMAReq_TX(m_SPIx); 
        LL_SPI_DisableDMAReq_RX(m_SPIx);        
        
        if( (m_SPIState == SPI_MASTER_TX_DMA) && (m_Transaction.RxLen > 0) )
        {
          /* Load Tx DMA for generating clock for Rx*/ 
          LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
          
          /* Load DMA Rx transaction*/    
          LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
          
          LL_SPI_ClearFlag_OVR(m_SPIx);
          
          m_SPIState = SPI_MASTER_RX_DMA; 
          
          LL_SPI_EnableDMAReq_RX(m_SPIx);
          LL_SPI_EnableDMAReq_TX(m_SPIx);
        }
        else
        {  
          SPI_CS_HIGH();
           
          DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );    
          
          if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT))          SpiStatus = SPI_TXE_TIMEOUT;          
          else if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))  SpiStatus = SPI_BUSY_TIMEOUT;
          else SpiStatus = SPI_OK;
          
          if( m_Transaction.XferDoneCallback) 
            m_Transaction.XferDoneCallback->CallbackFunction(SpiStatus);
          
          SPI_LOG_STATES(SPI::SPI_LOG_DMA_TX_DONE);
          
#if SPI_MASTER_Q   
          // Check if any transaction pending in Txn Queue, if yes then load the next transaction
          Transaction_t const* _pCurrentTxn;
          if(m_SPITxnQueue.AvailableEnteries())
          {
            m_SPITxnQueue.Read(&_pCurrentTxn);
            memcpy(&m_Transaction,_pCurrentTxn,sizeof(Transaction_t));
            if(m_Transaction.TxLen)
            {
              LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);            
              m_SPIState = SPI_MASTER_TX_DMA;           
            }
            else
            {
              LL_SPI_ClearFlag_OVR(m_SPIx);
              
              /* Load Tx DMA for generating clock for Rx*/ 
              LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
              
              /* Load DMA Rx transaction*/    
              LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
              LL_SPI_EnableDMAReq_RX(m_SPIx);
              
              m_SPIState = SPI_MASTER_RX_DMA;             
            }
            SPI_CS_LOW();
            LL_SPI_EnableDMAReq_TX(m_SPIx);  
            return;
          }
#endif         
          m_SPIState = SPI_READY;
        }         
      }     
      /* Half Transfer Complete Interrupt management ******************************/
      else if (LL_DMA_IsActiveFlag_HT3(dma1.m_DMAx))
      {
        SPI_LOG_STATES(SPI::SPI_LOG_DMA_HALF_TX_DONE);
        
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Tx_Channel) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable the half transfer interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
        }
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT3(dma1.m_DMAx);
      }      
      /* Transfer Error Interrupt management **************************************/
      else if ( LL_DMA_IsActiveFlag_TE3(dma1.m_DMAx))
      {
        /* When a DMA transfer error occurs */
        /* A hardware clear of its EN bits is performed */
        /* Disable ALL DMA IT */
        /* Disable all the DMA interrupt */
        dma1.DisableHalfTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
        dma1.DisableTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
        dma1.DisableTransferErrorInterrupt(m_Curent_DMA_Tx_Channel);
        
        SPI_LOG_STATES(SPI::SPI_LOG_DMA_TX_ERROR);                
      }            
    }		
    
    void Spi::SPI2_DMA_Tx_Done_Handler()
    {		
      SpiStatus_t SpiStatus;
      /* Transfer Complete Interrupt management ***********************************/
      if(LL_DMA_IsActiveFlag_TC5(dma1.m_DMAx))
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Tx_Channel) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable all the DMA interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(Spi::m_Curent_DMA_Tx_Channel);
          dma1.DisableTransferCompleteInterrupt(Spi::m_Curent_DMA_Tx_Channel);
          dma1.DisableTransferErrorInterrupt(Spi::m_Curent_DMA_Tx_Channel);
        } 
        
        if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Rx_Channel) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable all the DMA interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(m_Curent_DMA_Rx_Channel);
          dma1.DisableTransferCompleteInterrupt(m_Curent_DMA_Rx_Channel);
          dma1.DisableTransferErrorInterrupt(m_Curent_DMA_Rx_Channel);
        } 
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_TC4(dma1.m_DMAx);
        LL_DMA_ClearFlag_TC5(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT4(dma1.m_DMAx); 
        LL_DMA_ClearFlag_HT5(dma1.m_DMAx);
        
        LL_SPI_DisableDMAReq_TX(m_SPIx); 
        LL_SPI_DisableDMAReq_RX(m_SPIx);        
        
        if( (m_SPIState == SPI_MASTER_TX_DMA) && (m_Transaction.RxLen > 0) )
        {
          /* Load Tx DMA for generating clock for Rx*/ 
          LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
          
          /* Load DMA Rx transaction*/    
          LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
          
          LL_SPI_ClearFlag_OVR(m_SPIx);
          
          m_SPIState = SPI_MASTER_RX_DMA; 
          
          LL_SPI_EnableDMAReq_RX(m_SPIx);
          LL_SPI_EnableDMAReq_TX(m_SPIx);
        }
        else
        {         
          SPI_CS_HIGH();
          
          DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );    
          
          if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT))          SpiStatus = SPI_TXE_TIMEOUT;          
          else if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))  SpiStatus = SPI_BUSY_TIMEOUT;
          else SpiStatus = SPI_OK;
          
          if( m_Transaction.XferDoneCallback) 
            m_Transaction.XferDoneCallback->CallbackFunction(SpiStatus);
          
          SPI_LOG_STATES(SPI::SPI_LOG_DMA_TX_DONE);
          
#if SPI_MASTER_Q   
          // Check if any transaction pending in Txn Queue, if yes then load the next transaction
          Transaction_t const* _pCurrentTxn;
          if(m_SPITxnQueue.AvailableEnteries())
          {
            m_SPITxnQueue.Read(&_pCurrentTxn);
            memcpy(&m_Transaction,_pCurrentTxn,sizeof(Transaction_t));
            if(m_Transaction.TxLen)
            {
              LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);            
              m_SPIState = SPI_MASTER_TX_DMA;           
            }
            else
            {
              LL_SPI_ClearFlag_OVR(m_SPIx);
              
              /* Load Tx DMA for generating clock for Rx*/ 
              LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
              
              /* Load DMA Rx transaction*/    
              LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
              LL_SPI_EnableDMAReq_RX(m_SPIx);
              
              m_SPIState = SPI_MASTER_RX_DMA;             
            }
            LL_SPI_EnableDMAReq_TX(m_SPIx);
            
            SPI_CS_LOW();
            
            return;
          }       
#endif        
           m_SPIState = SPI_READY;
        }        
      }     
      /* Half Transfer Complete Interrupt management ******************************/
      else if (LL_DMA_IsActiveFlag_HT5(dma1.m_DMAx))
      {
        SPI_LOG_STATES(SPI::SPI_LOG_DMA_HALF_TX_DONE);
        
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Tx_Channel) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable the half transfer interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
        }
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT5(dma1.m_DMAx);
      }      
      /* Transfer Error Interrupt management **************************************/
      else if ( LL_DMA_IsActiveFlag_TE5(dma1.m_DMAx))
      {
        /* When a DMA transfer error occurs */
        /* A hardware clear of its EN bits is performed */
        /* Disable ALL DMA IT */
        /* Disable all the DMA interrupt */
        dma1.DisableHalfTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
        dma1.DisableTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
        dma1.DisableTransferErrorInterrupt(m_Curent_DMA_Tx_Channel);
        
        SPI_LOG_STATES(SPI::SPI_LOG_DMA_TX_ERROR);                
      }            
    }    
    
    void Spi::SPI1_DMA_Tx_Callback::ISR( )
    {
      _this->SPI1_DMA_Tx_Done_Handler();            
    }    
    
    void Spi::SPI2_DMA_Tx_Callback::ISR()
    {
      _this->SPI2_DMA_Tx_Done_Handler();
    }
    
#endif
    
}


