/******************
** FILE: I2CIntr.cpp
**
** DESCRIPTION:
**  I2CIntr implementation in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
******************/

#include "I2CIntr.h"


namespace HAL
{
    
    I2CIntr::I2CIntr(Pin_t scl, Pin_t sda, Hz_t Hz) :
        m_sclPin(scl,GpioOutput::AF_OPEN_DRAIN),
        m_sdaPin(sda,GpioOutput::AF_OPEN_DRAIN),
        m_hz(Hz),
        m_I2CState(RESET),
        m_I2C1_DMA_Rx_Callback(this),
        m_I2C1_DMA_Tx_Callback(this),
        m_I2C2_DMA_Rx_Callback(this),
        m_I2C2_DMA_Tx_Callback(this)
        {
            if((scl == Gpio::B6) && (sda == Gpio::B7))
            {
                m_I2Cx = I2C1;
            }
            else if((scl == Gpio::B10) && (sda == Gpio::B11))
            {
                m_I2Cx = I2C2;
            }
            else if((scl == Gpio::B8) && (sda == Gpio::B9))
            {
                // initialize with nullptr to configure it later in HwInit() with pin remap
                m_I2Cx = nullptr;
            }
            else
            {
                while(1); // Fatal Error
            }  
            
            m_RxQueueFullCallback = nullptr;
            m_TxQueueEmptyCallback = nullptr;
            m_SlaveRxDoneCallback = nullptr;
            m_SlaveTxDoneCallback = nullptr;
            
            m_I2CStatus = I2C_OK;
            
#ifdef I2C_INTR_DEBUG
            I2CStates_Idx = 0;
#endif
        }
        
