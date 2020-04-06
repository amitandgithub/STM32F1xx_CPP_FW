/******************
** CLASS: I2cMasterIntr
**
** DESCRIPTION:
**  Implements the I2cMasterIntr class
**
** CREATED: 20 Mar 2020 by Amit Chaudhary
**
** FILE: I2cMasterIntr.h
**
******************/
#ifndef I2cMasterIntr_h
#define I2cMasterIntr_h

#include "I2c.h"


extern HAL::DMA dma1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::InterruptManager InterruptManagerInstance;

namespace HAL
{      
  template<I2c::I2cPort_t I2cPort>
    class I2cMasterIntr : public I2c, public InterruptSource
    {
    public:
      
      I2CStatus_t HwInit(I2cPins_t I2cPins);
      
      I2CStatus_t     Xfer(Transaction_t const *pTransaction);   
      
      virtual void ISR();     
      
    };
    
    
    
    
    template<I2c::I2cPort_t I2cPort>
      I2cMasterIntr<I2cPort>::I2CStatus_t I2cMasterIntr<I2cPort>::HwInit(I2cPins_t I2cPins)
      {        
        I2c::HwInit((I2Cx_t)I2cPort,I2cPins);
        
        if( (I2cPins == I2C1_B6_B7) || (I2cPins == I2C1_B8_B9) )
        {
          InterruptManagerInstance.RegisterDeviceInterrupt(I2C1_EV_IRQn,0,this);
          InterruptManagerInstance.RegisterDeviceInterrupt(I2C1_ER_IRQn,0,this);
        }
        else
        {
          InterruptManagerInstance.RegisterDeviceInterrupt(I2C2_EV_IRQn,0,this);
          InterruptManagerInstance.RegisterDeviceInterrupt(I2C2_ER_IRQn,0,this);
        }        
        
        return I2C_OK;
      }
    
    template<I2c::I2cPort_t I2cPort>
      I2cMasterIntr<I2cPort>::I2CStatus_t I2cMasterIntr<I2cPort>::Xfer(Transaction_t const *pTransaction)
      {
        I2CStatus_t status;
        
        status = CheckAndLoadTxn((I2Cx_t)I2cPort,pTransaction);
        
        if(status != I2C_OK) return status;
        
        if(m_Transaction.TxLen)
        {
          m_I2CState = I2C_MASTER_TX;
        }
        else
        {
          m_I2CState = I2C_MASTER_RX;
        }
        
        I2C_ENABLE_INT_EVT_BUF_ERR(I2cPort);
        
        /* Enable Acknowledge, Generate Start */
        I2C_ENABLE_ACK_AND_START(I2cPort);  
        
        return I2C_OK;
      }
    
