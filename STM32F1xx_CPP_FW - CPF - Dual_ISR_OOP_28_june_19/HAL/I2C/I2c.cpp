/******************
** FILE: I2c.cpp
**
** DESCRIPTION:
**  I2c implementation in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
******************/

#include "I2c.h"


namespace HAL
{
    
    I2c::I2c(Pin_t scl, Pin_t sda, Hz_t Hz) :
        m_sclPin(scl,GpioOutput::AF_OPEN_DRAIN),
        m_sdaPin(sda,GpioOutput::AF_OPEN_DRAIN),
        m_hz(Hz),
        m_I2CState(RESET)
#if I2C_DMA
            ,m_I2C1_DMA_Rx_Callback(this),
            m_I2C1_DMA_Tx_Callback(this),
            m_I2C2_DMA_Rx_Callback(this),
            m_I2C2_DMA_Tx_Callback(this)
#endif
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
//                
//                m_RxQueueFullCallback = nullptr;
//                m_TxQueueEmptyCallback = nullptr;
//                m_SlaveRxDoneCallback = nullptr;
//                m_SlaveTxDoneCallback = nullptr;
                
                m_I2CStatus = I2C_OK;
            }
        
        I2c::I2CStatus_t I2c::HwInit(void *pInitStruct)
        {        	
            LL_I2C_InitTypeDef I2C_InitStruct;
            
            dbg_log_instance = Utils::DebugLog<DBG_LOG_TYPE>::GetInstance();
            dbg_log_instance->Enable(Utils::DebugLog<DBG_LOG_TYPE>::DBG_LOG_MODULE_ID_I2C);
            
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
#if I2C_DMA			
            DMA::DMAConfig_t DMAConfig; 
            
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
#endif            
            
            if(m_I2Cx == I2C1)
            {
#if (I2C_INT) || (I2C_DMA) 
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C1_EV_IRQn,0,this);
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C1_ER_IRQn,0,this);
#endif
#if I2C_DMA
                m_DMAx  = DMA::GetInstance(1);       
                m_DMAx->HwInit();
                m_DMAx->RegisterCallback(I2C1_RX_DMA_CHANNEL,&m_I2C1_DMA_Rx_Callback);
                m_DMAx->RegisterCallback(I2C1_TX_DMA_CHANNEL,&m_I2C1_DMA_Tx_Callback);
                InterruptManager::GetInstance()->EnableInterrupt(DMA1_Channel6_IRQn);
                InterruptManager::GetInstance()->EnableInterrupt(DMA1_Channel7_IRQn);
                m_DMAx->XferConfig(&DMAConfig, I2C1_TX_DMA_CHANNEL);
                m_DMAx->XferConfig(&DMAConfig, I2C1_RX_DMA_CHANNEL);  
                m_DMAx->EnableTransferCompleteInterrupt(I2C1_TX_DMA_CHANNEL);
            	m_DMAx->EnableTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
#endif                 
                
            }                
            else if(m_I2Cx == I2C2)
            {
#if (I2C_INT) || (I2C_DMA)                
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C2_EV_IRQn,0,this);
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C2_ER_IRQn,0,this);
#endif
#if I2C_DMA
                m_DMAx->RegisterCallback(I2C2_RX_DMA_CHANNEL,&m_I2C2_DMA_Rx_Callback);
                m_DMAx->RegisterCallback(I2C2_TX_DMA_CHANNEL,&m_I2C2_DMA_Tx_Callback);
				m_DMAx->EnableTransferCompleteInterrupt(I2C2_TX_DMA_CHANNEL);
            	m_DMAx->EnableTransferCompleteInterrupt(I2C2_RX_DMA_CHANNEL);
