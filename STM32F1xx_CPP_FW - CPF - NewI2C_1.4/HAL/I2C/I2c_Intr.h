


#ifndef I2c_Intr_h
#define I2c_Intr_h


#include "I2c.h"
#include "I2c_hw.h"


namespace HAL
{



  void I2c::LoadNextTransaction_MASTER_INTR()
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
          m_I2CState = I2C_MASTER_TX; 
        }
        else 
        {
          // Next transaction starts with Rx only mode
          m_I2CState = I2C_MASTER_RX;
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
          
          I2C_LOG_STATES(I2C_LOG_TXN_DONE_ALL);                    
        }
      }
    }
    



#pragma inline = forced
    void I2c::SB_SLAVE_INTR()
    {
     while(1);
    }
#pragma inline = forced    
    void I2c::ADDR_SLAVE_INTR()
    {
      if((m_I2CState == I2C_READY) || (m_I2CState == I2C_SLAVE_RX) || (m_I2CState == I2C_SLAVE_TX)) 
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
        if( (m_I2Cx->SR2 & LL_I2C_SR2_TRA) == 0)
        {
          m_I2CState = I2C_SLAVE_RX;
        }
        else
        {
          m_I2CState = I2C_SLAVE_TX;
        }
      }
      else
      {
        while(1);/* Fatal Error*/      
      }
      
    }
#pragma inline = forced   
    void I2c::BTF_SLAVE_INTR()
    {
      if(m_I2CState == I2C_MASTER_RX_REPEATED_START ) 
      {
        return;
      }
      
      if(m_I2CState == I2C_SLAVE_RX ) 
      {
        if(m_SlaveTxn.RxBuf->Idx >= m_SlaveTxn.RxBuf->Len - 1)
        {     
          /* Dummy read/Write to clear the RXNE interrupt*/
          I2C_DATA_REG(m_I2Cx) = I2C_DATA_REG(m_I2Cx);
          I2C_LOG_STATES(I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE); 
        }
        else
        {
          I2C_SLAVE_BUF_BYTE_IN(m_SlaveTxn);
          I2C_LOG_STATES(I2C_LOG_SLAVE_RX_BYTE_IN); 
        }
      }
      else if(m_I2CState == I2C_SLAVE_TX )    
      {
        if(m_SlaveTxn.TxBuf->Idx < m_SlaveTxn.TxBuf->Len)
        {
          I2C_SLAVE_BUF_BYTE_OUT(m_SlaveTxn);
          
          I2C_LOG_EVENTS(I2C_LOG_TXE);      
          
          if(m_SlaveTxn.TxBuf->Idx >= m_SlaveTxn.TxBuf->Len)
          {            
            I2C_LOG_STATES(I2C_LOG_TXE_DONE);
          }
        }
        else
        {
          I2C_LOG_STATES(I2C_LOG_TXE_DEFAULT_BYTE);
          I2C_DATA_REG(m_I2Cx) = m_SlaveTxn.DefaultByte;  
        }      
      }
      else
      {
        while(1);/* Fatal Error*/ 
      }
    }
#pragma inline = forced   
    void I2c::TXE_SLAVE_INTR()
    {
     if(m_I2CState == I2C_SLAVE_TX )  
      {
        if(m_SlaveTxn.TxBuf->Idx < m_SlaveTxn.TxBuf->Len)
        {
          I2C_SLAVE_BUF_BYTE_OUT(m_SlaveTxn);
          
          I2C_LOG_EVENTS(I2C_LOG_TXE);      
          
          if(m_SlaveTxn.TxBuf->Idx >= m_SlaveTxn.TxBuf->Len)
          {              
            I2C_LOG_STATES(I2C_LOG_TXE_DONE);
          }
        }
        else
        {
          I2C_LOG_STATES(I2C_LOG_TXE_DEFAULT_BYTE);
          I2C_DATA_REG(m_I2Cx) = m_SlaveTxn.DefaultByte;  
        }      
      }
      else
      {
        while(1);/* Fatal Error*/  
      }
    }
#pragma inline = forced    
    void I2c::RXNE_SLAVE_INTR()
    {
      if(m_I2CState == I2C_SLAVE_RX )
      {
        if(m_SlaveTxn.RxBuf->Idx >= m_SlaveTxn.RxBuf->Len - 1)
        {     
          /* Dummy read/Write to clear the RXNE interrupt*/
          I2C_DATA_REG(m_I2Cx) = I2C_DATA_REG(m_I2Cx);
          I2C_LOG_STATES(I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE); 
        }
        else
        {
          I2C_SLAVE_BUF_BYTE_IN(m_SlaveTxn);
          I2C_LOG_STATES(I2C_LOG_SLAVE_RX_BYTE_IN); 
        }
      }
      else
      {
        while(1);/* Fatal Error*/ 
      }
    }
