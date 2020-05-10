/******************
** FILE: I2CDMA.cpp
**
** DESCRIPTION:
**  I2CDMA implementation in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
******************/

#include "I2CDMA.h"


namespace HAL
{
    
    I2CDMA::I2CDMA(Pin_t scl, Pin_t sda, Hz_t Hz) :  I2CInt(scl, sda, Hz),
        m_I2C1_DMA_Rx_Callback(this),
        m_I2C1_DMA_Tx_Callback(this),
        m_I2C2_DMA_Rx_Callback(this),
        m_I2C2_DMA_Tx_Callback(this)
        {

        }
        
        I2CDMA::I2CStatus_t I2CDMA::HwInit(void *pInitStruct)
        {
        	DMA::DMAConfig_t DMAConfig; 
          
			I2CInt::HwInit();
            
            /* Set DMA_InitStruct fields to default values */
			DMAConfig.PeriphOrM2MSrcAddress  = 0;
			DMAConfig.MemoryOrM2MDstAddress  = 0;
			DMAConfig.Direction 			 = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
			DMAConfig.Mode					 = LL_DMA_MODE_NORMAL;
			DMAConfig.PeriphOrM2MSrcIncMode  = LL_DMA_MEMORY_NOINCREMENT;
			DMAConfig.MemoryOrM2MDstIncMode  = LL_DMA_MEMORY_INCREMENT;
			DMAConfig.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
			DMAConfig.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
			DMAConfig.NbData				 = 0;
			DMAConfig.Priority				 = LL_DMA_PRIORITY_LOW;
            
            // Register Interrupt ISR with Interrupt Manager
            if(m_I2Cx == I2C1)
            {
                _DMA->ClockEnable();
                _DMA->RegisterCallback(I2C1_RX_DMA_CHANNEL,&m_I2C1_DMA_Rx_Callback);
                _DMA->RegisterCallback(I2C1_TX_DMA_CHANNEL,&m_I2C1_DMA_Tx_Callback);
                InterruptManager::GetInstance()->EnableInterrupt(DMA1_Channel6_IRQn);
                InterruptManager::GetInstance()->EnableInterrupt(DMA1_Channel7_IRQn);
                
                _DMA->XferConfig(&DMAConfig, I2C1_TX_DMA_CHANNEL);
                _DMA->XferConfig(&DMAConfig, I2C1_RX_DMA_CHANNEL);  
            
				_DMA->EnableTransferCompleteInterrupt(I2C1_TX_DMA_CHANNEL);
            	_DMA->EnableTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
                
                
            }                
            else if(m_I2Cx == I2C2)
            {
                _DMA->RegisterCallback(I2C2_RX_DMA_CHANNEL,&m_I2C2_DMA_Rx_Callback);
                _DMA->RegisterCallback(I2C2_TX_DMA_CHANNEL,&m_I2C2_DMA_Tx_Callback);
				_DMA->EnableTransferCompleteInterrupt(I2C2_TX_DMA_CHANNEL);
            	_DMA->EnableTransferCompleteInterrupt(I2C2_RX_DMA_CHANNEL);
            }   
            else
            {
                while(1); // Fatal Error
            }
            
            m_I2CState = READY;
            
            return I2C_OK;            
        }
        
        I2CDMA::I2CStatus_t I2CDMA:: MasterTx_DMA(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
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
            
            if(TxLen <= 2)
            {
                /* Enable Last DMA bit */
                m_I2Cx->CR2 |= I2C_CR2_LAST;
            }
            
             /* Load DMA Tx transaction*/
            _DMA->Load(I2C1_TX_DMA_CHANNEL, (uint32_t)&(m_I2Cx->DR), (uint32_t)TxBuf, TxLen, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
            
            m_I2CState = MASTER_TX_DMA;
            
            /* Disable Pos */
            m_I2Cx->CR1 &= ~I2C_CR1_POS;
            
			/* Enable Acknowledge */
			m_I2Cx->CR1 |= I2C_CR1_ACK;            
            
            Enable_EVT_ERR_Interrupt();
            
            /* Generate Start */
            m_I2Cx->CR1 |= I2C_CR1_START;          
            
            I2C_LOG_STATES(I2C_LOG_START_MASTER_TX_DMA);
            
            /* Enable DMA Request */
            m_I2Cx->CR2 |= I2C_CR2_DMAEN;
            
            return I2C_OK;
        }
        
        I2CDMA::I2CStatus_t I2CDMA:: MasterTxRx_DMA(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen, 
                                                      uint8_t RepeatedStart,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
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
            
            if(TxLen <= 2)
            {
                /* Enable Last DMA bit */
                m_I2Cx->CR2 |= I2C_CR2_LAST;
            }
            
            /* Load DMA Tx transaction*/ 
            _DMA->Load(I2C1_TX_DMA_CHANNEL, (uint32_t)&(m_I2Cx->DR), (uint32_t)TxBuf, TxLen, LL_DMA_DIRECTION_MEMORY_TO_PERIPH ); 
            
            /* Load DMA Rx transaction*/
            _DMA->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)&(m_I2Cx->DR), (uint32_t)RxBuf, RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY );
            