#endif
            }   
            else
            {
                while(1); // Fatal Error
            }
            
            m_I2CState = READY;
            
            return I2C_OK;            
        }
        void I2c::ClockEnable()
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
        
        void I2c::ClockDisable()
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
        
        I2c::I2CStatus_t I2c::TxPoll(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen)
        {
            if(m_I2CState != READY)
                return I2C_BUSY;
            
            if( (TxLen == 0) || (TxBuf == nullptr) )
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
        
        I2c::I2CStatus_t I2c::RxPoll(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen)
        {
            if(m_I2CState != READY)
                return I2C_BUSY;
            
            if( (RxLen == 0) || (RxBuf == nullptr) )
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
        
        bool I2c::DetectSlave(uint8_t SlaveAddress)
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
        
        void I2c::ScanBus(uint8_t* pFoundDevices, uint8_t size)
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
        


#if (I2C_INT) || (I2C_DMA)

        I2c::I2CStatus_t I2c::Post(Transaction_t* pTransaction, uint32_t Mode)
        {            
            if( (pTransaction == nullptr) || ((pTransaction->TxBuf == nullptr) && (pTransaction->RxBuf == nullptr)) || (pTransaction->pStatus == nullptr)  )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);                
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
#if (I2C_INT)
                    return MasterTxRx_Intr(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen, pTransaction->RxBuf ,
                                           pTransaction->RxLen, pTransaction->RepeatedStart,pTransaction->pStatus,pTransaction->XferDoneCallback);  
#endif
                }
                else
                {
#if (I2C_DMA)
                    return MasterTxRx_DMA(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen, pTransaction->RxBuf ,
                                          pTransaction->RxLen, pTransaction->RepeatedStart,pTransaction->pStatus,pTransaction->XferDoneCallback);
#endif

                }
            }                      
        }



        void I2c::TxnDoneHandler(uint32_t StopFlag)
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
                        *(m_Transaction.pStatus) = I2C_STOP_TIMEOUT;
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
#if I2C_DMA
                            m_I2CState = MASTER_TX_DMA;
                            /* Load DMA Tx transaction*/
							m_DMAx->Load(I2C1_TX_DMA_CHANNEL, (uint32_t)&(m_I2Cx->DR), (uint32_t)m_Transaction.TxBuf, m_Transaction.TxLen, LL_DMA_DIRECTION_MEMORY_TO_PERIPH );
							
                            /* Load DMA Rx transaction*/
							m_DMAx->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)&(m_I2Cx->DR), (uint32_t)m_Transaction.RxBuf, m_Transaction.RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY );
                            
							/* Enable DMA Request */
                            m_I2Cx->CR2 |= I2C_CR2_DMAEN;
#endif
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
#if I2C_DMA
                            m_I2CState = MASTER_RX_DMA; 
                            
                            /* Load DMA Rx transaction*/
							m_DMAx->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)&(m_I2Cx->DR), (uint32_t)m_Transaction.RxBuf, m_Transaction.RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY );
                            
                            /* Enable DMA Request */
                            m_I2Cx->CR2 |= I2C_CR2_DMAEN;
#endif
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
                        *(m_Transaction.pStatus) = I2c::I2C_STOP_TIMEOUT;
                    } 
                    else
                    {
                        Disable_EVT_BUF_ERR_Interrupt();
                        
                        //DataAvailableRXNE(1000000);
                        
                        m_I2CState = READY;
                        
						/* Disable Acknowledge */
                    	m_I2Cx->CR1 &= ~I2C_CR1_ACK;
                        
						/* Disable Last DMA */
                		m_I2Cx->CR2 &= ~I2C_CR2_LAST;
                        
						/* Disable DMA Request */            
                		m_I2Cx->CR2 &= ~I2C_CR2_DMAEN;               
						
						I2C_LOG_STATES(I2C_LOG_TXN_DONE_ALL); 
                        
                    }
                }
            }                         
        }
        