#pragma inline = forced    
    void I2c::AF_SLAVE_INTR()
    {
      LL_I2C_ClearFlag_AF(m_I2Cx); 
      
     if(m_I2CState == I2C_SLAVE_TX)
      {                
        // In Slave mode just execute the transaction done callback if registered                   
        I2C_LOG_STATES(I2C_LOG_AF_SLAVE_ACK_FAIL);
        
        m_I2CState = I2C_READY;
        
        if(m_SlaveTxn.XferDoneCallback)
          m_SlaveTxn.XferDoneCallback->CallbackFunction(I2C_SLAVE_TX_DONE);				  
      }
      else                                
      {
        while(1);/* Fatal Error*/   
      }      
    }    
#pragma inline = forced  
    void I2c::STOPF_SLAVE_INTR()
    {
      I2C_CLEAR_STOPF(m_I2Cx);  
      if(m_I2CState == I2C_SLAVE_RX)
      {
        /* Execute the RxDone Callback */
        if(m_SlaveTxn.XferDoneCallback)
          m_SlaveTxn.XferDoneCallback->CallbackFunction(I2C_SLAVE_RX_DONE);  
      }           
      m_I2CState = I2C_READY;
      I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
    }

  
  
  #pragma inline = forced    
   void I2c::SB_MASTER_SLAVE_INTR()
    {
      if(m_I2CState == I2C_MASTER_TX)
      {
        I2C_DATA_REG(m_I2Cx) = m_Transaction.SlaveAddress & I2C_DIR_WRITE; 
        I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
      }
      else if(m_I2CState == I2C_MASTER_RX)
      {
        /* start listening RxNE and TxE interrupts */  
        if((m_I2CState == I2C_MASTER_RX))
          I2C_ENABLE_INT_BUF(m_I2Cx);
        
#ifndef I2C_RX_METHOD_1                
        if(m_Transaction.RxLen == 2U) 
        {
          /* Enable Pos */
          I2C_DISABLE_POS(m_I2Cx); 
        }
#endif 
        I2C_DATA_REG(m_I2Cx) = m_Transaction.SlaveAddress | I2C_DIR_READ;
        
        I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX);
      }
      else if(m_I2CState == I2C_MASTER_RX_REPEATED_START)
      {
        /* Repeated start is handled here, clear the flag*/
        m_Transaction.RepeatedStart = 0;         
        
#ifndef I2C_RX_METHOD_1                
        if(m_Transaction.RxLen == 2U) 
        {
          /* Enable Pos */
          I2C_DISABLE_POS(m_I2Cx);
        }
#endif          
        I2C_DATA_REG(m_I2Cx) = m_Transaction.SlaveAddress | I2C_DIR_READ;  
        
          /* start listening RxNE and TxE interrupts */                
          I2C_ENABLE_INT_BUF(m_I2Cx);
          
          m_I2CState = I2C_MASTER_RX;
          
          I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX_REPEATED_START);
      }
      else
      {
        while(1);
      }	
    }
#pragma inline = forced    
    void I2c::ADDR_MASTER_SLAVE_INTR()
    {
      if(m_I2CState == I2C_MASTER_RX)
      {
        if(m_Transaction.RxLen == 1U)   
        {
          /* Clear ADDR flag */
          LL_I2C_ClearFlag_ADDR(m_I2Cx);
          
          /* Disable Acknowledge */
          I2C_DISABLE_ACK(m_I2Cx);
          
          /* Generate Stop */
          I2C_GENERATE_STOP(m_I2Cx);
          
          I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1);                                       
        }   
#ifndef I2C_RX_METHOD_1 
        else if(m_Transaction.RxLen == 2U)   
        {
          /* Clear ADDR flag */
          LL_I2C_ClearFlag_ADDR(m_I2Cx);
          
          /* Disable Acknowledge */
          I2C_DISABLE_ACK(m_I2Cx);
          
          I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2);                       
        }
