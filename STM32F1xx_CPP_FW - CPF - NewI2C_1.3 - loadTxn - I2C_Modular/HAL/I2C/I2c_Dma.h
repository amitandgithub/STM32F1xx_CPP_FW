
#ifndef I2c_Dma_h
#define I2c_Dma_h


#include "I2c.h"
#include "I2c_hw.h"


namespace HAL
{

#pragma inline = forced
  void I2c::SB_MASTER_DMA()
    {
      if((m_I2CState == I2C_MASTER_TX_DMA) )
      {
        I2C_DATA_REG(m_I2Cx) = m_Transaction.SlaveAddress & I2C_DIR_WRITE; 
        I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
      }
      else if((m_I2CState == I2C_MASTER_RX_DMA))  
      {
        I2C_DATA_REG(m_I2Cx) = m_Transaction.SlaveAddress | I2C_DIR_READ;
        
        I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX);
      }
      else if(m_I2CState == I2C_MASTER_RX_REPEATED_START)
      {
        /* Repeated start is handled here, clear the flag*/
        m_Transaction.RepeatedStart = 0;         
        
        I2C_DATA_REG(m_I2Cx) = m_Transaction.SlaveAddress | I2C_DIR_READ;  
        
        m_I2CState = I2C_MASTER_RX_DMA;
        I2C_LOG_STATES(I2C_LOG_REPEATED_START_MASTER_RX_DMA);
      }
      else
      {
        while(1);
      }					
    }
#pragma inline = forced    
    void I2c::ADDR_MASTER_DMA()
    {
      if( m_I2CState == I2C_MASTER_TX_DMA)
      {                 
        /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(m_I2Cx);
        
        I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_DMA);
      }
      else if(m_I2CState == I2C_MASTER_RX_DMA)
      {                
        /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(m_I2Cx);
        
        I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_DMA);
      }
      else if((m_I2CState == I2C_READY) || (m_I2CState == I2C_SLAVE_RX_DMA) || (m_I2CState == I2C_MASTER_TX_DMA))    
      {
        // changing state to Slave Tx here
        /* Check the addressing mode*/
        if( (m_I2Cx->SR2 & LL_I2C_SR2_DUALF) == RESET)
        {
          m_Transaction.SlaveAddress = (m_I2Cx->OAR1 & 0x00FE); // Bit 1-7 are address
        }
        else
        {
          m_Transaction.SlaveAddress = (m_I2Cx->OAR2 & 0x00FE); // Bit 1-7 are address
        }            
        /* Transfer Direction requested by Master */
        if( (m_I2Cx->SR2 & LL_I2C_SR2_TRA) == RESET)
        {
          m_I2CState = I2C_SLAVE_RX_DMA;               
        }
        else
        {
          m_I2CState = I2C_SLAVE_TX_DMA;
        }
      }
      else
      {
        while(1);/* Fatal Error*/      
      }
    }
#pragma inline = forced    
    void I2c::BTF_MASTER_DMA()
    {      
      if(m_I2CState == I2C_MASTER_RX_REPEATED_START ) 
      {
        return;
      }
      else if(m_I2CState == I2C_MASTER_TX_DMA)
      {
        I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_DMA_STOP);
        TxnDoneHandler(I2C_CR1_STOP);                     
      }
      else
      {
        while(1);/* Fatal Error*/ 
      }
    }
#pragma inline = forced    
    void I2c::TXE_MASTER_DMA()
    {
      
    }
#pragma inline = forced    
    void I2c::RXNE_MASTER_DMA()
    {
      
    }    