            m_I2CState = MASTER_TX_DMA;
            
            /* Enable Last DMA bit */
            m_I2Cx->CR2 |= I2C_CR2_LAST;
                
            /* Disable Pos */
            m_I2Cx->CR1 &= ~I2C_CR1_POS;
            
			/* Enable Acknowledge */
			m_I2Cx->CR1 |= I2C_CR1_ACK;            
            
            Enable_EVT_ERR_Interrupt();
            
            /* Generate Start */
            m_I2Cx->CR1 |= I2C_CR1_START;          
            
            I2C_LOG_STATES(I2C_LOG_START_MASTER_TX_DMA);
            
            /* Enable DMA Request */
            m_I2Cx->CR2 |= I2C_CR2_DMAEN;
            
            return I2C_OK;
        }
        
        I2CDMA::I2CStatus_t I2CDMA:: MasterRx_DMA(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
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
            
            /* Load DMA Rx transaction*/
            _DMA->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)&(m_I2Cx->DR), (uint32_t)RxBuf, RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY );
             
            if(RxLen == 1)
            {
                /* Set the last data xfer bit*/
                m_I2Cx->CR2 |= I2C_CR2_LAST; 
            }
            
            m_I2CState = MASTER_RX_DMA;
            
            /* Disable Pos */
            m_I2Cx->CR1 &= ~I2C_CR1_POS;
            
			/* Enable Acknowledge */
			m_I2Cx->CR1 |= I2C_CR1_ACK;          
            
            /* Generate Start */
            m_I2Cx->CR1 |= I2C_CR1_START;
            
            /* Enable DMA Request */
            m_I2Cx->CR2 |= I2C_CR2_DMAEN;
            
            /* Disable Last DMA */
            m_I2Cx->CR2 |= I2C_CR2_LAST;
            
            Enable_EVT_ERR_Interrupt();
            
            I2C_LOG_STATES(I2C_LOG_START_MASTER_RX_DMA);
            
            return I2C_OK;
        }       
        
		void I2CDMA::I2C1_DMA_Tx_Done_Handler()
		{			      
            /* Transfer Complete Interrupt management ***********************************/
            if (LL_DMA_IsActiveFlag_TC6(_DMA->_DMAx))
            {
                /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
                if(LL_DMA_GetMode(_DMA->_DMAx,I2C1_TX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
                {
                    /* Disable all the DMA interrupt */
                    _DMA->DisableHalfTransferCompleteInterrupt(I2CDMA::I2C1_TX_DMA_CHANNEL);
                    _DMA->DisableTransferCompleteInterrupt(I2CDMA::I2C1_TX_DMA_CHANNEL);
                    _DMA->DisableTransferErrorInterrupt(I2CDMA::I2C1_TX_DMA_CHANNEL);
                }                 
                
                /* Clear the half transfer complete flag */
                LL_DMA_ClearFlag_TC6(_DMA->_DMAx);
                
                /* Clear the half transfer complete flag */
                LL_DMA_ClearFlag_HT6(_DMA->_DMAx);                 
                
                // This varoable is used by TxnDoneHandler() to keep track of completion status
                m_Transaction.TxLen = 0;                				
					
                I2C_LOG_STATES(I2CDMA::I2C_LOG_DMA_TX_DONE);
            }     
            /* Half Transfer Complete Interrupt management ******************************/
            else if (LL_DMA_IsActiveFlag_HT6(_DMA->_DMAx))
            {
                I2C_LOG_STATES(I2CDMA::I2C_LOG_DMA_HALF_TX_DONE);
                
                /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
                if(LL_DMA_GetMode(_DMA->_DMAx,I2C1_TX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
                {
                    /* Disable the half transfer interrupt */
                    _DMA->DisableHalfTransferCompleteInterrupt(I2C1_TX_DMA_CHANNEL);
                }
                /* Clear the half transfer complete flag */
                LL_DMA_ClearFlag_HT6(_DMA->_DMAx);
            }      
            /* Transfer Error Interrupt management **************************************/
            else if ( LL_DMA_IsActiveFlag_TE6(_DMA->_DMAx))
            {
                /* When a DMA transfer error occurs */
                /* A hardware clear of its EN bits is performed */
                /* Disable ALL DMA IT */
                /* Disable all the DMA interrupt */
                _DMA->DisableHalfTransferCompleteInterrupt(I2C1_TX_DMA_CHANNEL);
                _DMA->DisableTransferCompleteInterrupt(I2C1_TX_DMA_CHANNEL);
                _DMA->DisableTransferErrorInterrupt(I2C1_TX_DMA_CHANNEL);
                
                /* Clear all flags */
                //_DMA->_DMAx->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);
                
                I2C_LOG_STATES(I2CDMA::I2C_LOG_DMA_TX_ERROR);                
            }            
        }				
        
		void I2CDMA::I2C1_DMA_Rx_Done_Handler()
		{			    
            /* Transfer Complete Interrupt management ***********************************/
            if (LL_DMA_IsActiveFlag_TC7(_DMA->_DMAx))
            {
                /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
                if(LL_DMA_GetMode(_DMA->_DMAx,I2C1_RX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
                {
                    /* Disable all the DMA interrupt */
                    _DMA->DisableHalfTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
                    _DMA->DisableTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
                    _DMA->DisableTransferErrorInterrupt(I2C1_RX_DMA_CHANNEL);
                }                
                
                /* Disable DMA Request */            
                m_I2Cx->CR2 &= ~I2C_CR2_DMAEN;
                
                /* Clear the transfer complete flag */
                LL_DMA_ClearFlag_TC7(_DMA->_DMAx);
                
                /* Clear the half transfer complete flag */
                LL_DMA_ClearFlag_HT7(_DMA->_DMAx);        
                 
                /* Generate Stop */
                m_I2Cx->CR1 |= I2C_CR1_STOP;
                
				 // This variable is used by TxnDoneHandler() to keep track of completion status
				m_Transaction.RxLen = 0;
                
                I2C_LOG_STATES(I2CDMA::I2C_LOG_DMA_RX_DONE);
				 
                TxnDoneHandler(0); 
            }           
            /* Half Transfer Complete Interrupt management ******************************/
            else if (LL_DMA_IsActiveFlag_HT7(_DMA->_DMAx))
            {
                I2C_LOG_STATES(I2CDMA::I2C_LOG_DMA_HALF_RX_DONE);
                
                /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
                if(LL_DMA_GetMode(_DMA->_DMAx,I2C1_RX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
                {
                    /* Disable the half transfer interrupt */
                    _DMA->DisableHalfTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
                }
                /* Clear the half transfer complete flag */
                LL_DMA_ClearFlag_HT7(_DMA->_DMAx);
            }            
            /* Transfer Complete Interrupt management **************************************/
            else if ( LL_DMA_IsActiveFlag_TE7(_DMA->_DMAx))
            {
                /* When a DMA transfer error occurs */
                /* A hardware clear of its EN bits is performed */
                /* Disable ALL DMA IT */
                /* Disable all the DMA interrupt */
                _DMA->DisableHalfTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
                _DMA->DisableTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
                _DMA->DisableTransferErrorInterrupt(I2C1_RX_DMA_CHANNEL);                
                
                /* Clear all flags */
                //_DMA->_DMAx->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);
                
                I2C_LOG_STATES(I2CDMA::I2C_LOG_DMA_RX_ERROR);                
            }            
        }
        
        void I2CDMA::I2C1_DMA_Rx_Callback::CallbackFunction()
        {
            m_This->I2C1_DMA_Rx_Done_Handler();
        }
        
        void I2CDMA::I2C1_DMA_Tx_Callback::CallbackFunction()
        {
            m_This->I2C1_DMA_Tx_Done_Handler();            
        }
        
        void I2CDMA::I2C2_DMA_Rx_Callback::CallbackFunction()
        {
            
        }
        
        void I2CDMA::I2C2_DMA_Tx_Callback::CallbackFunction()
        {
            
        }
}