#endif
        else
        {                    
          /* Clear ADDR flag */
          LL_I2C_ClearFlag_ADDR(m_I2Cx);
          
          I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_OTHER);
        }  
      }
      else if(m_I2CState == I2C_MASTER_TX)
      {                
        /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(m_I2Cx);
        
        if(m_Transaction.TxLen > 0)
        {                    
          I2C_BUF_BYTE_OUT(m_Transaction);
          I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0);
        }
      }
      else if((m_I2CState == I2C_READY) || (m_I2CState == I2C_SLAVE_RX) || (m_I2CState == I2C_SLAVE_TX)) 
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
        if( (m_I2Cx->SR2 & LL_I2C_SR2_TRA) == 0)
        {
          m_I2CState = I2C_SLAVE_RX;
        }
        else
        {
            m_I2CState = I2C_SLAVE_TX;
        }
      }
      else
      {
        while(1);/* Fatal Error*/      
      }
      
    }
#pragma inline = forced    
    void I2c::BTF_MASTER_SLAVE_INTR()
    {
      if(m_I2CState == I2C_MASTER_RX_REPEATED_START ) 
      {
        return;
      }
      
      if(m_I2CState == I2C_MASTER_RX)
      {
        if( m_I2CState == I2C_MASTER_RX_REPEATED_START)
          return;            
        
        if(m_Transaction.RxLen == 3U)
        {
          /* Disable Acknowledge */
          I2C_DISABLE_ACK(m_I2Cx);
          
          // Read data3
          I2C_BUF_BYTE_IN(m_Transaction);
          
          /* Generate Stop */
          I2C_GENERATE_STOP(m_I2Cx); 
          
          // Read data2
          I2C_BUF_BYTE_IN(m_Transaction);                
          
          I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_3);
        }
        else if(m_Transaction.RxLen == 2U)
        {                 
          /* Generate Stop */
          I2C_GENERATE_STOP(m_I2Cx);         
          
          // Read data2
          I2C_BUF_BYTE_IN(m_Transaction);
          
          // Read data1
          I2C_BUF_BYTE_IN(m_Transaction); 
          
          I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED);
          
          if (WAIT_FOR_STOP_FLAG_TO_CLEAR(m_I2Cx,I2C_TIMEOUT))
          {
            m_I2CStatus = I2C_STOP_TIMEOUT;
            //(*m_Transaction.pStatus) = I2C_STOP_TIMEOUT;
          }
          TxnDoneHandler(0); 
        }
        else 
        {
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);
          I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_GT_3);
        } 
      }
      else if(m_I2CState == I2C_MASTER_TX)
      {
        if(m_Transaction.TxLen > 0)
        {
          I2C_BUF_BYTE_OUT(m_Transaction);
          I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_GT_0);
        }
        else
        {
          TxnDoneHandler(I2C_CR1_STOP);
        }
        
      }
      else if(m_I2CState == I2C_SLAVE_RX ) 
      {
        if(m_SlaveTxn.RxBuf->Idx >= m_SlaveTxn.RxBuf->Len - 1)
        {     
          /* Dummy read/Write to clear the RXNE interrupt*/
          I2C_DATA_REG(m_I2Cx) = I2C_DATA_REG(m_I2Cx);
          I2C_LOG_STATES(I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE); 
        }
        else
        {
          I2C_SLAVE_BUF_BYTE_IN(m_SlaveTxn);
          I2C_LOG_STATES(I2C_LOG_SLAVE_RX_BYTE_IN); 
        }
      }
      else if(m_I2CState == I2C_SLAVE_TX )    
      {
        if(m_SlaveTxn.TxBuf->Idx < m_SlaveTxn.TxBuf->Len)
        {
          I2C_SLAVE_BUF_BYTE_OUT(m_SlaveTxn);
          
          I2C_LOG_EVENTS(I2C_LOG_TXE);      
          
          if(m_SlaveTxn.TxBuf->Idx >= m_SlaveTxn.TxBuf->Len)
          {            
            I2C_LOG_STATES(I2C_LOG_TXE_DONE);
          }
        }
        else
        {
          I2C_LOG_STATES(I2C_LOG_TXE_DEFAULT_BYTE);
          I2C_DATA_REG(m_I2Cx) = m_SlaveTxn.DefaultByte;  
        }      
      }
      else
      {
        while(1);/* Fatal Error*/ 
      }
    }