#pragma inline = forced
    void I2c::AF_MASTER_DMA()
    {
      if(m_I2CState == I2C_MASTER_TX)  
      {
        m_I2CStatus = I2C_ACK_FAIL;
        LL_I2C_ClearFlag_AF(m_I2Cx);
        I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ACK_FAIL);
      }
      else if(m_I2CState == I2C_SLAVE_TX ) 
      {
        LL_I2C_ClearFlag_AF(m_I2Cx);  
        
        if(m_SlaveTxn.XferDoneCallback)
          m_SlaveTxn.XferDoneCallback->CallbackFunction(I2C_SLAVE_TX_DONE);	
        
        m_I2CState = I2C_READY;                    
        I2C_LOG_STATES(I2C_LOG_AF_SLAVE_ACK_FAIL);
      }
      else                                
      {
        while(1);/* Fatal Error*/   
      }      
    }
#pragma inline = forced    
    void I2c::STOPF_MASTER_DMA()
    {
      I2C_CLEAR_STOPF(m_I2Cx);  
      
      /* Execute the RxDone Callback */
      if(m_SlaveTxn.XferDoneCallback)
        m_SlaveTxn.XferDoneCallback->CallbackFunction(I2C_SLAVE_TX_DONE);	
      
      m_I2CState = I2C_READY;
      
      I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
    }
#pragma inline = forced   
    void I2c::LoadNextTransaction_MASTER_DMA()
    {      
#if I2C_MASTER_Q      
      Transaction_t*          _pCurrentTxn;
      // Check if there is some transaction pending in Txn Queue
      if(m_I2CTxnQueue.Available())
      {
        m_I2CTxnQueue.Read(_pCurrentTxn);
        if(_pCurrentTxn)
        {
          m_Transaction.SlaveAddress       = _pCurrentTxn->SlaveAddress;
          m_Transaction.TxBuf              = _pCurrentTxn->TxBuf;
          m_Transaction.TxLen              = _pCurrentTxn->TxLen;
          m_Transaction.RxBuf              = _pCurrentTxn->RxBuf;
          m_Transaction.RxLen              = _pCurrentTxn->RxLen;  
          m_Transaction.RepeatedStart      = _pCurrentTxn->RepeatedStart; 
          m_Transaction.XferDoneCallback   = _pCurrentTxn->XferDoneCallback;
        }
        
        if(m_Transaction.TxLen) 
        {
          // Next transaction starts with Tx mode first
          m_I2CState = I2C_MASTER_TX_DMA;
          /* Load DMA Tx transaction*/
          m_DMAx->Load(I2C1_TX_DMA_CHANNEL, (uint32_t)&(I2C_DATA_REG(m_I2Cx)), (uint32_t)m_Transaction.TxBuf, m_Transaction.TxLen, LL_DMA_DIRECTION_MEMORY_TO_PERIPH );
          
          /* Load DMA Rx transaction*/
          m_DMAx->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)&(I2C_DATA_REG(m_I2Cx)), (uint32_t)m_Transaction.RxBuf, m_Transaction.RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY );
          
          /* Enable DMA Request */
          m_I2Cx->CR2 |= I2C_CR2_DMAEN;
        }
        else 
        {
          m_I2CState = I2C_MASTER_RX_DMA; 
          
          /* Load DMA Rx transaction*/
          m_DMAx->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)&(I2C_DATA_REG(m_I2Cx)), (uint32_t)m_Transaction.RxBuf, m_Transaction.RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY );
          
          /* Enable DMA Request */
          m_I2Cx->CR2 |= I2C_CR2_DMAEN;
        }
        
        /* Disable Pos */
        I2C_DISABLE_POS(m_I2Cx);
        
        I2C_ENABLE_ACK_AND_START(m_I2Cx);
        
        I2C_LOG_STATES(I2C_LOG_TXN_DEQUEUED); 
      }
      else
