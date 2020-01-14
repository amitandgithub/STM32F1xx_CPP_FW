/******************
** CLASS: SpiDMA
**
** DESCRIPTION:
**  Implements the SpiDMA class
**
** CREATED: 13/1/2020, by Amit Chaudhary
**
** FILE: SpiDMA.cpp
**
*/

#include"SpiDMA.h"

namespace HAL
{   
  SpiDMA::SpiDMA(SpiPort_t SpiPort, Hz_t Hz) :
#if SPI_MASTER_DMA || SPI_SLAVE_DMA
    m_SPI1_DMA_Tx_Callback(this),
    m_SPI2_DMA_Tx_Callback(this)
#endif
#if SPI_SLAVE_DMA
      ,m_SPI1_DMA_Rx_Callback(this),
      m_SPI2_DMA_Rx_Callback(this)
#endif
      {
      
      }    
    
    SpiDMA::SpiStatus_t SpiDMA::HwInit(bool Slave, XferMode_t mode)
    {     
 			
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
      
      if(m_SPIx == SPI1)
      {
#if SPI_MASTER_DMA  || SPI_SLAVE_DMA 
        dma1.HwInit();
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel3_IRQn,0,&m_SPI1_DMA_Tx_Callback);
#endif
#if SPI_SLAVE_DMA     
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel2_IRQn,0,&m_SPI1_DMA_Rx_Callback); 
#endif        
      }                
      else if(m_SPIx == SPI2)
      {
#if  SPI_MASTER_DMA  || SPI_SLAVE_DMA    
        dma1.HwInit();
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel5_IRQn,0,&m_SPI2_DMA_Tx_Callback);
#endif
#if SPI_SLAVE_DMA     
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel4_IRQn,0,&m_SPI2_DMA_Rx_Callback);
#endif  
      }   
      else
      {
        while(1); // Fatal Error
      }    
      dma1.XferConfig(&DMAConfig, m_Curent_DMA_Tx_Channel);
      dma1.XferConfig(&DMAConfig, m_Curent_DMA_Rx_Channel); 
      dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
      dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Rx_Channel);
      
      return SPI_OK;            
    }        
    
    
    
    SpiDMA::SpiStatus_t SpiDMA::TxDMA(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
    {   
      SpiStatus_t SpiStatus = CheckAndLoadTxn(TxBuf,TxLen,TxBuf,0,mode,Spi_Baudrate,XferDoneCallback,pCS);
      
      if(SpiStatus != SPI_OK) return SpiStatus;
      
      m_SPIState = SPI_MASTER_TX_DMA;          
      
      LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen); 
      
      LL_SPI_EnableDMAReq_TX(m_SPIx);
      
      return SPI_OK;        
    }
    
    SpiDMA::SpiStatus_t SpiDMA::RxDMA(uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
    {      
      SpiStatus_t SpiStatus = CheckAndLoadTxn(RxBuf,0,RxBuf,RxLen,mode,Spi_Baudrate,XferDoneCallback,pCS);
      
      if(SpiStatus != SPI_OK) return SpiStatus;
      
      LL_SPI_ClearFlag_OVR(m_SPIx);
      
      m_SPIState = SPI_MASTER_RX_DMA;        
      
      /* Load DMA Rx transaction*/    
      LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
      
      LL_SPI_EnableDMAReq_RX(m_SPIx);
      
      LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);   
      
      LL_SPI_EnableDMAReq_TX(m_SPIx);
      
      return SPI_OK;        
    }
    
    SpiDMA::SpiStatus_t SpiDMA::TxRxDMA(Transaction_t const * pTransaction)
    {      
      
      m_SpiStatus = CheckAndLoadTxn(pTransaction);
      
      if(m_SpiStatus != SPI_OK) return m_SpiStatus;
      
      LL_SPI_ClearFlag_OVR(m_SPIx);
      
      m_SPIState = SPI_MASTER_TXRX_DMA;       
      
      /* Load DMA Tx transaction*/  
      LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);
      
      /* Load DMA Rx transaction*/    
      LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
      
      LL_SPI_EnableDMAReq_TX(m_SPIx);
      LL_SPI_EnableDMAReq_RX(m_SPIx);
      
      return m_SpiStatus;        
    } 
    
    SpiDMA::SpiStatus_t SpiDMA::XferDMA(Transaction_t const * pTransaction)
    {      
      m_SpiStatus = CheckAndLoadTxn(pTransaction);
      
      if(m_SpiStatus != SPI_OK) return m_SpiStatus;
      
      if(pTransaction->TxLen > 0)
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
      
      return m_SpiStatus;        
    } 
    
    void SpiDMA::TxnDoneHandler_DMA()
    {  
      SpiStatus_t SpiStatus;
      
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
        DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );    
        
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT))           SpiStatus = SPI_TXE_TIMEOUT;          
        else if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))         SpiStatus = SPI_BUSY_TIMEOUT;
        else SpiStatus = SPI_OK;
        
        SPI_CS_HIGH();
        
        if( m_Transaction.XferDoneCallback) 
          m_Transaction.XferDoneCallback->CallbackFunction(SpiStatus);
        
        SPI_LOG_STATES(SPI::SPI_LOG_DMA_TX_DONE);
        