#pragma inline = forced    
    void I2c::TXE_MASTER_SLAVE_INTR()
    {
      if(m_I2CState == I2C_MASTER_TX)   
      {
        if( m_I2CState == I2C_MASTER_RX_REPEATED_START)
          return;
        
        if(m_Transaction.TxLen > 0)
        {
          I2C_BUF_BYTE_OUT(m_Transaction);
          
          I2C_LOG_STATES(I2C_LOG_TXE_GT_0);
        } 
        else
        {
          I2C_LOG_STATES(I2C_LOG_TXE_DONE);
          TxnDoneHandler(I2C_CR1_STOP);
        }
      }
      else if(m_I2CState == I2C_SLAVE_TX )  
      {
        if(m_SlaveTxn.TxBuf->Idx < m_SlaveTxn.TxBuf->Len)
        {
          I2C_SLAVE_BUF_BYTE_OUT(m_SlaveTxn);
          
          I2C_LOG_EVENTS(I2C_LOG_TXE);      
          
          if(m_SlaveTxn.TxBuf->Idx >= m_SlaveTxn.TxBuf->Len)
          {              
            I2C_LOG_STATES(I2C_LOG_TXE_DONE);
          }
        }
        else
        {
          I2C_LOG_STATES(I2C_LOG_TXE_DEFAULT_BYTE);
          I2C_DATA_REG(m_I2Cx) = m_SlaveTxn.DefaultByte;  
        }      
      }
      else
      {
        while(1);/* Fatal Error*/  
      }
    }
#pragma inline = forced    
    void I2c::RXNE_MASTER_SLAVE_INTR()
    {
      if(m_I2CState == I2C_MASTER_RX)
      {
#ifdef I2C_RX_METHOD_1
        if(m_Transaction.RxLen == 2U)
        {
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);
          
          /* Disable Acknowledge */
          I2C_DISABLE_ACK(m_I2Cx);
          
          /* Generate Stop */
          I2C_GENERATE_STOP(m_I2Cx);   
          
          I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2);
        }
        else if(m_Transaction.RxLen == 1U)
        {
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);
          
          /* Disable EVT, BUF and ERR interrupt */
          I2C_DISABLE_INT_EVT_BUF_ERR(m_I2Cx);
          
          I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_1);
          
          TxnDoneHandler(0);                
        }
        else
        {
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);
          
          I2C_LOG_STATES(I2C_LOG_RXNE_MASTER);
        }
#else
        if(m_Transaction.RxLen > 3U)
        {
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);                
        }
        else if( (m_Transaction.RxLen == 2U) || (m_Transaction.RxLen == 3U) )
        {
          // Do nothing here, data 3 is here in data register.
          // Let the data 2 also accumulate in shift register in next BTF.
          // After that we read data 3 and data2 in the Master Rx BTF handler
          I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2_OR_3);
        }
        else
        {      
          // This is for the case when Last byte/data1 is to be read
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);
          
          /* Disable EVT, BUF and ERR interrupt */
          I2C_DISABLE_INT_EVT_BUF_ERR(m_I2Cx);
          
          I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_LAST);
          TxnDoneHandler(0);                
        }        
#endif
      }
      else if(m_I2CState == I2C_SLAVE_RX )
      {
        if(m_SlaveTxn.RxBuf->Idx >= m_SlaveTxn.RxBuf->Len - 1)
        {     
          /* Dummy read/Write to clear the RXNE interrupt*/
          I2C_DATA_REG(m_I2Cx) = I2C_DATA_REG(m_I2Cx);
          I2C_LOG_STATES(I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE); 
        }
        else
        {
          I2C_SLAVE_BUF_BYTE_IN(m_SlaveTxn);
          I2C_LOG_STATES(I2C_LOG_SLAVE_RX_BYTE_IN); 
        }
      }
      else
      {
        while(1);/* Fatal Error*/ 
      }
    }
#pragma inline = forced    
    void I2c::AF_MASTER_SLAVE_INTR()
    {
      LL_I2C_ClearFlag_AF(m_I2Cx); 
      if(m_I2CState == I2C_MASTER_TX)
      {
        /* Generate Stop */
        I2C_GENERATE_STOP(m_I2Cx);
        
        I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ACK_FAIL);
        
#if I2C_MASTER_Q       
        //Load next transaction from Txn queue if any
        LOAD_NEXT_TXN();
#endif
        return;
      }
      else if(m_I2CState == I2C_SLAVE_TX)
      {                
        // In Slave mode just execute the transaction done callback if registered                   
        I2C_LOG_STATES(I2C_LOG_AF_SLAVE_ACK_FAIL);
        
        m_I2CState = I2C_READY;
        
        if(m_SlaveTxn.XferDoneCallback)
          m_SlaveTxn.XferDoneCallback->CallbackFunction(I2C_SLAVE_TX_DONE);				  
      }
      else                                
      {
        while(1);/* Fatal Error*/   
      }
      
    }    