#endif //I2C_MASTER_Q
      {
        
        // Txlen = 0, RxLen = 0, Txn Queue empty
        if (WAIT_FOR_STOP_FLAG_TO_CLEAR(m_I2Cx,I2C_TIMEOUT))
        {
          m_I2CStatus = I2C_STOP_TIMEOUT;
        } 
        else
        {
          I2C_DISABLE_INT_EVT_BUF_ERR(m_I2Cx); 
          
          m_I2CState = I2C_READY;
          
          /* Disable Acknowledge */
          I2C_DISABLE_ACK(m_I2Cx); 
          
          /* Disable Last DMA */
          m_I2Cx->CR2 &= ~I2C_CR2_LAST;
          
          /* Disable DMA Request */            
          m_I2Cx->CR2 &= ~I2C_CR2_DMAEN;               
          
          I2C_LOG_STATES(I2C_LOG_TXN_DONE_ALL);                    
        }
      }
    }
     
  #pragma inline = forced
    void I2c::SB_SLAVE_DMA()
    {
     
    }
#pragma inline = forced    
    void I2c::ADDR_SLAVE_DMA()
    {
      if((m_I2CState == I2C_READY) || (m_I2CState == I2C_SLAVE_RX_DMA) || (m_I2CState == I2C_MASTER_TX_DMA))    
      {
        // changing state to Slave Tx here
        /* Check the addressing mode*/
        if( (m_I2Cx->SR2 & LL_I2C_SR2_DUALF) == RESET)
        {
          m_Transaction.SlaveAddress = (m_I2Cx->OAR1 & 0x00FE); // Bit 1-7 are address
        }
        else
        {
          m_Transaction.SlaveAddress = (m_I2Cx->OAR2 & 0x00FE); // Bit 1-7 are address
        }            
        /* Transfer Direction requested by Master */
        if( (m_I2Cx->SR2 & LL_I2C_SR2_TRA) == RESET)
        {
          m_I2CState = I2C_SLAVE_RX_DMA;               
        }
        else
        {
          m_I2CState = I2C_SLAVE_TX_DMA;
        }
      }
      else
      {
        while(1);/* Fatal Error*/      
      }
      /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(m_I2Cx);
    }
#pragma inline = forced    
    void I2c::BTF_SLAVE_DMA()
    {      
      if(m_I2CState == I2C_MASTER_RX_REPEATED_START ) 
      {
        return;
      }

        I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_DMA_STOP);
        //TxnDoneHandler(I2C_CR1_STOP);                     
    }
#pragma inline = forced    
    void I2c::TXE_SLAVE_DMA()
    {
      
    }
#pragma inline = forced    
    void I2c::RXNE_SLAVE_DMA()
    {
      
    }    
#pragma inline = forced 
    void I2c::AF_SLAVE_DMA()
    {
      if(m_I2CState == I2C_SLAVE_TX ) 
      {
        LL_I2C_ClearFlag_AF(m_I2Cx);  
        
        if(m_SlaveTxn.XferDoneCallback)
          m_SlaveTxn.XferDoneCallback->CallbackFunction(I2C_SLAVE_TX_DONE);	
        
        m_I2CState = I2C_READY;                    
        I2C_LOG_STATES(I2C_LOG_AF_SLAVE_ACK_FAIL);
      }
      else                                
      {
        while(1);/* Fatal Error*/   
      }      
    }
#pragma inline = forced    
    void I2c::STOPF_SLAVE_DMA()
    {
      I2C_CLEAR_STOPF(m_I2Cx);  
      
      m_SlaveTxn.RxBuf->Len = m_DMAx->GetDataLen(I2C1_RX_DMA_CHANNEL);

      /* Execute the RxDone Callback */
      if(m_SlaveTxn.XferDoneCallback)
        m_SlaveTxn.XferDoneCallback->CallbackFunction(I2C_SLAVE_TX_DONE);	
      
      m_I2CState = I2C_READY;
      
      I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
    }
    