    template<I2c::I2cPort_t I2cPort>
      void I2cMasterIntr<I2cPort>::ISR()
      {
        switch(I2C_GET_EVENT(I2cPort))
        {
        case EVENT_SB :
          I2C_LOG_EVENTS(I2C_LOG_SB);
          if(m_I2CState == I2C_MASTER_TX)
          {
            I2C_DATA_REG(I2cPort) = m_Transaction.SlaveAddress & I2C_DIR_WRITE; 
            I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
          }
          else if(m_I2CState == I2C_MASTER_RX)
          {
            /* start listening RxNE and TxE interrupts */  
            if((m_I2CState == I2C_MASTER_RX))
              I2C_ENABLE_INT_BUF(I2cPort);
            
#ifndef I2C_RX_METHOD_1                
            if(m_Transaction.RxLen == 2U) 
            {
              /* Enable Pos */
              I2C_ENABLE_POS(I2cPort); 
            }
#endif 
            I2C_DATA_REG(I2cPort) = m_Transaction.SlaveAddress | I2C_DIR_READ;
            
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
              I2C_ENABLE_POS(I2cPort);
            }
#endif          
            I2C_DATA_REG(I2cPort) = m_Transaction.SlaveAddress | I2C_DIR_READ;  
          }
          else
          {
            while(1);
          }	
          break;
        case EVENT_ADDR: 
          I2C_LOG_EVENTS(I2C_LOG_ADDR); 
          if(m_I2CState == I2C_MASTER_RX)
          {
            if(m_Transaction.RxLen == 1U)   
            {
              /* Clear ADDR flag */
              LL_I2C_ClearFlag_ADDR((I2Cx_t)I2cPort);
              
              /* Disable Acknowledge */
              I2C_DISABLE_ACK(I2cPort);
              
              /* Generate Stop */
              I2C_GENERATE_STOP(I2cPort);
              
              I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1);                                       
            }   
#ifndef I2C_RX_METHOD_1 
            else if(m_Transaction.RxLen == 2U)   
            {
              /* Clear ADDR flag */
              LL_I2C_ClearFlag_ADDR((I2Cx_t)I2cPort);
              
              /* Disable Acknowledge */
              I2C_DISABLE_ACK(I2cPort);
              
              I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2);                       
            }
#endif
            else
            {                    
              /* Clear ADDR flag */
              LL_I2C_ClearFlag_ADDR((I2Cx_t)I2cPort);
              
              I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_OTHER);
            }  
          }
          else if(m_I2CState == I2C_MASTER_TX)
          {                
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR((I2Cx_t)I2cPort);
            
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
          break;        
        case EVENT_BTF : 
          I2C_LOG_EVENTS(I2C_LOG_BTF);        
          if( m_I2CState == I2C_MASTER_RX_REPEATED_START) return; 
          
          if(m_I2CState == I2C_MASTER_RX)
          {      
            if(m_Transaction.RxLen == 3U)
            {
              /* Disable Acknowledge */
              I2C_DISABLE_ACK(I2cPort);
              
              // Read data3
              I2C_BUF_BYTE_IN(m_Transaction);
              
              /* Generate Stop */
              I2C_GENERATE_STOP(I2cPort); 
              
              // Read data2
              I2C_BUF_BYTE_IN(m_Transaction);                
              
              I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_3);
            }
            else if(m_Transaction.RxLen == 2U)
            {                 
              /* Generate Stop */
              I2C_GENERATE_STOP(I2cPort);         
              
              // Read data2
              I2C_BUF_BYTE_IN(m_Transaction);
              
              // Read data1
              I2C_BUF_BYTE_IN(m_Transaction); 
              
              I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED);
              
              if (WAIT_FOR_STOP_FLAG_TO_CLEAR(I2cPort,I2C_TIMEOUT))
              {
                m_I2CStatus = I2C_STOP_TIMEOUT;
              }
              TxnDoneHandler((I2Cx_t)I2cPort,0); 
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
              TxnDoneHandler((I2Cx_t)I2cPort,I2C_CR1_STOP);
            }        
          }
          else
          {
            while(1);/* Fatal Error*/ 
          }
          break;        
        case EVENT_ADD10 : 
          I2C_LOG_EVENTS(I2C_LOG_ADD10);
          break;        
        case EVENT_STOPF : 
          I2C_LOG_EVENTS(I2C_LOG_STOPF);
          I2C_CLEAR_STOPF(I2cPort);             
          m_I2CState = I2C_READY;
          I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
          break;        
        case EVENT_RSVD : 
          break; //while(1);      
        case EVENT_RXNE :
          I2C_LOG_EVENTS(I2C_LOG_RXNE);
          if(m_I2CState == I2C_MASTER_RX)
          {
#ifdef I2C_RX_METHOD_1
            if(m_Transaction.RxLen > 2)
            {
              /* Read data from DR */
              I2C_BUF_BYTE_IN(m_Transaction);        
              I2C_LOG_STATES(I2C_LOG_RXNE_MASTER);
            }
            else if(m_Transaction.RxLen == 2U)
            {
              /* Read data from DR */
              I2C_BUF_BYTE_IN(m_Transaction);
              
              /* Disable Acknowledge */
              I2C_DISABLE_ACK(I2cPort);
              
              /* Generate Stop */
              I2C_GENERATE_STOP(I2cPort);   
              
              I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2);
            }
            else if(m_Transaction.RxLen == 1U)
            {
              /* Read data from DR */
              I2C_BUF_BYTE_IN(m_Transaction);
              
              /* Disable EVT, BUF and ERR interrupt */
              I2C_DISABLE_INT_EVT_BUF_ERR(I2cPort);
              
              I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_1);
              
              TxnDoneHandler(0);                
            }
            else
            {
              while(1); // fatal error
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
              I2C_DISABLE_INT_EVT_BUF_ERR(I2cPort);
              
              I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_LAST);
              TxnDoneHandler((I2Cx_t)I2cPort,0);                
            }        