#pragma inline = forced 
    void I2c::STOPF_MASTER_SLAVE_INTR()
    {
      I2C_CLEAR_STOPF(m_I2Cx);  
      
        /* Execute the RxDone Callback */
        if(m_SlaveTxn.XferDoneCallback)
          m_SlaveTxn.XferDoneCallback->CallbackFunction(I2C_SLAVE_RX_DONE);          
      m_I2CState = I2C_READY;
      I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
    }
    
  
  #pragma inline = forced
    void I2c::SB_MASTER_INTR()
    {
      if(m_I2CState == I2C_MASTER_TX)
      {
        I2C_DATA_REG(m_I2Cx) = m_Transaction.SlaveAddress & I2C_DIR_WRITE; 
        I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
      }
      else if((m_I2CState == I2C_MASTER_RX) || (m_I2CState == I2C_MASTER_RX_DMA))  
      {
        /* start listening RxNE and TxE interrupts */  
        if((m_I2CState == I2C_MASTER_RX))
          I2C_ENABLE_INT_BUF(m_I2Cx);
        
#ifndef I2C_RX_METHOD_1                
        if(m_Transaction.RxLen == 2U) 
        {
          /* Enable Pos */
          I2C_DISABLE_POS(m_I2Cx); 
        }
#endif 
        I2C_DATA_REG(m_I2Cx) = m_Transaction.SlaveAddress | I2C_DIR_READ;
        
        I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX);
      }
      else if(m_I2CState == I2C_MASTER_RX_REPEATED_START)
      {
        /* Repeated start is handled here, clear the flag*/
        m_Transaction.RepeatedStart = 0;         
        
#ifndef I2C_RX_METHOD_1                
        if(m_Transaction.RxLen == 2U) 
        {
          /* Enable Pos */
          I2C_DISABLE_POS(m_I2Cx);
        }
#endif          
        I2C_DATA_REG(m_I2Cx) = m_Transaction.SlaveAddress | I2C_DIR_READ;  
      }
      else
      {
        while(1);
      }	
    }
#pragma inline = forced   
    void I2c::ADDR_MASTER_INTR()
    {
      if(m_I2CState == I2C_MASTER_RX)
      {
        if(m_Transaction.RxLen == 1U)   
        {
          /* Clear ADDR flag */
          LL_I2C_ClearFlag_ADDR(m_I2Cx);
          
          /* Disable Acknowledge */
          I2C_DISABLE_ACK(m_I2Cx);
          
          /* Generate Stop */
          I2C_GENERATE_STOP(m_I2Cx);
          
          I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1);                                       
        }   
#ifndef I2C_RX_METHOD_1 
        else if(m_Transaction.RxLen == 2U)   
        {
          /* Clear ADDR flag */
          LL_I2C_ClearFlag_ADDR(m_I2Cx);
          
          /* Disable Acknowledge */
          I2C_DISABLE_ACK(m_I2Cx);
          
          I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2);                       
        }
#endif
        else
        {                    
          /* Clear ADDR flag */
          LL_I2C_ClearFlag_ADDR(m_I2Cx);
          
          I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_OTHER);
        }  
      }
      else if(m_I2CState == I2C_MASTER_TX)
      {                
        /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(m_I2Cx);
        
        if(m_Transaction.TxLen > 0)
        {                    
          I2C_BUF_BYTE_OUT(m_Transaction);
          I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0);
        }
      }
      else
      {
        while(1);/* Fatal Error*/      
      }      
    }