//        void I2c::SetCallback(I2CCallbackType_t I2CCallbackType, I2CCallback_t I2CCallback)
//        {
//            switch(I2CCallbackType)
//            {
//            case I2C_RX_QUEUE_FULL_CALLBACK:        m_RxQueueFullCallback = I2CCallback; break;
//            case I2C_TX_QUEUE_EMPTY_CALLBACK:       m_TxQueueEmptyCallback = I2CCallback; break;
//            case I2C_SLAVE_TX_COMPLETE_CALLBACK:    m_SlaveTxDoneCallback = I2CCallback; break;
//            case I2C_SLAVE_RX_COMPLETE_CALLBACK:    m_SlaveRxDoneCallback = I2CCallback; break;
//            default: break; 
//            }            
//        }
		
#endif //(I2C_INT) || (I2C_DMA)


#if (I2C_INT) //&& (I2C_DMA)
		void I2c::ISR( IRQn_Type event )
        {  
            switch(POSITION_VAL(m_I2Cx->SR1))
            {
            case SB : 
                I2C_LOG_EVENTS(I2C_LOG_SB); 
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
                break;
            case ADDR: 
				I2C_LOG_EVENTS(I2C_LOG_ADDR); 
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
                    /* Clear ADDR flag */
                    LL_I2C_ClearFlag_ADDR(m_I2Cx);
                    
                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_DMA);
                }
                else if((m_I2CState == READY) || (m_I2CState == SLAVE_RX) || (m_I2CState == MASTER_TX_DMA))    
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
                        m_I2CState = SLAVE_RX;               
                    }
                    else
                    {
                        m_I2CState = SLAVE_TX;
                    }
                }
                else
                {
                    while(1);/* Fatal Error*/      
                }
                break;
                
            case BTF : 
				I2C_LOG_EVENTS(I2C_LOG_BTF);
                
                if(m_I2CState == MASTER_RX_REPEATED_START ) 
                {
                    return;
                }
                else if((m_I2CState == MASTER_RX) || (m_I2CState == MASTER_RX_DMA))
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
                else if((m_I2CState == MASTER_TX) || (m_I2CState == MASTER_TX_DMA))
                {
                    if(m_I2CState == MASTER_TX_DMA)
                    {
                        //                if(m_Transaction.RxLen != 0)
                        //				{
                        //                    /* Load DMA Rx transaction*/
                        //	                m_DMAx->Load(I2C1_RX_DMA_CHANNEL, (uint32_t)m_Transaction.RxBuf, (uint32_t)&(m_I2Cx->DR), m_Transaction.RxLen, LL_DMA_DIRECTION_PERIPH_TO_MEMORY );
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
                else if(m_I2CState == SLAVE_RX )
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
                else if(m_I2CState == SLAVE_TX )    
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
                else
                {
                    while(1);/* Fatal Error*/ 
                }
                break;
                
            case ADD10 : 
                I2C_LOG_EVENTS(I2C_LOG_ADD10);
                break;
                
            case STOPF : 
			I2C_LOG_EVENTS(I2C_LOG_STOPF);
            LL_I2C_ClearFlag_STOP(m_I2Cx);  
            
            /* Execute the RxDone Callback */
            if(m_SlaveRxDoneCallback)
                m_SlaveRxDoneCallback->CallbackFunction();
            
            m_I2CState = READY;
            
            I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
            break;
            case RSVD : while(1);
            case RXNE :
				I2C_LOG_EVENTS(I2C_LOG_RXNE);
                if(m_I2CState == MASTER_RX)
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
                    {      
                        // This is for the case when Last byte/data1 is to be read
                        /* Read data from DR */
                        (*m_Transaction.RxBuf++) = m_I2Cx->DR;                
                        m_Transaction.RxLen--;
                        
                        /* Disable EVT, BUF and ERR interrupt */
                        Disable_EVT_BUF_ERR_Interrupt();
                        
                        I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_LAST);
                        TxnDoneHandler(0);                
                    }        
#endif
                }
                else if(m_I2CState == SLAVE_RX )
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
                else
                {
                    while(1);/* Fatal Error*/ 
                }
                break;
                
            case TXE : 
				I2C_LOG_EVENTS(I2C_LOG_TXE);
                if(m_I2CState == MASTER_TX)   
                {
                    if( m_I2CState == MASTER_RX_REPEATED_START)
                        return;
                    
                    if(m_Transaction.TxLen > 0)
                    {
                        m_I2Cx->DR = (*m_Transaction.TxBuf++);
                        m_Transaction.TxLen--;
                        I2C_LOG_STATES(I2C_LOG_TXE_GT_0);
                    } 
                    else
                    {
                        I2C_LOG_STATES(I2C_LOG_TXE_DONE);
                        TxnDoneHandler(I2C_CR1_STOP);
                    }
                }
                else if(m_I2CState == SLAVE_TX )  
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
                else
                {
                    while(1);/* Fatal Error*/  
                }
                break;
                
            case BERR : 
				I2C_LOG_EVENTS(I2C_LOG_BERR);
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_BERR);
                LL_I2C_ClearFlag_BERR(m_I2Cx);
                /* Workaround: Start cannot be generated after a misplaced Stop */
                SoftReset();
                m_I2CStatus = I2C_BUS_ERROR; 
                break;
                
            case ARLO : 
				I2C_LOG_EVENTS(I2C_LOG_ARLO);
                m_I2CStatus = I2C_ARB_LOST;
                LL_I2C_ClearFlag_ARLO(m_I2Cx);
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ARB_LOST);
                break;
                
            case AF :
				I2C_LOG_EVENTS(I2C_LOG_AF);
                if(m_I2CState == MASTER_TX)  
                {
                    m_I2CStatus = I2C_ACK_FAIL;
                    LL_I2C_ClearFlag_AF(m_I2Cx);
                    I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ACK_FAIL);
                }
                else if(m_I2CState == SLAVE_TX ) 
                {
                    LL_I2C_ClearFlag_AF(m_I2Cx);  
                    
                    if(m_SlaveTxDoneCallback)
                        m_SlaveTxDoneCallback->CallbackFunction();
                    
                    m_I2CState = READY;                    
                    I2C_LOG_STATES(I2C_LOG_AF_SLAVE_ACK_FAIL);
                }
                else                                
                {
                    while(1);/* Fatal Error*/   
                }
                break;
                
            case OVR : 
				I2C_LOG_EVENTS(I2C_LOG_OVR);
                m_I2CStatus = I2C_DATA_OVR;
                LL_I2C_ClearFlag_OVR(m_I2Cx);
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_DATA_OVR);
                break;
                
            case PECERR :
				I2C_LOG_EVENTS(I2C_LOG_PECERR);
                while(1); // Not Implemented currently
                break;
                
            default : /*while(1);*/return;
            }
        } 

