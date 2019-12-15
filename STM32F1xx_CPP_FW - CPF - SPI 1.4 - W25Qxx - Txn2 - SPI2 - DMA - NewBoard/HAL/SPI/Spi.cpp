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
        if(SpiPort  == SPI1_A5_A6_A7)
        {
          m_SPIx = SPI1;
        }
        else if(SpiPort  == SPI2_B13_B14_B15)
        {
          m_SPIx = SPI2;
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
      SPI_InitStruct.BaudRate          = LL_SPI_BAUDRATEPRESCALER_DIV4;
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
        //        EnableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
#endif
#if SPI_MASTER_DMA   
        dma1.HwInit();
        //InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel2_IRQn,0,&m_SPI1_DMA_Rx_Callback);
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel3_IRQn,0,&m_SPI1_DMA_Tx_Callback);
//        InterruptManagerInstance.EnableInterrupt(DMA1_Channel2_IRQn);
//        InterruptManagerInstance.EnableInterrupt(DMA1_Channel3_IRQn);
        dma1.XferConfig(&DMAConfig, SPI1_TX_DMA_CHANNEL);
        dma1.XferConfig(&DMAConfig, SPI1_RX_DMA_CHANNEL);  
        dma1.EnableTransferCompleteInterrupt(SPI1_TX_DMA_CHANNEL);
        dma1.EnableTransferCompleteInterrupt(SPI1_RX_DMA_CHANNEL);
#endif
#if SPI_SLAVE_DMA     
        dma1.HwInit();
        dma1.XferConfig(&DMAConfig, SPI1_TX_DMA_CHANNEL);
        dma1.XferConfig(&DMAConfig, SPI1_RX_DMA_CHANNEL);  
#endif        
      }                
      else if(m_SPIx == SPI2)
      {
#if SPI_MASTER_INTR || SPI_MASTER_DMA || SPI_SLAVE_INTR || SPI_SLAVE_DMA
        InterruptManagerInstance.RegisterDeviceInterrupt(SPI2_IRQn,0,this);
#endif
#if (SPI_MASTER_DMA == 1) 
        //InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel5_IRQn,0,&m_SPI2_DMA_Rx_Callback);
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel4_IRQn,0,&m_SPI2_DMA_Tx_Callback);
        dma1.EnableTransferCompleteInterrupt(SPI2_TX_DMA_CHANNEL);
        dma1.EnableTransferCompleteInterrupt(SPI2_RX_DMA_CHANNEL);