#pragma inline = forced    
    void I2c::BTF_MASTER_INTR()
    {
      if(m_I2CState == I2C_MASTER_RX_REPEATED_START ) 
      {
        return;
      }
      
      if(m_I2CState == I2C_MASTER_RX)
      {
        if( m_I2CState == I2C_MASTER_RX_REPEATED_START)
          return;            
        
        if(m_Transaction.RxLen == 3U)
        {
          /* Disable Acknowledge */
          I2C_DISABLE_ACK(m_I2Cx);
          
          // Read data3
          I2C_BUF_BYTE_IN(m_Transaction);
          
          /* Generate Stop */
          I2C_GENERATE_STOP(m_I2Cx); 
          
          // Read data2
          I2C_BUF_BYTE_IN(m_Transaction);                
          
          I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_3);
        }
        else if(m_Transaction.RxLen == 2U)
        {                 
          /* Generate Stop */
          I2C_GENERATE_STOP(m_I2Cx);         
          
          // Read data2
          I2C_BUF_BYTE_IN(m_Transaction);
          
          // Read data1
          I2C_BUF_BYTE_IN(m_Transaction); 
          
          I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED);
          
          if (WAIT_FOR_STOP_FLAG_TO_CLEAR(m_I2Cx,I2C_TIMEOUT))
          {
            m_I2CStatus = I2C_STOP_TIMEOUT;
            //(*m_Transaction.pStatus) = I2C_STOP_TIMEOUT;
          }
          TxnDoneHandler(0); 
        }
        else 
        {
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);
          I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_GT_3);
        } 
      }
      else if(m_I2CState == I2C_MASTER_TX)
      {
        if(m_Transaction.TxLen > 0)
        {
          I2C_BUF_BYTE_OUT(m_Transaction);
          I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_GT_0);
        }
        else
        {
          TxnDoneHandler(I2C_CR1_STOP);
        }        
      }
      else
      {
        while(1);/* Fatal Error*/ 
      }
    }
#pragma inline = forced    
    void I2c::TXE_MASTER_INTR()
    {
      if( m_I2CState == I2C_MASTER_RX_REPEATED_START)
        return;
      
      if(m_I2CState == I2C_MASTER_TX)
      {
        if(m_Transaction.TxLen > 0)
        {
          I2C_BUF_BYTE_OUT(m_Transaction);
          
          I2C_LOG_STATES(I2C_LOG_TXE_GT_0);
        } 
        else
        {
          I2C_LOG_STATES(I2C_LOG_TXE_DONE);
          TxnDoneHandler(I2C_CR1_STOP);
        }
      }
      else
      {
        while(1);/* Fatal Error*/  
      }
    }
#pragma inline = forced    
    void I2c::RXNE_MASTER_INTR()
    {
      if(m_I2CState == I2C_MASTER_RX)
      {
#ifdef I2C_RX_METHOD_1
        if(m_Transaction.RxLen == 2U)
        {
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);
          
          /* Disable Acknowledge */
          I2C_DISABLE_ACK(m_I2Cx);
          
          /* Generate Stop */
          I2C_GENERATE_STOP(m_I2Cx);   
          
          I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2);
        }
        else if(m_Transaction.RxLen == 1U)
        {
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);
          
          /* Disable EVT, BUF and ERR interrupt */
          I2C_DISABLE_INT_EVT_BUF_ERR(m_I2Cx);
          
          I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_1);
          
          TxnDoneHandler(0);                
        }
        else
        {
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);
          
          I2C_LOG_STATES(I2C_LOG_RXNE_MASTER);
        }
#else
        if(m_Transaction.RxLen > 3U)
        {
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);                
        }
        else if( (m_Transaction.RxLen == 2U) || (m_Transaction.RxLen == 3U) )
        {
          // Do nothing here, data 3 is here in data register.
          // Let the data 2 also accumulate in shift register in next BTF.
          // After that we read data 3 and data2 in the Master Rx BTF handler
          I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2_OR_3);
        }
        else
        {      
          // This is for the case when Last byte/data1 is to be read
          /* Read data from DR */
          I2C_BUF_BYTE_IN(m_Transaction);
          
          /* Disable EVT, BUF and ERR interrupt */
          I2C_DISABLE_INT_EVT_BUF_ERR(m_I2Cx);
          
          I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_LAST);
          TxnDoneHandler(0);                
        }        
#endif
      }
      else
      {
        while(1);/* Fatal Error*/ 
      }
    }
#pragma inline = forced    
    void I2c::AF_MASTER_INTR()
    {
      LL_I2C_ClearFlag_AF(m_I2Cx); 
      if(m_I2CState == I2C_MASTER_TX)
      {
        /* Generate Stop */
        I2C_GENERATE_STOP(m_I2Cx);
        
        I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ACK_FAIL);
        
#if I2C_MASTER_Q       
        //Load next transaction from Txn queue if any
        LOAD_NEXT_TXN();
#endif
        return;
      }
      else                                
      {
        while(1);/* Fatal Error*/   
      }      
    }    
#pragma inline = forced 
    void I2c::STOPF_MASTER_INTR()
    {
      I2C_CLEAR_STOPF(m_I2Cx);             
      m_I2CState = I2C_READY;
      I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
    }













}

#endif //I2c_Intr_h