#if (I2C_MASTER_DMA == 1) || (I2C_SLAVE_DMA == 1)
     
    I2c::I2CStatus_t I2c::XferDMA(Transaction_t* pTransaction)
    {     
      I2CStatus_t status;
      
       status = CheckAndLoadTxn(pTransaction);
      
      if(status != I2C_OK) return status; 
      
      if(m_Transaction.TxLen)
      {
        m_I2CState = I2C_MASTER_TX_DMA;
        /* Load DMA Tx transaction*/ 
        m_DMAx->Load(I2C1_TX_DMA_CHANNEL, (uint32_t)&(I2C_DATA_REG(m_I2Cx)), (uint32_t)m_Transaction.TxBuf, m_Transaction.TxLen, LL_DMA_DIRECTION_MEMORY_TO_PERIPH ); 
      }
      else if(m_Transaction.RxLen)
      {
        m_I2CState = I2C_MASTER_RX_DMA;
        /* Load DMA Rx transaction*/
       // m_DMAx->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)&(I2C_DATA_REG(m_I2Cx)), (uint32_t)m_Transaction.RxBuf, m_Transaction.RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY );        
      }
      else
      {
        return I2C_INVALID_PARAMS;
      }
      
      m_DMAx->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)&(I2C_DATA_REG(m_I2Cx)), (uint32_t)m_Transaction.RxBuf, m_Transaction.RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY ); 
      
      /* Enable Last DMA bit */
      m_I2Cx->CR2 |= I2C_CR2_LAST;
      
      /* Disable Pos */
      I2C_DISABLE_POS(m_I2Cx);           
      
      I2C_ENABLE_INT_EVT_ERR(m_I2Cx);
      
      /* Enable Acknowledge, Generate Start */
      I2C_ENABLE_ACK_AND_START(m_I2Cx); 
      
      I2C_LOG_STATES(I2C_LOG_START_MASTER_TX_DMA);
      
      /* Enable DMA Request */
      m_I2Cx->CR2 |= I2C_CR2_DMAEN;
      
      return I2C_OK;         
    }     