#if SPI_MASTER_Q   
        // Check if any transaction pending in Txn Queue, if yes then load the next transaction
        Transaction_t const* _pCurrentTxn;
        if(m_SPITxnQueue.AvailableEnteries())
        {         
          SPI_CS_LOW();
          
          m_SPITxnQueue.Read(&_pCurrentTxn);
          memcpy(&m_Transaction,_pCurrentTxn,sizeof(Transaction_t));
          
          SetBaudrate();
          
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
          return;
        }       
#endif    // SPI_MASTER_Q    
        m_SPIState = SPI_READY;
      }                              
    }
    
    void SpiDMA::SPI1_DMA_MasterTx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if (LL_DMA_IsActiveFlag_TC3(dma1.m_DMAx))
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Tx_Channel) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable all the DMA interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(SpiDMA::m_Curent_DMA_Tx_Channel);
          dma1.DisableTransferCompleteInterrupt(SpiDMA::m_Curent_DMA_Tx_Channel);
          dma1.DisableTransferErrorInterrupt(SpiDMA::m_Curent_DMA_Tx_Channel);
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
        
        TxnDoneHandler_DMA();  
        
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
    
    void SpiDMA::SPI2_DMA_MasterTx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if(LL_DMA_IsActiveFlag_TC5(dma1.m_DMAx))
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Tx_Channel) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable all the DMA interrupt */
          dma1.DisableHalfTransferCompleteInterrupt(SpiDMA::m_Curent_DMA_Tx_Channel);
          dma1.DisableTransferCompleteInterrupt(SpiDMA::m_Curent_DMA_Tx_Channel);
          dma1.DisableTransferErrorInterrupt(SpiDMA::m_Curent_DMA_Tx_Channel);
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
        
        TxnDoneHandler_DMA();     
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
    
    
#endif //SPI_MASTER_DMA
    