#endif
      }   
      else
      {
        while(1); // Fatal Error
      }
      
      Enable();
      
      m_SpiStatus = SPI_OK;
      
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
    
    Spi::SpiStatus_t Spi::TxIntr(uint8_t* TxBuf, uint32_t TxLen, SPICallback_t XferDoneCallback)
    {      
      if(TxBuf == nullptr) return SPI_INVALID_PARAMS;
      
      m_Transaction.TxBuf = TxBuf;
      m_Transaction.TxLen = TxLen;
      m_Transaction.RxBuf = nullptr;
      m_Transaction.RxLen = 0;
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
    
    Spi::SpiStatus_t Spi::RxIntr(uint8_t* RxBuf, uint32_t RxLen,SPICallback_t XferDoneCallback)
    {      
      if(RxBuf == nullptr) return SPI_INVALID_PARAMS;
      
      m_Transaction.TxBuf = nullptr;
      m_Transaction.TxLen = 0;
      m_Transaction.RxBuf = RxBuf;
      m_Transaction.RxLen = RxLen;
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
            if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
            {
              // m_SPIState = SPI_READY;
              
              if( m_Transaction.XferDoneCallback) 
                m_Transaction.XferDoneCallback->CallbackFunction(SPI_BUSY_TIMEOUT);
            }
            else
            {                      
              //m_SPIState = SPI_READY;           
              
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
    
    Spi::SpiStatus_t Spi::TxDMA(uint8_t* TxBuf, uint32_t TxLen, SPICallback_t XferDoneCallback)
    {      
      if(TxBuf == nullptr) return SPI_INVALID_PARAMS;
      
      m_Transaction.TxBuf = TxBuf;
      m_Transaction.TxLen = TxLen;
      m_Transaction.RxBuf = nullptr;
      m_Transaction.RxLen = 0;
      m_Transaction.XferDoneCallback = XferDoneCallback;
      
      if(TxLen > 0)
      {        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
        
        m_SPIState = SPI_MASTER_TX_DMA;          
       
        LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen); 
        LL_SPI_EnableDMAReq_TX(m_SPIx);
        //EnableInterrupt(SPI_CR2_ERRIE );
        
        return SPI_OK;        
      }
      
      return SPI_INVALID_PARAMS;
    }
    
    Spi::SpiStatus_t Spi::RxDMA(uint8_t* RxBuf, uint32_t RxLen,SPICallback_t XferDoneCallback)
    {      
      if(RxBuf == nullptr) return SPI_INVALID_PARAMS;
      
      m_Transaction.TxBuf = nullptr;
      m_Transaction.TxLen = 0;
      m_Transaction.RxBuf = RxBuf;
      m_Transaction.RxLen = RxLen;
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
       // EnableInterrupt(SPI_CR2_ERRIE );
        
        return SPI_OK;        
      }
      
      return SPI_INVALID_PARAMS;
    }
    
    Spi::SpiStatus_t Spi::XferDMA(Transaction_t const * pTransaction)
    {     
      SpiStatus_t status;
      
      status = CheckAndLoadTxn(pTransaction);
      
      if(status != SPI_OK) return status; 
      
      if(m_Transaction.TxLen)
      {
        m_SPIState = SPI_MASTER_TX_DMA;
        /* Load DMA Tx transaction*/
        LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);  
      }
      else if(m_Transaction.RxLen)
      {
        m_SPIState = SPI_MASTER_RX_DMA;
      }
      else
      {
        return SPI_INVALID_PARAMS;
      }
      
      /* Load DMA Rx transaction*/    
      LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);          
      
      SPI_LOG_STATES(SPI_LOG_START_MASTER_TX_DMA);
      
      EnableInterrupt(SPI_CR2_ERRIE ); 
      
      return SPI_OK;         
    }    
    
    void Spi::SPI1_DMA_Tx_Done_Handler()
    {		
      SpiStatus_t SpiStatus;
      /* Transfer Complete Interrupt management ***********************************/
      if (LL_DMA_IsActiveFlag_TC3(dma1.m_DMAx))
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,SPI1_TX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable all the DMA interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(Spi::SPI1_TX_DMA_CHANNEL);
          dma1.DisableTransferCompleteInterrupt(Spi::SPI1_TX_DMA_CHANNEL);
          dma1.DisableTransferErrorInterrupt(Spi::SPI1_TX_DMA_CHANNEL);
        } 
        
        if(LL_DMA_GetMode(dma1.m_DMAx,SPI1_RX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable all the DMA interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(SPI1_RX_DMA_CHANNEL);
          dma1.DisableTransferCompleteInterrupt(SPI1_RX_DMA_CHANNEL);
          dma1.DisableTransferErrorInterrupt(SPI1_RX_DMA_CHANNEL);
        } 
                
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_TC2(dma1.m_DMAx);
        LL_DMA_ClearFlag_TC3(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT2(dma1.m_DMAx); 
        LL_DMA_ClearFlag_HT3(dma1.m_DMAx);
        
        LL_SPI_DisableDMAReq_TX(m_SPIx); 
        LL_SPI_DisableDMAReq_RX(m_SPIx); 
        
        // This varoable is only used by TxnDoneHandler() to keep track of Tx completion status
        m_Transaction.TxLen = 0; 
        m_Transaction.RxLen = 0;        
        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT))
          SpiStatus = SPI_TXE_TIMEOUT;
        
        if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
          SpiStatus = SPI_BUSY_TIMEOUT;
        
        if( m_Transaction.XferDoneCallback) 
                m_Transaction.XferDoneCallback->CallbackFunction(SpiStatus);
        
        DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
        
        SPI_LOG_STATES(SPI::SPI_LOG_DMA_TX_DONE);
      }     
      /* Half Transfer Complete Interrupt management ******************************/
      else if (LL_DMA_IsActiveFlag_HT3(dma1.m_DMAx))
      {
        SPI_LOG_STATES(SPI::SPI_LOG_DMA_HALF_TX_DONE);
        
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,SPI1_TX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable the half transfer interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(SPI1_TX_DMA_CHANNEL);
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
        dma1.DisableHalfTransferCompleteInterrupt(SPI1_TX_DMA_CHANNEL);
        dma1.DisableTransferCompleteInterrupt(SPI1_TX_DMA_CHANNEL);
        dma1.DisableTransferErrorInterrupt(SPI1_TX_DMA_CHANNEL);
        
        SPI_LOG_STATES(SPI::SPI_LOG_DMA_TX_ERROR);                
      }            
    }				
    