#if I2C_MASTER_DMA == 1
    
  
#endif // I2C_MASTER_DMA
    
    void I2c::I2C1_DMA_Tx_Done_Handler()
    {			      
      /* Transfer Complete Interrupt management ***********************************/
      if (LL_DMA_IsActiveFlag_TC6(m_DMAx->_DMAx))
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(m_DMAx->_DMAx,I2C1_TX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable all the DMA interrupt */
          m_DMAx->DisableHalfTransferCompleteInterrupt(I2c::I2C1_TX_DMA_CHANNEL);
          m_DMAx->DisableTransferCompleteInterrupt(I2c::I2C1_TX_DMA_CHANNEL);
          m_DMAx->DisableTransferErrorInterrupt(I2c::I2C1_TX_DMA_CHANNEL);
        }                 
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_TC6(m_DMAx->_DMAx);
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT6(m_DMAx->_DMAx);                 
        
        // This varoable is only used by TxnDoneHandler() to keep track of Tx completion status
        m_Transaction.TxLen = 0;                				
        
        I2C_LOG_STATES(I2c::I2C_LOG_DMA_TX_DONE);
      }     
      /* Half Transfer Complete Interrupt management ******************************/
      else if (LL_DMA_IsActiveFlag_HT6(m_DMAx->_DMAx))
      {
        I2C_LOG_STATES(I2c::I2C_LOG_DMA_HALF_TX_DONE);
        
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(m_DMAx->_DMAx,I2C1_TX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable the half transfer interrupt */
          m_DMAx->DisableHalfTransferCompleteInterrupt(I2C1_TX_DMA_CHANNEL);
        }
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT6(m_DMAx->_DMAx);
      }      
      /* Transfer Error Interrupt management **************************************/
      else if ( LL_DMA_IsActiveFlag_TE6(m_DMAx->_DMAx))
      {
        /* When a DMA transfer error occurs */
        /* A hardware clear of its EN bits is performed */
        /* Disable ALL DMA IT */
        /* Disable all the DMA interrupt */
        m_DMAx->DisableHalfTransferCompleteInterrupt(I2C1_TX_DMA_CHANNEL);
        m_DMAx->DisableTransferCompleteInterrupt(I2C1_TX_DMA_CHANNEL);
        m_DMAx->DisableTransferErrorInterrupt(I2C1_TX_DMA_CHANNEL);
        
        /* Clear all flags */
        //m_DMAx->_DMAx->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);
        
        I2C_LOG_STATES(I2c::I2C_LOG_DMA_TX_ERROR);                
      }            
    }				
    
    void I2c::I2C1_DMA_Rx_Done_Handler()
    {			    
      /* Transfer Complete Interrupt management ***********************************/
      if (LL_DMA_IsActiveFlag_TC7(m_DMAx->_DMAx))
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(m_DMAx->_DMAx,I2C1_RX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable all the DMA interrupt */
          m_DMAx->DisableHalfTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
          m_DMAx->DisableTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
          m_DMAx->DisableTransferErrorInterrupt(I2C1_RX_DMA_CHANNEL);
        }                
        
        //I2C_DISABLE_INT_EVT_BUF_ERR(m_I2Cx);
        
        /* Disable DMA Request */            
        m_I2Cx->CR2 &= ~I2C_CR2_DMAEN;
        
        /* Clear the transfer complete flag */
        LL_DMA_ClearFlag_TC7(m_DMAx->_DMAx);
        
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT7(m_DMAx->_DMAx);  
        
#if I2C_MASTER_DMA       
        /* Generate Stop */
        I2C_GENERATE_STOP(m_I2Cx);
        
        // This variable is used by TxnDoneHandler() to keep track of completion status
        m_Transaction.RxLen = 0;
        
        I2C_LOG_STATES(I2c::I2C_LOG_DMA_RX_DONE);

        TxnDoneHandler(0); 
#elif I2C_SLAVE_DMA
        // In DMA slave mode nothing to de here.
        // When STOPF will be generated by master.
        // then Read the DMA count register to know
        // How much data is received in Rx buf then 
        // Call the Rx completion call back
#endif
      }           
      /* Half Transfer Complete Interrupt management ******************************/
      else if (LL_DMA_IsActiveFlag_HT7(m_DMAx->_DMAx))
      {
        I2C_LOG_STATES(I2c::I2C_LOG_DMA_HALF_RX_DONE);
        
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(m_DMAx->_DMAx,I2C1_RX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
        {
          /* Disable the half transfer interrupt */
          m_DMAx->DisableHalfTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
        }
        /* Clear the half transfer complete flag */
        LL_DMA_ClearFlag_HT7(m_DMAx->_DMAx);
      }            
      /* Transfer Error Interrupt management **************************************/
      else if ( LL_DMA_IsActiveFlag_TE7(m_DMAx->_DMAx))
      {
        /* When a DMA transfer error occurs */
        /* A hardware clear of its EN bits is performed */
        /* Disable ALL DMA IT */
        /* Disable all the DMA interrupt */
        m_DMAx->DisableHalfTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
        m_DMAx->DisableTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
        m_DMAx->DisableTransferErrorInterrupt(I2C1_RX_DMA_CHANNEL);                
        
        /* Clear all flags */
        //m_DMAx->_DMAx->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);
        
        I2C_LOG_STATES(I2c::I2C_LOG_DMA_RX_ERROR);                
      }            
    }
    
    void I2c::I2C1_DMA_Rx_Callback::CallbackFunction()
    {
      _This->I2C1_DMA_Rx_Done_Handler();
    }
    
    void I2c::I2C1_DMA_Tx_Callback::CallbackFunction()
    {
      _This->I2C1_DMA_Tx_Done_Handler();            
    }
    
    void I2c::I2C2_DMA_Rx_Callback::CallbackFunction()
    {
      
    }
    
    void I2c::I2C2_DMA_Tx_Callback::CallbackFunction()
    {
      
    }
    
#endif
    
    

  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
}

#endif //