        I2CIntr::I2CStatus_t I2CIntr::HwInit(void *pInitStruct)
        {
        	DMA::DMAConfig_t DMAConfig; 
            LL_I2C_InitTypeDef I2C_InitStruct;
            
            /* Set I2C_InitStruct fields to default values */
            I2C_InitStruct.PeripheralMode  = LL_I2C_MODE_I2C;
            I2C_InitStruct.ClockSpeed      = m_hz;
            I2C_InitStruct.DutyCycle       = LL_I2C_DUTYCYCLE_2;
            I2C_InitStruct.OwnAddress1     = I2C_OWN_SLAVE_ADDRESS;
            I2C_InitStruct.TypeAcknowledge = LL_I2C_NACK;
            I2C_InitStruct.OwnAddrSize     = LL_I2C_OWNADDRESS1_7BIT;           
            
            m_sclPin.HwInit();
            m_sdaPin.HwInit();
            
            m_sclPin.SetPinSpeed(GpioOutput::GPIO_OUTPUT_SPEED_HIGH);
            m_sdaPin.SetPinSpeed(GpioOutput::GPIO_OUTPUT_SPEED_HIGH);
            
            if(m_I2Cx == nullptr)
            {
                // Remap B8 and B9 Pin in I2C mode 
            }
            
            ClockEnable();
            
            if(pInitStruct == nullptr)
            {
                LL_I2C_Init(m_I2Cx, &I2C_InitStruct);
            }
            else
            {
                LL_I2C_Init(m_I2Cx, (LL_I2C_InitTypeDef*)pInitStruct);
            }        
            
            //LL_I2C_SetOwnAddress1(m_I2Cx,I2C_SLAVE_ADDRESS,LL_I2C_OWNADDRESS1_7BIT);
            //LL_I2C_SetOwnAddress2(m_I2Cx,I2C_SLAVE_ADDRESS-4);
            //LL_I2C_EnableOwnAddress2(m_I2Cx);
            //LL_I2C_EnableGeneralCall(m_I2Cx);				
			
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
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C1_EV_IRQn,0,this);
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C1_ER_IRQn,0,this);
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
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C2_EV_IRQn,0,this);
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C2_ER_IRQn,0,this);
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
        void I2CIntr::ClockEnable()
        {
            if(m_I2Cx == I2C1)
            {
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
            }
            else if(m_I2Cx == I2C2)
            {
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
            }        
        }
        
        void I2CIntr::ClockDisable()
        {
            if(m_I2Cx == I2C1)
            {
                LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1);
            }
            else if(m_I2Cx == I2C2)
            {
                LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C2);
            }        
        }
        
    I2CIntr::I2CStatus_t I2CIntr::TxPoll(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen)
    {
        if(m_I2CState != READY)
            return I2C_BUSY;
        
        if( (TxLen == 0) || (TxBuf == nullptr) )
        {          
            I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
            m_I2CStatus = I2C_INVALID_PARAMS;
            return I2C_INVALID_PARAMS;                
        }          
        
        /* Wait until BUSY flag is reset */
        if( Busy(I2C_TIMEOUT) == true )
        {
            I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
            return I2C_BUSY_TIMEOUT;
        }
        
        /* Disable Pos */
        m_I2Cx->CR1 &= ~I2C_CR1_POS;
        
        /* Generate Start */
        m_I2Cx->CR1 |= I2C_CR1_START;
        
        /* Check if Start condition generated successfully*/
        if(StartConditionGenerated(I2C_TIMEOUT) == false)
        {         
            I2C_DEBUG_LOG(I2C_START_TIMEOUT);
            return I2C_START_TIMEOUT;                
        }            
        
        /* Send slave address */
        m_I2Cx->DR = SlaveAddress & I2C_DIR_WRITE;
        
        /* Wait until ADDR flag is set */
        if(SlaveAddressSent(I2C_TIMEOUT) == false)
        {          
            I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
            return I2C_ADDR_TIMEOUT;                
        }
        
        /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(m_I2Cx);
        
        while(TxLen > 0U)
        {
            /* Wait until TXE flag is set */
            if(TransmitterEmpty(I2C_TIMEOUT) == false)
            {       
                if( (bool)LL_I2C_IsActiveFlag_AF(m_I2Cx) == true )
                {
                    /* Generate Stop */
                    m_I2Cx->CR1 |= I2C_CR1_STOP; 
                    
                    I2C_DEBUG_LOG(I2C_ACK_FAIL);
                    return I2C_ACK_FAIL; 
                }
                
                I2C_DEBUG_LOG(I2C_TXE_TIMEOUT);
                return I2C_TXE_TIMEOUT;
            }                 
            
            /* Write data to DR */
            m_I2Cx->DR = *TxBuf++;                
            TxLen--;     
            
            if( (bool)LL_I2C_IsActiveFlag_BTF(m_I2Cx) == true)
            {
                /* Write data to DR */
                m_I2Cx->DR = *TxBuf++;                
                TxLen--;                    
            }
            
            /* Wait until BTF flag is set */
            if(TransferDone(I2C_TIMEOUT) == false)
            {
                if( (bool)LL_I2C_IsActiveFlag_AF(m_I2Cx) == true )
                {
                    /* Generate Stop */
                    m_I2Cx->CR1 |= I2C_CR1_STOP;                 
                    I2C_DEBUG_LOG(I2C_ACK_FAIL);
                    return I2C_ACK_FAIL;
                }
                I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
                return I2C_BTF_TIMEOUT;
            }                
        }                  
        
        /* Generate Stop */
        m_I2Cx->CR1 |= I2C_CR1_STOP;
        
        I2C_DEBUG_LOG(I2C_OK);
        return I2C_OK; 
    }
    
    I2CIntr::I2CStatus_t I2CIntr::RxPoll(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen)
    {
        if(m_I2CState != READY)
            return I2C_BUSY;
        
        if( (RxLen == 0) || (RxBuf == nullptr) )
        {          
            I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
            m_I2CStatus = I2C_INVALID_PARAMS;
            return I2C_INVALID_PARAMS;                
        }          
        
        /* Wait until BUSY flag is reset */
        if( Busy(I2C_TIMEOUT) == true )
        {
            I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
            return I2C_BUSY_TIMEOUT;
        }
        
        /* Disable Pos */
        m_I2Cx->CR1 &= ~I2C_CR1_POS;
        
        /* Generate Start */
        m_I2Cx->CR1 |= I2C_CR1_START;
        
        /* Check if Start condition generated successfully*/
        if(StartConditionGenerated(I2C_TIMEOUT) == false)
        {          
            I2C_DEBUG_LOG(I2C_START_TIMEOUT);
            return I2C_START_TIMEOUT;                
        }
        
        /* Send slave address */
        m_I2Cx->DR = SlaveAddress | I2C_DIR_READ;
        
        /* Wait until ADDR flag is set */
        if(SlaveAddressSent(I2C_TIMEOUT) == false)
        {          
            I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
            return I2C_ADDR_TIMEOUT;                
        }          
        
        if(RxLen == 1)
        {
            /* Disable Acknowledge */
            m_I2Cx->CR1 &= ~I2C_CR1_ACK;
            
            /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
            software sequence must complete before the current byte end of transfer */
            __disable_irq();
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(m_I2Cx);
            
            /* Generate Stop */
            m_I2Cx->CR1 |= I2C_CR1_STOP;
            
            /* Re-enable IRQs */
            __enable_irq();             
        }
        else if(RxLen == 2)
        {
            /* Enable Pos */
            m_I2Cx->CR1 |= I2C_CR1_POS;
            
            /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
            software sequence must complete before the current byte end of transfer */
            __disable_irq();
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(m_I2Cx);
            
            /* Disable Acknowledge */
            m_I2Cx->CR1 &= ~I2C_CR1_ACK;
            
            /* Re-enable IRQs */
            __enable_irq(); 
        }
        else
        {
            /* Enable Acknowledge */
            m_I2Cx->CR1 |= I2C_CR1_ACK;
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(m_I2Cx);
        }
        
        while(RxLen > 0U)
        {
            if(RxLen <= 3U)
            {
                /* One byte */
                if(RxLen == 1U)
                {
                    /* Wait until RXNE flag is set */
                    if(DataAvailableRXNE(I2C_TIMEOUT) == false)      
                    {
                        I2C_DEBUG_LOG(I2C_RXNE_TIMEOUT);
                        return I2C_RXNE_TIMEOUT;
                    }
                    
                    /* Read data from DR */
                    (*RxBuf) = m_I2Cx->DR;
                    RxLen--;
                }
                /* Two bytes */
                else if(RxLen == 2U)
                {
                    /* Wait until BTF flag is set */
                    if(TransferDone(I2C_TIMEOUT) == false)
                    {         
                        I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
                        return I2C_BTF_TIMEOUT;                
                    }
                    
                    /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                    software sequence must complete before the current byte end of transfer */
                    __disable_irq();
                    
                    /* Generate Stop */
                    m_I2Cx->CR1 |= I2C_CR1_STOP;
                    
                    /* Read data from DR */
                    (*RxBuf++)= m_I2Cx->DR;
                    RxLen--;
                    
                    /* Re-enable IRQs */
                    __enable_irq();
                    
                    /* Read data from DR */
                    (*RxBuf++) = m_I2Cx->DR;
                    RxLen--;
                }
                /* 3 Last bytes */
                else
                {
                    /* Wait until BTF flag is set */
                    if(TransferDone(I2C_TIMEOUT) == false)
                    {          
                        I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
                        return I2C_BTF_TIMEOUT;                
                    }
                    
                    /* Disable Acknowledge */
                    m_I2Cx->CR1 &= ~I2C_CR1_ACK;
                    
                    /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                    software sequence must complete before the current byte end of transfer */
                    __disable_irq();
                    
                    /* Read data from DR */
                    (*RxBuf++) = m_I2Cx->DR;
                    RxLen--;
                    
                    /* Wait until BTF flag is set */
                    if(TransferDone(I2C_TIMEOUT) == false)
                    {         
                        I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
                        return I2C_BTF_TIMEOUT;                
                    }
                    
                    /* Generate Stop */
                    m_I2Cx->CR1 |= I2C_CR1_STOP;
                    
                    /* Read data from DR */
                    (*RxBuf++) = m_I2Cx->DR;
                    RxLen--;
                    
                    /* Re-enable IRQs */
                    __enable_irq(); 
                    
                    /* Read data from DR */
                    (*RxBuf++) = m_I2Cx->DR;
                    RxLen--;
                }
            }
            else
            {
                /* Wait until RXNE flag is set */
                if(DataAvailableRXNE(I2C_TIMEOUT) == false)      
                {
                    I2C_DEBUG_LOG(I2C_RXNE_TIMEOUT);
                    return I2C_RXNE_TIMEOUT;
                }
                
                /* Read data from DR */
                (*RxBuf++) = m_I2Cx->DR;
                RxLen--;
                
                if((bool)LL_I2C_IsActiveFlag_BTF(m_I2Cx) == true)
                {
                    /* Read data from DR */
                    (*RxBuf++) = m_I2Cx->DR;
                    RxLen--;
                }
            }
        }       
        
        I2C_DEBUG_LOG(I2C_OK);
        return I2C_OK;
    }
    
    bool I2CIntr::DetectSlave(uint8_t SlaveAddress)
        {
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            /* Generate Start */
            m_I2Cx->CR1 |= I2C_CR1_START;
            
            /* Check if Start condition generated successfully*/
            if(StartConditionGenerated(I2C_TIMEOUT) == false)
            { 
                I2C_DEBUG_LOG(I2C_START_TIMEOUT);
                return I2C_START_TIMEOUT;                
            }
            
            /* Send slave address */
            m_I2Cx->DR = SlaveAddress;
            
            /* Wait until ADDR flag is set */
            if(SlaveAddressSent(I2C_TIMEOUT) == false)
            {
                I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
                return I2C_ADDR_TIMEOUT;                
            }
            
            I2C_DEBUG_LOG(I2C_OK);
            return I2C_OK;
        }
        
        void I2CIntr::ScanBus(uint8_t* pFoundDevices, uint8_t size)
        {
            uint8_t slave, i=0;
            
            for(slave = 0; slave < 255; slave++)
            {
                if(DetectSlave(slave) == 0)
                {
                    pFoundDevices[i++ % size] = slave;
                }
                else
                {
                    LL_I2C_ClearFlag_AF(m_I2Cx);
                    LL_I2C_GenerateStopCondition(m_I2Cx);
                }
            }
        }
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterTx(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        { 
            if(m_I2CState != READY)
                return I2C_BUSY;
            
            if( (TxLen == 0) || (TxBuf == nullptr) || (pStatus == nullptr) )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                m_I2CStatus = I2C_INVALID_PARAMS;
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
            
            //InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_ENABLE_ALL);
            Enable_EVT_BUF_ERR_Interrupt();
            
            /* Generate Start */
            m_I2Cx->CR1 |= I2C_CR1_START;
            I2C_LOG_STATES(I2C_LOG_START_MASTER_TX);
            
            return I2C_OK;
        }
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterTx(Transaction_t* pTransaction)
        { 
            if( pTransaction == nullptr )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                m_I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }
            
            return MasterTx(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen,pTransaction->pStatus, pTransaction->XferDoneCallback);
        }
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterRx(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        {            
            if(m_I2CState != READY)
                return I2C_BUSY;
            
            if( (RxLen == 0) || (RxBuf == nullptr) || (pStatus == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                m_I2CStatus = I2C_INVALID_PARAMS;
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
            
            //InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_ENABLE_ALL);
            Enable_EVT_BUF_ERR_Interrupt();
            
            /* Generate Start */
            m_I2Cx->CR1 |= I2C_CR1_START;
            
            I2C_LOG_STATES(I2C_LOG_START_MASTER_RX);
            
            I2C_DEBUG_LOG(I2C_OK);
            return I2C_OK;
        }
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterRx(Transaction_t* pTransaction)
        { 
            if( pTransaction == nullptr )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                m_I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }
            
            return MasterRx(pTransaction->SlaveAddress, pTransaction->RxBuf ,pTransaction->RxLen,pTransaction->pStatus, pTransaction->XferDoneCallback);
        }
        
        I2CIntr::I2CStatus_t I2CIntr::MasterTxRx(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen, uint8_t RepeatedStart,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        {
            if(m_I2CState != READY)
                return I2C_BUSY;
            
            if( ((TxBuf == nullptr) && (RxBuf == nullptr)) || (pStatus == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                m_I2CStatus = I2C_INVALID_PARAMS;
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
        
        I2CIntr::I2CStatus_t I2CIntr::MasterTxRx(Transaction_t* pTransaction)
        {
            if( pTransaction == nullptr )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                m_I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }
            
            return MasterTxRx(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen, pTransaction->RxBuf ,
                              pTransaction->RxLen, pTransaction->RepeatedStart,pTransaction->pStatus, pTransaction->XferDoneCallback);            
        }
        
        I2CIntr::I2CStatus_t I2CIntr::MasterTxRx_DMA(Transaction_t* pTransaction)
        {
            if( pTransaction == nullptr )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                m_I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }
            
            return MasterTxRx_DMA(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen, pTransaction->RxBuf ,
                              pTransaction->RxLen, pTransaction->RepeatedStart,pTransaction->pStatus, pTransaction->XferDoneCallback);            
        }
        
        I2CIntr::I2CStatus_t I2CIntr::Post(Transaction_t* pTransaction, uint32_t Mode)
        {            
            if( (pTransaction == nullptr) || ((pTransaction->TxBuf == nullptr) && (pTransaction->RxBuf == nullptr)) || (pTransaction->pStatus == nullptr)  )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                m_I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }
            
            if(m_I2CState != READY)
            {      
                if( !m_I2CTxnQueue.Write(pTransaction) )
                {
                    I2C_LOG_STATES(I2C_LOG_TXN_QUEUED);
                    return I2C_TXN_POSTED;
                }
                else 
                {
                    I2C_LOG_STATES(I2C_LOG_TXN_QUEUE_ERROR);                    
                    return I2C_TXN_QUEUE_ERROR;
                }                
            }
            else
            {
                if(Mode == 0)
                {
                    return MasterTxRx(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen, pTransaction->RxBuf ,
                                      pTransaction->RxLen, pTransaction->RepeatedStart,pTransaction->pStatus,pTransaction->XferDoneCallback);  
                }
                else
                {
                    return MasterTxRx_DMA(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen, pTransaction->RxBuf ,
                                      pTransaction->RxLen, pTransaction->RepeatedStart,pTransaction->pStatus,pTransaction->XferDoneCallback);
                }
            }                      
        }
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterTx_DMA(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        { 
            if(m_I2CState != READY)
                return I2C_BUSY;
            
            if( (TxLen == 0) || (TxBuf == nullptr) || (pStatus == nullptr) )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                m_I2CStatus = I2C_INVALID_PARAMS;
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
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterTxRx_DMA(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen, 
                                                      uint8_t RepeatedStart,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        {             
            if(m_I2CState != READY)
                return I2C_BUSY;
            
           if( ((TxBuf == nullptr) && (RxBuf == nullptr)) || (pStatus == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                m_I2CStatus = I2C_INVALID_PARAMS;
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
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterRx_DMA(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        { 
            if(m_I2CState != READY)
                return I2C_BUSY;
            
            if( (RxLen == 0) || (RxBuf == nullptr) || (pStatus == nullptr) )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                m_I2CStatus = I2C_INVALID_PARAMS;
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
        
        I2CIntr::I2CStatus_t I2CIntr::SlaveRx(uint8_t* RxBuf, uint32_t RxLen, I2CCallback_t XferDoneCallback )
        {
            return I2C_OK; 
        }
        
        I2CIntr::I2CStatus_t I2CIntr::SlaveTx(uint8_t* TxBuf, uint32_t TxLen, I2CCallback_t XferDoneCallback )
        {          
            return I2C_OK;
        }
        //#pragma inline = forced
        void I2CIntr::TxnDoneHandler(uint32_t StopFlag)
        { 
            Transaction_t*          _pCurrentTxn;
            if(m_Transaction.RxLen != 0)
            {              
                // From here we don't expect buffer interrupts till SB,ADDR is handled
                Disable_BUF_Interrupt();
				
                // Rx is pending, generate start or repeated start
                if(m_Transaction.RepeatedStart)
                {
                    /* Generate Start */
                    m_I2Cx->CR1 |= I2C_CR1_START;
                    
                     m_I2CState = MASTER_RX_REPEATED_START;
                    
                    I2C_LOG_STATES(I2C_LOG_REPEATED_START);                     
                }
                else
                {
                    /* Generate Stop */
                    m_I2Cx->CR1 |= StopFlag;//I2C_CR1_STOP;
                    
                    if (StopFlagCleared(I2C_TIMEOUT) == true)
                    {
                        m_I2CStatus = I2C_STOP_TIMEOUT;
                        *(m_Transaction.pStatus) = I2CIntr::I2C_STOP_TIMEOUT;
                    }  
                    
                    /* Generate Start */
                    m_I2Cx->CR1 |= I2C_CR1_START;   
                    
                    //Kepp the transmission mode consistent Interrupt/DMA
                    if(m_I2CState == MASTER_TX)
                    {
                        m_I2CState = MASTER_RX;
                    }
                    else
                    {
                        m_I2CState = MASTER_RX_DMA;  
                        
                        /* Enable DMA Request */
                        //m_I2Cx->CR2 |= I2C_CR2_DMAEN;
                    }
                }				 
                return;				
            }
            else 
            {                
                // TxLen and RxLen is 0, Txn finished, Load neext Txn if available
                
                /* Generate Stop */
                m_I2Cx->CR1 |= StopFlag;
                
                (*m_Transaction.pStatus) = I2C_XFER_DONE;
                
                // Transaction ended here, call the completion callback
                if(m_Transaction.XferDoneCallback)
                    m_Transaction.XferDoneCallback->CallbackFunction();
                
                // Check if there is some transaction pending
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
                        m_Transaction.pStatus            = _pCurrentTxn->pStatus; 
                        m_Transaction.XferDoneCallback   = _pCurrentTxn->XferDoneCallback;
                    }
                    
                    if(m_Transaction.TxLen) 
                    {
                        // Next transaction starts with Tx mode first
                        if ( (m_I2CState == MASTER_TX) || (m_I2CState == MASTER_RX) )
                        {
                            m_I2CState = MASTER_TX;
                        }
                        else
                        {
                            m_I2CState = MASTER_TX_DMA;

							 /* Load DMA Tx transaction*/
							_DMA->Load(I2C1_TX_DMA_CHANNEL, (uint32_t)&(m_I2Cx->DR), (uint32_t)m_Transaction.TxBuf, m_Transaction.TxLen, LL_DMA_DIRECTION_MEMORY_TO_PERIPH );
							
                            /* Load DMA Rx transaction*/
							_DMA->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)&(m_I2Cx->DR), (uint32_t)m_Transaction.RxBuf, m_Transaction.RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY );
                            
							/* Enable DMA Request */
                            m_I2Cx->CR2 |= I2C_CR2_DMAEN;
                        }
                    }
                    else 
                    {
                        // Next transaction starts with Rx only mode
                        if ( (m_I2CState == MASTER_TX) || (m_I2CState == MASTER_RX) )
                        {
                                m_I2CState = MASTER_RX;
                        }
                        else
                        {
                            m_I2CState = MASTER_RX_DMA; 
                            
                            /* Load DMA Rx transaction*/
							_DMA->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)&(m_I2Cx->DR), (uint32_t)m_Transaction.RxBuf, m_Transaction.RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY );
                            
                            /* Enable DMA Request */
                           m_I2Cx->CR2 |= I2C_CR2_DMAEN;
                        }
                    }
                    
                    /* Disable Pos */
                    m_I2Cx->CR1 &= ~I2C_CR1_POS;
                    
					/* Enable Acknowledge */
					m_I2Cx->CR1 |= I2C_CR1_ACK;
                    
                    /* Generate Start */
                    m_I2Cx->CR1 |= I2C_CR1_START;
                    
                    I2C_LOG_STATES(I2C_LOG_TXN_DEQUEUED); 
                }
                else
                {
                    // Txlen = 0, RxLen = 0, Txn Queue empty
                    if (StopFlagCleared(I2C_TIMEOUT) == true)
                    {
                        m_I2CStatus = I2C_STOP_TIMEOUT;
                        *(m_Transaction.pStatus) = I2CIntr::I2C_STOP_TIMEOUT;
                    } 
                    else
                    {
                        Disable_EVT_BUF_ERR_Interrupt();
                        
                        //DataAvailableRXNE(1000000);
                        
                        m_I2CState = READY;

						/* Disable Last DMA */
                		m_I2Cx->CR2 &= ~I2C_CR2_LAST;
                
						/* Disable DMA Request */            
                		m_I2Cx->CR2 &= ~I2C_CR2_DMAEN;               
						
						I2C_LOG_STATES(I2C_LOG_TXN_DONE_ALL); 
                        
                    }
                }
            }                         
        }
        
        void I2CIntr::SetCallback(I2CCallbackType_t I2CCallbackType, I2CCallback_t I2CCallback)
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
        void I2CIntr::ISR( IRQn_Type event )
        {                
            switch(POSITION_VAL(m_I2Cx->SR1))
            {
            case I2C_SR1_SB_Pos : Master_SB_Handler(); 
            break;
            case I2C_SR1_ADDR_Pos:                    
                if((m_I2CState == MASTER_TX) || (m_I2CState == MASTER_RX) || (m_I2CState == MASTER_TX_DMA) || (m_I2CState == MASTER_RX_DMA) )   Master_ADDR_Handler();
                else if((m_I2CState == READY) || (m_I2CState == SLAVE_RX) || (m_I2CState == MASTER_TX_DMA))    Slave_ADDR_Handler(); // changing to Slave Tx here
                else while(1);/* Fatal Error*/                        
                break;
                
            case I2C_SR1_BTF_Pos :  
                if     ((m_I2CState == MASTER_RX) || (m_I2CState == MASTER_RX_DMA))     Master_Rx_BTF_Handler();
                else if((m_I2CState == MASTER_TX) || (m_I2CState == MASTER_TX_DMA))     Master_Tx_BTF_Handler();
                else if(m_I2CState == SLAVE_RX )     Slave_Rx_BTF_Handler();
                else if(m_I2CState == SLAVE_TX )     Slave_Tx_BTF_Handler(); 
                else if(m_I2CState == MASTER_RX_REPEATED_START )     return;
                else                                while(1);/* Fatal Error*/ 
                break;
                
            case I2C_SR1_ADD10_Pos : 
                break;
                
            case I2C_SR1_STOPF_Pos : Slave_STOP_Handler(); 
            break;
            
            case I2C_SR1_RXNE_Pos : 
                if     (m_I2CState == MASTER_RX)     Master_RxNE_Handler(); 
                else if(m_I2CState == SLAVE_RX )     Slave_RxNE_Handler();  
                else                                while(1);/* Fatal Error*/ 
                break;
                
            case I2C_SR1_TXE_Pos : 
                if     (m_I2CState == MASTER_TX)     Master_TxE_Handler(); 
                else if(m_I2CState == SLAVE_TX )     Slave_TxE_Handler(); 
                else                                while(1);/* Fatal Error*/                           
                break;
                
            case I2C_SR1_BERR_Pos : 
                Master_BERR_Handler();
                break;
                
            case I2C_SR1_ARLO_Pos : 
                Master_AL_Handler(); 
                break;
                
            case I2C_SR1_AF_Pos :
                if     (m_I2CState == MASTER_TX)     Master_AF_Handler(); 
                else if(m_I2CState == SLAVE_TX )     Slave_AF_Handler();
                else                                while(1);/* Fatal Error*/   
                break;
                
            case I2C_SR1_OVR_Pos : 
                Master_OVR_Handler(); 
                break;
                
            case I2C_SR1_PECERR_Pos :
                while(1);
                break;
                
            default : /*while(1);*/return;
            }
            //}
        }       
        
		void I2CIntr::Master_SB_Handler()
        {
            if( (m_I2CState == MASTER_TX) || (m_I2CState == MASTER_TX_DMA) )
            {
                m_I2Cx->DR = m_Transaction.SlaveAddress & I2C_DIR_WRITE; 
                I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
            }
            else if((m_I2CState == MASTER_RX) || (m_I2CState == MASTER_RX_DMA))  
            {
                /* start listening RxNE and TxE interrupts */  
                if((m_I2CState == MASTER_RX))
                    Enable_BUF_Interrupt();
                
#ifndef I2C_RX_METHOD_1                
                if(m_Transaction.RxLen == 2U) 
                {
                    /* Enable Pos */
                    m_I2Cx->CR1 |= I2C_CR1_POS;
                }
#endif 
                m_I2Cx->DR = m_Transaction.SlaveAddress | I2C_DIR_READ;
                
                I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX);
            }
            else if(m_I2CState == MASTER_RX_REPEATED_START)
            {
                /* Repeated start is handled here, clear the flag*/
                m_Transaction.RepeatedStart = 0;         
                
#ifndef I2C_RX_METHOD_1                
                if(m_Transaction.RxLen == 2U) 
                {
                    /* Enable Pos */
                    m_I2Cx->CR1 |= I2C_CR1_POS;
                }
#endif          
                m_I2Cx->DR = m_Transaction.SlaveAddress | I2C_DIR_READ;  
                
                if(m_I2Cx->CR2 & I2C_CR2_DMAEN)
                {
                    m_I2CState = MASTER_RX_DMA;
					I2C_LOG_STATES(I2C_LOG_REPEATED_START_MASTER_RX_DMA);
                }
                else
                {
                 	/* start listening RxNE and TxE interrupts */                
                	Enable_BUF_Interrupt();
					
                    m_I2CState = MASTER_RX;

					I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX_REPEATED_START);
                }
            }
            else
            {
                while(1);
            }
        }
        
        void I2CIntr::Master_ADDR_Handler()
        {
            if(m_I2CState == MASTER_RX)
            {
                if(m_Transaction.RxLen == 1U)   
                {
                    /* Clear ADDR flag */
                    LL_I2C_ClearFlag_ADDR(m_I2Cx);
                    
                    /* Disable Acknowledge */
                    m_I2Cx->CR1 &= ~I2C_CR1_ACK;
                    
                    /* Generate Stop */
                    m_I2Cx->CR1 |= I2C_CR1_STOP;
                    
                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1);                                       
                }   
#ifndef I2C_RX_METHOD_1 
                else if(m_Transaction.RxLen == 2U)   
                {
                    /* Clear ADDR flag */
                    LL_I2C_ClearFlag_ADDR(m_I2Cx);
                    
                    /* Disable Acknowledge */
                    m_I2Cx->CR1 &= ~I2C_CR1_ACK;
                    
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
            else if(m_I2CState == MASTER_TX)
            {                
                /* Clear ADDR flag */
                LL_I2C_ClearFlag_ADDR(m_I2Cx);
                
                if(m_Transaction.TxLen > 0)
                {                    
                    m_I2Cx->DR = (*m_Transaction.TxBuf++);
                    m_Transaction.TxLen--;
                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0);
                }
                else
                {
                    //while(1);
                }
            }
            else if( m_I2CState == MASTER_TX_DMA)
            {                 
                /* Clear ADDR flag */
                LL_I2C_ClearFlag_ADDR(m_I2Cx);
                
                I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_DMA);
            }
            else if(m_I2CState == MASTER_RX_DMA)
            {
//                if((m_Transaction.RxLen == 1U))  
//                {
//                    /* Disable Acknowledge */
//                    m_I2Cx->CR1 &= ~I2C_CR1_ACK;
//                    
//                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_DMA_SIZE_1);
//                }
//                else if(m_Transaction.RxLen == 2U) 
//                {
//                    /* Enable Last DMA bit */
//                    m_I2Cx->CR2 |= I2C_CR2_LAST;
//                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_DMA_SIZE_2);
//                }
                
                /* Clear ADDR flag */
                LL_I2C_ClearFlag_ADDR(m_I2Cx);
                
                I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_DMA);
            }
            else
            {
                while(1);
            }
        }
        
        void I2CIntr::Master_RxNE_Handler()
        {
#ifdef I2C_RX_METHOD_1
            if(m_Transaction.RxLen == 2U)
            {
                /* Read data from DR */
                (*m_Transaction.RxBuf++) = m_I2Cx->DR;                
                m_Transaction.RxLen--;
                
                /* Disable Acknowledge */
                m_I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                /* Generate Stop */
                m_I2Cx->CR1 |= I2C_CR1_STOP;   
                
                I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2);
            }
            else if(m_Transaction.RxLen == 1U)
            {
                /* Read data from DR */
                (*m_Transaction.RxBuf++) = m_I2Cx->DR;                
                m_Transaction.RxLen--;
                
                /* Disable EVT, BUF and ERR interrupt */
                Disable_EVT_BUF_ERR_Interrupt();
                
                I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_1);
                
                //Stop();
                /*STOP = FALSE*/
                TxnDoneHandler(0);                
            }
            else
            {
                /* Read data from DR */
                (*m_Transaction.RxBuf++) = m_I2Cx->DR;                 
                m_Transaction.RxLen--;
                
                I2C_LOG_STATES(I2C_LOG_RXNE_MASTER);
            }