#endif
          }
          else
          {
            while(1);/* Fatal Error*/ 
          }
          break;        
        case EVENT_TXE : 
          I2C_LOG_EVENTS(I2C_LOG_TXE);        
          if(m_I2CState == I2C_MASTER_TX)
          {
            if( m_I2CState == I2C_MASTER_RX_REPEATED_START)  return;  
            if(m_Transaction.TxLen > 0)
            {
              I2C_BUF_BYTE_OUT(m_Transaction);
              
              I2C_LOG_STATES(I2C_LOG_TXE_GT_0);
            } 
            else
            {
              I2C_LOG_STATES(I2C_LOG_TXE_DONE);
              TxnDoneHandler((I2Cx_t)I2cPort,I2C_CR1_STOP);
            }
          }
          else
          {
            while(1);/* Fatal Error*/  
          }
          break;        
        case EVENT_BERR : 
          I2C_LOG_EVENTS(I2C_LOG_BERR);
          LL_I2C_ClearFlag_BERR((I2Cx_t)I2cPort);
          /* Workaround: Start cannot be generated after a misplaced Stop */
          //SoftReset();
          //m_I2CStatus = I2C_BUS_ERROR; 				
#if I2C_MASTER_Q       
          //Load next transaction from Txn queue if any
          LoadNextTransaction_MASTER_INTR();
#endif
          break;        
        case EVENT_ARLO : 
          I2C_LOG_EVENTS(I2C_LOG_ARLO);			
          LL_I2C_ClearFlag_ARLO((I2Cx_t)I2cPort);
          I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ARB_LOST);
#if I2C_MASTER_Q       
          //Load next transaction from Txn queue if any
          LoadNextTransaction_MASTER_INTR();
#endif
          break;        
        case EVENT_AF : // NACK        
          I2C_LOG_EVENTS(I2C_LOG_AF);
          LL_I2C_ClearFlag_AF((I2Cx_t)I2cPort); 
          if(m_I2CState == I2C_MASTER_TX)
          {
            /* Generate Stop */
            I2C_GENERATE_STOP(I2cPort);
            
            I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ACK_FAIL);
            
#if I2C_MASTER_Q       
            //Load next transaction from Txn queue if any
            LoadNextTransaction_MASTER_INTR();
#endif
            return;
          }
          else                                
          {
            while(1);/* Fatal Error*/   
          }     
          break;         
        case EVENT_OVR : 
          I2C_LOG_EVENTS(I2C_LOG_OVR);
          LL_I2C_ClearFlag_OVR((I2Cx_t)I2cPort);
          I2C_LOG_STATES(I2C_LOG_BTF_MASTER_DATA_OVR);
          while(1);// Not Implemented currently
          break;        
        case EVENT_PECERR :
          I2C_LOG_EVENTS(I2C_LOG_PECERR);
          while(1); // Not Implemented currently
          break;        
        default : 
          return;
        }
        
      }
}
#endif// I2cMasterIntr_h