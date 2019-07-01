/******************
** FILE: I2CInt.cpp
**
** DESCRIPTION:
**  I2CInt implementation in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
******************/

#include "I2CInt.h"


namespace HAL
{
    
    I2CInt::I2CInt()
        {
           
            
            m_RxQueueFullCallback = nullptr;
            m_TxQueueEmptyCallback = nullptr;
            m_SlaveRxDoneCallback = nullptr;
            m_SlaveTxDoneCallback = nullptr;
            
            //I2C_DEBUG_LOG(I2C_OK);
        }
        
       /*  I2CInt::I2CStatus_t I2CInt::HwInit(void *pInitStruct)
        {
          
          
            // Register Interrupt ISR with Interrupt Manager
            if(m_I2Cx == I2C1)
            {
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C1_EV_IRQn,0,this);
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C1_ER_IRQn,0,this);      
            }                
            else if(m_I2Cx == I2C2)
            {
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C2_EV_IRQn,0,this);
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C2_ER_IRQn,0,this);
            }   
            else
            {
                while(1); // Fatal Error
            }
            
            m_I2CState = READY;
            
            return I2C_OK;            
        } */
        
#if I2C_INT

        void I2CInt::SetCallback(I2CCallbackType_t I2CCallbackType, I2CCallback_t I2CCallback)
        {
            switch(I2CCallbackType)
            {
            case I2C_RX_QUEUE_FULL_CALLBACK:        m_RxQueueFullCallback = I2CCallback; break;
            case I2C_TX_QUEUE_EMPTY_CALLBACK:       m_TxQueueEmptyCallback = I2CCallback; break;
            case I2C_SLAVE_TX_COMPLETE_CALLBACK:    m_SlaveTxDoneCallback = I2CCallback; break;
            case I2C_SLAVE_RX_COMPLETE_CALLBACK:    m_SlaveRxDoneCallback = I2CCallback; break;
            default: break; 
            }            
        }
		
        I2CInt::I2CStatus_t I2CInt:: MasterTx_Intr(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        { 
            if(m_I2CState != READY)
                return I2C_BUSY;
            
            if( (TxLen == 0) || (TxBuf == nullptr) || (pStatus == nullptr) )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);                
                return I2C_INVALID_PARAMS;                
            }          
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true )
            {
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;
            }
            
            m_Transaction.SlaveAddress       = SlaveAddress;
            m_Transaction.TxBuf              = TxBuf;
            m_Transaction.TxLen              = TxLen;
            m_Transaction.RxBuf              = nullptr;
            m_Transaction.RxLen              = 0;  
            m_Transaction.RepeatedStart      = 0;  
            m_Transaction.pStatus            = pStatus; 
            m_Transaction.XferDoneCallback   = XferDoneCallback;
            
            m_I2CState = MASTER_TX;
            
            /* Disable Pos */
            m_I2Cx->CR1 &= ~I2C_CR1_POS;
            
			/* Enable Acknowledge */
			m_I2Cx->CR1 |= I2C_CR1_ACK;
            
            Enable_EVT_BUF_ERR_Interrupt();
            
            /* Generate Start */
            m_I2Cx->CR1 |= I2C_CR1_START;
            I2C_LOG_STATES(I2C_LOG_START_MASTER_TX);
            
            return I2C_OK;
        }
        
        I2CInt::I2CStatus_t I2CInt:: MasterTx_Intr(Transaction_t* pTransaction)
        { 
            if( pTransaction == nullptr )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);                
                return I2C_INVALID_PARAMS;                
            }
            
            return MasterTx_Intr(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen,pTransaction->pStatus, pTransaction->XferDoneCallback);
        }
        
        I2CInt::I2CStatus_t I2CInt:: MasterRx_Intr(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        {            
            if(m_I2CState != READY)
                return I2C_BUSY;
            
            if( (RxLen == 0) || (RxBuf == nullptr) || (pStatus == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);                
                return I2C_INVALID_PARAMS;                
            }
            
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {          
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            m_Transaction.SlaveAddress = SlaveAddress;
            m_Transaction.TxBuf              = nullptr;
            m_Transaction.TxLen              = 0;
            m_Transaction.RxBuf              = RxBuf;
            m_Transaction.RxLen              = RxLen;  
            m_Transaction.RepeatedStart      = 0;  
            m_Transaction.pStatus            = pStatus; 
            m_Transaction.XferDoneCallback   = XferDoneCallback;
            
            m_I2CState = MASTER_RX;   
            
            /* Disable Pos */
            m_I2Cx->CR1 &= ~I2C_CR1_POS;
            
			/* Enable Acknowledge */
			m_I2Cx->CR1 |= I2C_CR1_ACK;
            
            //InteruptControl(HAL::I2CInt::I2C_INTERRUPT_ENABLE_ALL);
            Enable_EVT_BUF_ERR_Interrupt();
            
            /* Generate Start */
            m_I2Cx->CR1 |= I2C_CR1_START;
            
            I2C_LOG_STATES(I2C_LOG_START_MASTER_RX);
            
            I2C_DEBUG_LOG(I2C_OK);
            return I2C_OK;
        }
        
        I2CInt::I2CStatus_t I2CInt:: MasterRx_Intr(Transaction_t* pTransaction)
        { 
            if( pTransaction == nullptr )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);                
                return I2C_INVALID_PARAMS;                
            }
            
            return MasterRx_Intr(pTransaction->SlaveAddress, pTransaction->RxBuf ,pTransaction->RxLen,pTransaction->pStatus, pTransaction->XferDoneCallback);
        }
        
        I2CInt::I2CStatus_t I2CInt::MasterTxRx_Intr(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen, uint8_t RepeatedStart,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        {
            if(m_I2CState != READY)
                return I2C_BUSY;
            
            if( ((TxBuf == nullptr) && (RxBuf == nullptr)) || (pStatus == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);                
                return I2C_INVALID_PARAMS;                
            }
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {          
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            m_Transaction.SlaveAddress       = SlaveAddress;
            m_Transaction.TxBuf              = TxBuf;
            m_Transaction.TxLen              = TxLen;
            m_Transaction.RxBuf              = RxBuf;
            m_Transaction.RxLen              = RxLen;  
            m_Transaction.RepeatedStart      = RepeatedStart;  
            m_Transaction.pStatus            = pStatus; 
            m_Transaction.XferDoneCallback   = XferDoneCallback;
            
            m_I2CState = MASTER_TX;
            
            /* Disable Pos */
            m_I2Cx->CR1 &= ~I2C_CR1_POS;
            
			/* Enable Acknowledge */
			m_I2Cx->CR1 |= I2C_CR1_ACK;
            
            Enable_EVT_BUF_ERR_Interrupt();
            
            /* Generate Start */
            m_I2Cx->CR1 |= I2C_CR1_START;
            
            I2C_LOG_STATES(I2C_LOG_START_MASTER_TXRX);
            
            I2C_DEBUG_LOG(I2C_OK);
            
            return I2C_OK;           
        }        
        
        I2CInt::I2CStatus_t I2CInt::MasterTxRx_Intr(Transaction_t* pTransaction)
        {
            if( pTransaction == nullptr )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);                
                return I2C_INVALID_PARAMS;                
            }
            
            return MasterTxRx_Intr(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen, pTransaction->RxBuf ,
                                   pTransaction->RxLen, pTransaction->RepeatedStart,pTransaction->pStatus, pTransaction->XferDoneCallback);            
        }        
#endif // I2C_INT

}