#else
            if(m_Transaction.RxLen > 3U)
            {
                /* Read data from DR */
                (*m_Transaction.RxBuf++) = m_I2Cx->DR;                
                m_Transaction.RxLen--;                
            }
            else if( (m_Transaction.RxLen == 2U) || (m_Transaction.RxLen == 3U) )
            {
                // Do nothing here, data 3 is here in data register.
                // Let the data 2 also accumulate in shift register in next BTF.
                // After that we read data 3 and data2 in the Master Rx BTF handler
                I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2_OR_3);
            }
            else
            {    // This is for the case when Last byte/data1 is to be read
                
                /* Read data from DR */
                (*m_Transaction.RxBuf++) = m_I2Cx->DR;                
                m_Transaction.RxLen--;
                
                /* Disable EVT, BUF and ERR interrupt */
                Disable_EVT_BUF_ERR_Interrupt();
                
                I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_LAST);
                
                //Stop();
                /*STOP = FALSE*/
                TxnDoneHandler(0);                
            }            
#endif
        }        
        void I2CIntr::Master_Rx_BTF_Handler()
        {
            if( m_I2CState == MASTER_RX_REPEATED_START)
                return;            
            
            if(m_I2CState == MASTER_RX_DMA)            
			{                

				while(1);
//                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_DMA_STOP);                
//                TxnDoneHandler(I2C_CR1_STOP); 
//                return;
            }            
            if(m_Transaction.RxLen == 3U)
            {
                /* Disable Acknowledge */
                m_I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                // Read data3
                (*m_Transaction.RxBuf++) = m_I2Cx->DR;
                m_Transaction.RxLen--;
                
                /* Generate Stop */
                m_I2Cx->CR1 |= I2C_CR1_STOP; 
                
                // Read data2
                (*m_Transaction.RxBuf++) = m_I2Cx->DR;
                m_Transaction.RxLen--;                
                
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_3);
            }
            else if(m_Transaction.RxLen == 2U)
            {                 
                /* Generate Stop */
                m_I2Cx->CR1 |= I2C_CR1_STOP;         
                
                // Read data2
                (*m_Transaction.RxBuf++) = m_I2Cx->DR;
                m_Transaction.RxLen--;
                
                // Read data1
                (*m_Transaction.RxBuf++) = m_I2Cx->DR;
                m_Transaction.RxLen--; 
                
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED);
                
                if (StopFlagCleared(I2C_TIMEOUT) == true)
                {
                    m_I2CStatus = I2C_STOP_TIMEOUT;
                    (*m_Transaction.pStatus) = I2C_STOP_TIMEOUT;
                }
                
                //Stop();
                /*STOP = FALSE*/
                TxnDoneHandler(0); 
            }
            else 
            {
                /* Read data from DR */
                (*m_Transaction.RxBuf++) = m_I2Cx->DR;
                m_Transaction.RxLen--;
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_GT_3);
            }              
             
        }        
        
        void I2CIntr::Master_TxE_Handler()
        {
            if( m_I2CState == MASTER_RX_REPEATED_START)
                return;
            
            if(m_Transaction.TxLen > 0)
            {
                m_I2Cx->DR = (*m_Transaction.TxBuf++);
                m_Transaction.TxLen--;
                I2C_LOG_STATES(I2C_LOG_TXE);
            } 
            else
            {
                I2C_LOG_STATES(I2C_LOG_TXE_DONE);
                TxnDoneHandler(I2C_CR1_STOP);
            }
        }
        
        void I2CIntr::Master_Tx_BTF_Handler()
        {
            if(m_I2CState == MASTER_TX_DMA)
            {
//                if(m_Transaction.RxLen != 0)
//				{
//                    /* Load DMA Rx transaction*/
//	                _DMA->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)m_Transaction.RxBuf, (uint32_t)&(m_I2Cx->DR), m_Transaction.RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY );
//				}                
				I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_DMA_STOP);
                TxnDoneHandler(I2C_CR1_STOP);  
            }            
            else if(m_Transaction.TxLen > 0)
            {
                m_I2Cx->DR = (*m_Transaction.TxBuf++);
                m_Transaction.TxLen--;
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_GT_0);
            }
            else
            {
                TxnDoneHandler(I2C_CR1_STOP);
            }
        }
        
        void I2CIntr::Slave_ADDR_Handler()
        {            
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
                m_I2CState = SLAVE_RX;               
            }
            else
            {
                m_I2CState = SLAVE_TX;
            }             
        }
        
        void I2CIntr::Slave_Tx_BTF_Handler()
        {
            uint8_t Data;
            if(m_I2CSlaveTxQueue.IsQueueEmpty() != true)
            {
                m_I2CSlaveTxQueue.Read(Data);
                
                m_I2Cx->DR = Data;
                
                I2C_LOG_STATES(I2C_LOG_BTF_SLAVE_TX);
            }
            else
            {                
                m_I2Cx->DR = 0xDE; // Send some junk data
                
                I2C_LOG_STATES(I2C_LOG_BTF_SLAVE_TX_QUEUE_EMPTY);
                
                if(m_TxQueueEmptyCallback)
                    m_TxQueueEmptyCallback->CallbackFunction();
            }
        }
        
        void I2CIntr::Slave_Rx_BTF_Handler()
        {
            /* Queue the received data */
            if(m_I2CSlaveRxQueue.IsQueueFull() == false)
            {
                m_I2CSlaveRxQueue.Write(m_I2Cx->DR);
                I2C_LOG_STATES(I2C_LOG_BTF_SLAVE_RX_QUEUED);
            }
            else
            {                
                I2C_LOG_STATES(I2C_LOG_BTF_SLAVE_RX_QUEUE_FULL);
            }
        }
        
        void I2CIntr::Slave_TxE_Handler()
        {
            uint8_t Data;
            if(m_I2CSlaveTxQueue.IsQueueEmpty() != true)
            {
                m_I2CSlaveTxQueue.Read(Data);
                
                m_I2Cx->DR = Data;
                
                I2C_LOG_STATES(I2C_LOG_TxE_SLAVE_TX);
            }
            else
            {                
                m_I2Cx->DR = 0xDE; // Send some junk data
                
                I2C_LOG_STATES(I2C_LOG_TxE_SLAVE_TX_QUEUE_EMPTY);
                
                if(m_TxQueueEmptyCallback)
                    m_TxQueueEmptyCallback->CallbackFunction();
            }            
        }
        
        void I2CIntr::Slave_RxNE_Handler()
        {
            /* Queue the received data */
            if(m_I2CSlaveRxQueue.IsQueueFull() == false)
            {
                m_I2CSlaveRxQueue.Write(m_I2Cx->DR);
                I2C_LOG_STATES(I2C_LOG_RXNE_SLAVE_RX_QUEUED);
            }
            else
            {
                if(m_RxQueueFullCallback)
                    m_RxQueueFullCallback->CallbackFunction();
                
                I2C_LOG_STATES(I2C_LOG_RXNE_SLAVE_RX_QUEUE_FULL);
            }           
        }
        
        void I2CIntr::Slave_STOP_Handler()
        {
            LL_I2C_ClearFlag_STOP(m_I2Cx);            
            
            /* Execute the RxDone Callback */
            if(m_SlaveRxDoneCallback)
                m_SlaveRxDoneCallback->CallbackFunction();
            
            m_I2CState = READY;
            
            I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
        }       
        
        void I2CIntr::Slave_AF_Handler()
        {            
            LL_I2C_ClearFlag_AF(m_I2Cx);            
            
            if(m_SlaveTxDoneCallback)
                m_SlaveTxDoneCallback->CallbackFunction();
            
            m_I2CState = READY;
            
            I2C_LOG_STATES(I2C_LOG_AF_SLAVE_ACK_FAIL);
        }
        
        void I2CIntr::Master_BERR_Handler()
        {
            I2C_LOG_STATES(I2C_LOG_BTF_MASTER_BERR);
            LL_I2C_ClearFlag_BERR(m_I2Cx);
            /* Workaround: Start cannot be generated after a misplaced Stop */
            SoftReset();
            m_I2CStatus = I2C_BUS_ERROR;
        }
        
        void I2CIntr::Master_AF_Handler()
        {
            m_I2CStatus = I2C_ACK_FAIL;
            LL_I2C_ClearFlag_AF(m_I2Cx);
            I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ACK_FAIL);
        }
        
        void I2CIntr::Master_AL_Handler()
        {
            m_I2CStatus = I2C_ARB_LOST;
            LL_I2C_ClearFlag_ARLO(m_I2Cx);
            I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ARB_LOST);
        }
        
        void I2CIntr::Master_OVR_Handler()
        {
            m_I2CStatus = I2C_DATA_OVR;
            LL_I2C_ClearFlag_OVR(m_I2Cx);
            I2C_LOG_STATES(I2C_LOG_BTF_MASTER_DATA_OVR);
        }  
        
		void I2CIntr::I2C1_DMA_Tx_Done_Handler()
		{			      
            /* Transfer Complete Interrupt management ***********************************/
            if (LL_DMA_IsActiveFlag_TC6(_DMA->_DMAx))
            {
                /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
                if(LL_DMA_GetMode(_DMA->_DMAx,I2C1_TX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
                {
                    /* Disable all the DMA interrupt */
                    _DMA->DisableHalfTransferCompleteInterrupt(I2CIntr::I2C1_TX_DMA_CHANNEL);
                    _DMA->DisableTransferCompleteInterrupt(I2CIntr::I2C1_TX_DMA_CHANNEL);
                    _DMA->DisableTransferErrorInterrupt(I2CIntr::I2C1_TX_DMA_CHANNEL);
                }                 
                
                /* Clear the half transfer complete flag */
                LL_DMA_ClearFlag_TC6(_DMA->_DMAx);
                
                /* Clear the half transfer complete flag */
                LL_DMA_ClearFlag_HT6(_DMA->_DMAx);                 
                
                // This varoable is used by TxnDoneHandler() to keep track of completion status
                m_Transaction.TxLen = 0;                				
					
                I2C_LOG_STATES(I2CIntr::I2C_LOG_DMA_TX_DONE);
            }     
            /* Half Transfer Complete Interrupt management ******************************/
            else if (LL_DMA_IsActiveFlag_HT6(_DMA->_DMAx))
            {
                I2C_LOG_STATES(I2CIntr::I2C_LOG_DMA_HALF_TX_DONE);
                
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
                
                I2C_LOG_STATES(I2CIntr::I2C_LOG_DMA_TX_ERROR);                
            }            
        }				
        
		void I2CIntr::I2C1_DMA_Rx_Done_Handler()
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
                
                I2C_LOG_STATES(I2CIntr::I2C_LOG_DMA_RX_DONE);
				 
                TxnDoneHandler(0); 
            }           
            /* Half Transfer Complete Interrupt management ******************************/
            else if (LL_DMA_IsActiveFlag_HT7(_DMA->_DMAx))
            {
                I2C_LOG_STATES(I2CIntr::I2C_LOG_DMA_HALF_RX_DONE);
                
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
                
                I2C_LOG_STATES(I2CIntr::I2C_LOG_DMA_RX_ERROR);                
            }            
        }
        
        void I2CIntr::I2C1_DMA_Rx_Callback::CallbackFunction()
        {
            _This->I2C1_DMA_Rx_Done_Handler();
        }
        
        void I2CIntr::I2C1_DMA_Tx_Callback::CallbackFunction()
        {
            _This->I2C1_DMA_Tx_Done_Handler();            
        }
        
        void I2CIntr::I2C2_DMA_Rx_Callback::CallbackFunction()
        {
            
        }
        
        void I2CIntr::I2C2_DMA_Tx_Callback::CallbackFunction()
        {
            
        }
}