#if SPI_SLAVE_DMA  
    
    //SPI1_RX_DMA_CHANNEL = 2;
    void SpiDMA::SPI1_DMA_SlaveRx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if (LL_DMA_IsActiveFlag_TC2(dma1.m_DMAx))
      {        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_TC2(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT2(dma1.m_DMAx); 
        
        LL_SPI_DisableDMAReq_RX(m_SPIx);        
        
        if( m_Transaction.XferDoneCallback) 
          m_Transaction.XferDoneCallback->CallbackFunction(SPI_SLAVE_RX_DMA_DONE);       
      }                
    }
    
    //SPI1_TX_DMA_CHANNEL = 3;
    void SpiDMA::SPI1_DMA_SlaveTx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if (LL_DMA_IsActiveFlag_TC3(dma1.m_DMAx))
      {        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_TC3(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT3(dma1.m_DMAx);
        
        LL_SPI_DisableDMAReq_TX(m_SPIx);       
        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT))
        {
          m_SpiStatus =  SPI_TXE_TIMEOUT;
        }
        else
        {
          m_SpiStatus = SPI_SLAVE_TX_DMA_DONE;
        } 
        
        if( m_Transaction.XferDoneCallback) 
          m_Transaction.XferDoneCallback->CallbackFunction(m_SpiStatus);
      }                
    }    
    //SPI2_RX_DMA_CHANNEL = 4;
    void SpiDMA::SPI2_DMA_SlaveRx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if (LL_DMA_IsActiveFlag_TC4(dma1.m_DMAx))
      {        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_TC4(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT4(dma1.m_DMAx); 
        
        LL_SPI_DisableDMAReq_RX(m_SPIx);        
        
        if( m_Transaction.XferDoneCallback) 
          m_Transaction.XferDoneCallback->CallbackFunction(SPI_SLAVE_RX_DMA_DONE);  
      }                
    }
    
    //SPI2_TX_DMA_CHANNEL = 5;
    void SpiDMA::SPI2_DMA_SlaveTx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if (LL_DMA_IsActiveFlag_TC5(dma1.m_DMAx))
      {        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_TC5(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT5(dma1.m_DMAx);
        
        LL_SPI_DisableDMAReq_TX(m_SPIx);       
        
        if( m_Transaction.XferDoneCallback) 
          m_Transaction.XferDoneCallback->CallbackFunction(SPI_SLAVE_TX_DMA_DONE);     
      }                
    }
    
    void SpiDMA::SrartListeningTXDma(uint8_t* Buf, uint16_t Len)
    {
      // LL_SPI_ClearFlag_OVR(m_SPIx);
      m_SPIState = SPI_SLAVE_TX_DMA;
      LoadTxDmaChannel(Buf,Len); // oopps, 198 cycles 
      LL_SPI_EnableDMAReq_TX(m_SPIx);
    }
    
    void SpiDMA::SrartListeningRXDma(uint8_t* Buf, uint16_t Len)
    {
      LL_SPI_ClearFlag_OVR(m_SPIx);
      m_SPIState = SPI_SLAVE_RX_DMA;
      LoadRxDmaChannel(Buf,Len);
      LL_SPI_EnableDMAReq_RX(m_SPIx);
    } 
    
    void SpiDMA::SPI1_DMA_Rx_Callback::ISR( )
    {
      _this->SPI1_DMA_SlaveRx_Done_Handler(); 
    } 
    
    void SpiDMA::SPI2_DMA_Rx_Callback::ISR( )
    {
      _this->SPI2_DMA_SlaveRx_Done_Handler(); 
    } 
#endif //SPI_SLAVE_DMA
    
#if SPI_MASTER_DMA  || SPI_SLAVE_DMA   
    
    void SpiDMA::SPI1_DMA_Tx_Callback::ISR( )
    {      
#if SPI_MASTER_DMA && SPI_SLAVE_DMA 
      if((_this->m_SPIState == SPI_MASTER_TX_DMA) || (_this->m_SPIState == SPI_MASTER_RX_DMA) )
      {
        _this->SPI1_DMA_MasterTx_Done_Handler();    
      }
      else
      {
        _this->SPI1_DMA_SlaveTx_Done_Handler(); 
      }
#elif SPI_MASTER_DMA
      _this->SPI1_DMA_MasterTx_Done_Handler();    
#elif SPI_SLAVE_DMA
      _this->SPI1_DMA_SlaveTx_Done_Handler(); 
#endif
    }    
    
    
    void SpiDMA::SPI2_DMA_Tx_Callback::ISR( )
    {      
#if SPI_MASTER_DMA && SPI_SLAVE_DMA 
      if((_this->m_SPIState == SPI_MASTER_TX_DMA) || (_this->m_SPIState == SPI_MASTER_RX_DMA) )
      {
        _this->SPI2_DMA_MasterTx_Done_Handler();    
      }
      else
      {
        _this->SPI2_DMA_SlaveTx_Done_Handler(); 
      }
#elif SPI_MASTER_DMA
      _this->SPI2_DMA_MasterTx_Done_Handler();    
#elif SPI_SLAVE_DMA
      _this->SPI2_DMA_SlaveTx_Done_Handler(); 
#endif
    }   
    
#endif // SPI_MASTER_DMA || SPI_SLAVE_DMA    
    
    
}