#elif (I2C_DMA)

		void I2c::ISR( IRQn_Type event )
        {  
            switch(POSITION_VAL(m_I2Cx->SR1))
            {
            case SB : 
                I2C_LOG_EVENTS(I2C_LOG_SB); 
                if((m_I2CState == MASTER_TX_DMA) )
                {
                    m_I2Cx->DR = m_Transaction.SlaveAddress & I2C_DIR_WRITE; 
                    I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
                }
                else if((m_I2CState == MASTER_RX_DMA))  
                {
                    m_I2Cx->DR = m_Transaction.SlaveAddress | I2C_DIR_READ;
                    
                    I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX);
                }
                else if(m_I2CState == MASTER_RX_REPEATED_START)
                {
                    /* Repeated start is handled here, clear the flag*/
                    m_Transaction.RepeatedStart = 0;         
					
                    m_I2Cx->DR = m_Transaction.SlaveAddress | I2C_DIR_READ;  
                    
					m_I2CState = MASTER_RX_DMA;
					I2C_LOG_STATES(I2C_LOG_REPEATED_START_MASTER_RX_DMA);
                }
                else
                {
                    while(1);
                }						
                break;
            case ADDR: 
				I2C_LOG_EVENTS(I2C_LOG_ADDR); 
               
                if( m_I2CState == MASTER_TX_DMA)
                {                 
                    /* Clear ADDR flag */
                    LL_I2C_ClearFlag_ADDR(m_I2Cx);
                    
                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_DMA);
                }
                else if(m_I2CState == MASTER_RX_DMA)
                {                
                    /* Clear ADDR flag */
                    LL_I2C_ClearFlag_ADDR(m_I2Cx);
                    
                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_DMA);
                }
                else if((m_I2CState == READY) || (m_I2CState == SLAVE_RX) || (m_I2CState == MASTER_TX_DMA))    
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
                        m_I2CState = SLAVE_RX;               
                    }
                    else
                    {
                        m_I2CState = SLAVE_TX;
                    }
                }
                else
                {
                    while(1);/* Fatal Error*/      
                }
                break;
                
            case BTF : 
				I2C_LOG_EVENTS(I2C_LOG_BTF);
                
                if(m_I2CState == MASTER_RX_REPEATED_START ) 
                {
                    return;
                }
                else if(m_I2CState == MASTER_RX_DMA) // Not required in DMA mode
                {
					while(1);                
                }
                else if(m_I2CState == MASTER_TX_DMA)
                {
                        I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_DMA_STOP);
                        TxnDoneHandler(I2C_CR1_STOP);                     
                }
                else
                {
                    while(1);/* Fatal Error*/ 
                }
                break;
                
            case ADD10 : 
                I2C_LOG_EVENTS(I2C_LOG_ADD10);
                break;
                
            case STOPF : 
			I2C_LOG_EVENTS(I2C_LOG_STOPF);
            LL_I2C_ClearFlag_STOP(m_I2Cx);  
            
            /* Execute the RxDone Callback */
            if(m_SlaveRxDoneCallback)
                m_SlaveRxDoneCallback->CallbackFunction();
            
            m_I2CState = READY;
            
            I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
            break;
            case RSVD : while(1); /*Fatal Error*/
            case RXNE :
				I2C_LOG_EVENTS(I2C_LOG_RXNE);
                break;                
            case TXE : 
				I2C_LOG_EVENTS(I2C_LOG_TXE);               
                break;                
            case BERR : 
				I2C_LOG_EVENTS(I2C_LOG_BERR);
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_BERR);
                LL_I2C_ClearFlag_BERR(m_I2Cx);
                /* Workaround: Start cannot be generated after a misplaced Stop */
                SoftReset();
                m_I2CStatus = I2C_BUS_ERROR; 
                break;
                
            case ARLO : 
				I2C_LOG_EVENTS(I2C_LOG_ARLO);
                m_I2CStatus = I2C_ARB_LOST;
                LL_I2C_ClearFlag_ARLO(m_I2Cx);
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ARB_LOST);
                break;
                
            case AF :
				I2C_LOG_EVENTS(I2C_LOG_AF);
                if(m_I2CState == MASTER_TX)  
                {
                    m_I2CStatus = I2C_ACK_FAIL;
                    LL_I2C_ClearFlag_AF(m_I2Cx);
                    I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ACK_FAIL);
                }
                else if(m_I2CState == SLAVE_TX ) 
                {
                    LL_I2C_ClearFlag_AF(m_I2Cx);  
                    
                    if(m_SlaveTxDoneCallback)
                        m_SlaveTxDoneCallback->CallbackFunction();
                    
                    m_I2CState = READY;                    
                    I2C_LOG_STATES(I2C_LOG_AF_SLAVE_ACK_FAIL);
                }
                else                                
                {
                    while(1);/* Fatal Error*/   
                }
                break;
                
            case OVR : 
				I2C_LOG_EVENTS(I2C_LOG_OVR);
                m_I2CStatus = I2C_DATA_OVR;
                LL_I2C_ClearFlag_OVR(m_I2Cx);
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_DATA_OVR);
                break;
                
            case PECERR :
				I2C_LOG_EVENTS(I2C_LOG_PECERR);
                while(1); // Not Implemented currently
                break;
                
            default : /*while(1);*/return;
            }
        } 
#else

		void I2c::ISR( IRQn_Type event )
        {  
        	// To avoid the compilation error regarding the Virtaul function table
		}


#endif
        

        
}