#if 0
    void Spi::SPI1_DMA_Rx_Done_Handler()
    {			    
      /* Transfer Complete Interrupt management ***********************************/
      if (LL_DMA_IsActiveFlag_TC2(dma1.m_DMAx))
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,SPI1_RX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable all the DMA interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(SPI1_RX_DMA_CHANNEL);
          dma1.DisableTransferCompleteInterrupt(SPI1_RX_DMA_CHANNEL);
          dma1.DisableTransferErrorInterrupt(SPI1_RX_DMA_CHANNEL);
        }          
        
        /* Disable DMA Request */            
        //SPI_DMA_DISABLE(m_SPIx); 
        LL_SPI_DisableDMAReq_RX(m_SPIx);       
        
        /* Clear the transfer complete flag */
        LL_DMA_ClearFlag_TC2(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT2(dma1.m_DMAx);  
        
        // This variable is used by TxnDoneHandler() to keep track of completion status
        m_Transaction.RxLen = 0;
        
        SPI_LOG_STATES(SPI::SPI_LOG_DMA_RX_DONE);
        
        TxnDoneHandler(); 
      }           
      /* Half Transfer Complete Interrupt management ******************************/
      else if (LL_DMA_IsActiveFlag_HT7(dma1.m_DMAx))
      {
        SPI_LOG_STATES(SPI::SPI_LOG_DMA_HALF_RX_DONE);
        
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,SPI1_RX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable the half transfer interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(SPI1_RX_DMA_CHANNEL);
        }
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT2(dma1.m_DMAx);
      }            
      /* Transfer Error Interrupt management **************************************/
      else if ( LL_DMA_IsActiveFlag_TE2(dma1.m_DMAx))
      {
        /* When a DMA transfer error occurs */
        /* A hardware clear of its EN bits is performed */
        /* Disable ALL DMA IT */
        /* Disable all the DMA interrupt */
        dma1.DisableHalfTransferCompleteInterrupt(SPI1_RX_DMA_CHANNEL);
        dma1.DisableTransferCompleteInterrupt(SPI1_RX_DMA_CHANNEL);
        dma1.DisableTransferErrorInterrupt(SPI1_RX_DMA_CHANNEL);                
        
        SPI_LOG_STATES(SPI::SPI_LOG_DMA_RX_ERROR);                
      }            
    }
#endif
    
    
    void Spi::SPI1_DMA_Tx_Callback::ISR( )
    {
      _this->SPI1_DMA_Tx_Done_Handler();            
    }
    
    
    void Spi::SPI2_DMA_Tx_Callback::ISR()
    {
      
    }
    
#endif
    
}


