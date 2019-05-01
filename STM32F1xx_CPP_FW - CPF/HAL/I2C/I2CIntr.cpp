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
    static GpioOutput B13(Gpio::B13);
    
    I2CIntr::I2CIntr(Pin_t scl, Pin_t sda, Hz_t Hz) :
        _sclPin(scl,GpioOutput::AF_OPEN_DRAIN),
        _sdaPin(sda,GpioOutput::AF_OPEN_DRAIN),
        _hz(Hz),
        _I2CState(RESET)
        {
            if((scl == Gpio::B6) && (sda == Gpio::B7))
            {
                _I2Cx = I2C1;
            }
            else if((scl == Gpio::B10) && (sda == Gpio::B11))
            {
                _I2Cx = I2C2;
            }
            else if((scl == Gpio::B8) && (sda == Gpio::B9))
            {
                // initialize with nullptr to configure it later in HwInit() with pin remap
                _I2Cx = nullptr;
            }
            else
            {
                while(1); // Fatal Error
            }
            
            _I2CStatus = I2C_OK;
            start = stop = 0;
            I2CStates_Idx = 0;
            B13.HwInit();
        }
        
        I2CIntr::I2CStatus_t I2CIntr::HwInit(void *pInitStruct)
        {
            LL_I2C_InitTypeDef I2C_InitStruct;
            
            /* Set I2C_InitStruct fields to default values */
            I2C_InitStruct.PeripheralMode  = LL_I2C_MODE_I2C;
            I2C_InitStruct.ClockSpeed      = _hz;
            I2C_InitStruct.DutyCycle       = LL_I2C_DUTYCYCLE_2;
            I2C_InitStruct.OwnAddress1     = I2C_OWN_SLAVE_ADDRESS;
            I2C_InitStruct.TypeAcknowledge = LL_I2C_NACK;
            I2C_InitStruct.OwnAddrSize     = LL_I2C_OWNADDRESS1_7BIT;           
            
            _sclPin.HwInit();
            _sdaPin.HwInit();
            
            _sclPin.SetPinSpeed(GpioOutput::GPIO_OUTPUT_SPEED_HIGH);
            _sdaPin.SetPinSpeed(GpioOutput::GPIO_OUTPUT_SPEED_HIGH);
            
            if(_I2Cx == nullptr)
            {
                // Remap B8 and B9 Pin in I2C mode 
            }
            
            ClockEnable();
            
            if(pInitStruct == nullptr)
            {
                LL_I2C_Init(_I2Cx, &I2C_InitStruct);
            }
            else
            {
                LL_I2C_Init(_I2Cx, (LL_I2C_InitTypeDef*)pInitStruct);
            }        
            
            //LL_I2C_SetOwnAddress1(_I2Cx,I2C_SLAVE_ADDRESS,LL_I2C_OWNADDRESS1_7BIT);
            //LL_I2C_SetOwnAddress2(_I2Cx,I2C_SLAVE_ADDRESS-4);
            //LL_I2C_EnableOwnAddress2(_I2Cx);
            //LL_I2C_EnableGeneralCall(_I2Cx);
            
            // Register Interrupt ISR with Interrupt Manager
            if(_I2Cx == I2C1)
            {
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C1_EV_IRQn,0,this);
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C1_ER_IRQn,0,this);
            }                
            else if(_I2Cx == I2C2)
            {
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C2_EV_IRQn,0,this);
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C2_ER_IRQn,0,this);
            }   
            else
            {
                while(1); // Fatal Error
            }
            
            _I2CState = READY;
            
            return I2C_OK;
            
        }
        void I2CIntr::ClockEnable()
        {
            if(_I2Cx == I2C1)
            {
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
            }
            else if(_I2Cx == I2C2)
            {
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
            }        
        }
        
        void I2CIntr::ClockDisable()
        {
            if(_I2Cx == I2C1)
            {
                LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1);
            }
            else if(_I2Cx == I2C2)
            {
                LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C2);
            }        
        }
        I2CIntr::I2CStatus_t I2CIntr:: MasterTx(uint16_t SlaveAddress,uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered)
        { 
            if(_I2CState != READY)
                return I2C_BUSY;
            
            if( (len == 0) || (pdata == nullptr) )
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
            
            _Transaction.TxBuf = pdata;
            _Transaction.TxSize = len;
            _Transaction.SlaveAddress = SlaveAddress;
            _I2CState = MASTER_TX;
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_ENABLE_ALL);
            
            /* Generate Start */
            Start();
            I2C_LOG_STATES(I2C_LOG_START_MASTER_TX);
            
            return I2C_OK;
        }
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterRx(uint16_t SlaveAddress,uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered )
        {            
            if(_I2CState != READY)
                return I2C_BUSY;
            
            if( (len == 0) || (pdata == nullptr) )
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
            
            _Transaction.RxBuf = pdata;
            _Transaction.RxSize = len;
            _Transaction.SlaveAddress = SlaveAddress;
            _I2CState = MASTER_RX;   
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_ENABLE_ALL);
            
            /* Generate Start */
            Start();
            
            I2C_LOG_STATES(I2C_LOG_START_MASTER_RX);
            
            I2C_DEBUG_LOG(I2C_OK);
            return I2C_OK;
        }
        I2CIntr::I2CStatus_t I2CIntr::MasterTxRx(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen, bool RepeatedStart)
        {
            if(_I2CState != READY)
                return I2C_BUSY;
            
            if( (TxBuf == nullptr) || (RxBuf == nullptr) )
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
            
            _Transaction.SlaveAddress = SlaveAddress;
            _Transaction.TxBuf          = TxBuf;
            _Transaction.TxSize         = TxLen;
            _Transaction.RxBuf          = RxBuf;
            _Transaction.RxSize         = RxLen;  
            _Transaction.RepeatedStart  = RepeatedStart;            
            _I2CState = MASTER_TX;
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_ENABLE_ALL);
            
            /* Generate Start */
            Start();
            
            I2C_LOG_STATES(I2C_LOG_START_MASTER_TXRX);
            
            I2C_DEBUG_LOG(I2C_OK);
            
            return I2C_OK;
            
        }
        I2CIntr::I2CStatus_t I2CIntr::SlaveRx(uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered )
        {
            uint32_t XferSize, Received_data = 0;
            
            if( (len == 0) || (pdata == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                return I2C_INVALID_PARAMS;                
            }
            
            XferSize = len;
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {          
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            /* Enable Address Acknowledge */
            _I2Cx->CR1 |= I2C_CR1_ACK;
            
            /* Wait until ADDR flag is set */
            if(SlaveAddressSent(I2C_TIMEOUT) == false)
            {          
                I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
                return I2C_ADDR_TIMEOUT;                
            }          
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(_I2Cx);
            
            while(XferSize>0)
            {
                /* Wait until RXNE flag is set */
                if(DataAvailableRXNE(I2C_TIMEOUT) == true)      
                {
                    /* Disable Address Acknowledge */
                    _I2Cx->CR1 &= ~I2C_CR1_ACK;
                    I2C_DEBUG_LOG(I2C_RXNE_TIMEOUT);
                    return I2C_RXNE_TIMEOUT;
                }
                
                /* Read data from DR */
                *pdata++ = _I2Cx->DR;
                XferSize--;
                Received_data++;
                
                if( (bool)LL_I2C_IsActiveFlag_BTF(_I2Cx) == true)
                {
                    /* Write data to DR */
                    *pdata++ = _I2Cx->DR;            
                    XferSize--; 
                    Received_data++;                     
                }
            }
            
            /* Wait until STOP flag is set */
            if(StopCondition(I2C_TIMEOUT) == false)
            {
                /* Disable Address Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                if( (bool)LL_I2C_IsActiveFlag_AF(_I2Cx) == true )
                {              
                    I2C_DEBUG_LOG(I2C_ACK_FAIL);
                    return I2C_ACK_FAIL;
                }
                I2C_DEBUG_LOG(I2C_STOP_TIMEOUT);
                return I2C_STOP_TIMEOUT;
            }
            
            /* Clear STOP flag */
            LL_I2C_ClearFlag_STOP(_I2Cx);
            
            /* Disable Address Acknowledge */
            _I2Cx->CR1 &= ~I2C_CR1_ACK;
            
            I2C_DEBUG_LOG(I2C_OK);
            return I2C_OK;
        }
        
        I2CIntr::I2CStatus_t I2CIntr::SlaveTx(uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered )
        {
            uint32_t XferSize, Received_data = 0;
            
            if( (len == 0) || (pdata == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                return I2C_INVALID_PARAMS;                
            }
            
            XferSize = len;
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {          
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            /* Enable Address Acknowledge */
            _I2Cx->CR1 |= I2C_CR1_ACK;
            
            /* Wait until ADDR flag is set */
            if(SlaveAddressSent(I2C_TIMEOUT) == false)
            {          
                I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
                return I2C_ADDR_TIMEOUT;                
            }          
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(_I2Cx);
            
            while(XferSize>0)
            {
                /* Wait until TXE flag is set */
                if(TransmitterEmpty(I2C_TIMEOUT) == true)      
                {
                    /* Disable Address Acknowledge */
                    _I2Cx->CR1 &= ~I2C_CR1_ACK;
                    I2C_DEBUG_LOG(I2C_RXNE_TIMEOUT);
                    return I2C_RXNE_TIMEOUT;
                }
                
                /* Write data to DR */
                _I2Cx->DR = *pdata++;
                XferSize--;
                Received_data++;
                
                if( (bool)LL_I2C_IsActiveFlag_BTF(_I2Cx) == true)
                {
                    /* Write data to DR */
                    _I2Cx->DR = *pdata++;            
                    XferSize--; 
                    Received_data++;                     
                }
            }
            
            /* Wait until AF flag is set */
            if(ACKFail(I2C_TIMEOUT) == false)
            {
                /* Disable Address Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                if( (bool)LL_I2C_IsActiveFlag_AF(_I2Cx) == true )
                {              
                    I2C_DEBUG_LOG(I2C_ACK_FAIL);
                    return I2C_ACK_FAIL;
                }
                I2C_DEBUG_LOG(I2C_STOP_TIMEOUT);
                return I2C_STOP_TIMEOUT;
            }
            
            /* Clear AF flag */
            LL_I2C_ClearFlag_AF(_I2Cx);
            
            /* Disable Address Acknowledge */
            _I2Cx->CR1 &= ~I2C_CR1_ACK;
            
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
            Start();
            
            /* Check if Start condition generated successfully*/
            if(StartConditionGenerated(I2C_TIMEOUT) == false)
            { 
                I2C_DEBUG_LOG(I2C_START_TIMEOUT);
                return I2C_START_TIMEOUT;                
            }
            
            /* Send slave address */
            WriteDataRegister(SlaveAddress);
            
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
                    LL_I2C_ClearFlag_AF(_I2Cx);
                    LL_I2C_GenerateStopCondition(_I2Cx);
                }
            }
            
        }
        I2CIntr::I2CStatus_t I2CIntr::AddressMatch()
        {
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            /* Enable Address Acknowledge */
            _I2Cx->CR1 |= I2C_CR1_ACK;
            
            /* Wait until ADDR flag is set */
            if(SlaveAddressSent(I2C_TIMEOUT) == false)
            {          
                I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
                return I2C_ADDR_TIMEOUT;                
            } 
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(_I2Cx);
            
            /* Wait until STOP flag is set */
            if(StopCondition(I2C_TIMEOUT) == false)
            {
                /* Disable Address Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                if( (bool)LL_I2C_IsActiveFlag_AF(_I2Cx) == true )
                {              
                    I2C_DEBUG_LOG(I2C_ACK_FAIL);
                    return I2C_ACK_FAIL;
                }
                I2C_DEBUG_LOG(I2C_STOP_TIMEOUT);
                return I2C_STOP_TIMEOUT;
            }
            
            /* Clear STOP flag */
            LL_I2C_ClearFlag_STOP(_I2Cx);
            
            return I2C_OK;
        }
        
        void I2CIntr::InteruptControl(I2CInterrupt_t I2CInterrupt)
        {
            switch(I2CInterrupt)
            {
            case I2C_EVENT_INTERRUPT_ENABLE:            LL_I2C_EnableIT_EVT(_I2Cx);
            break;
            case I2C_EVENT_INTERRUPT_DISABLE:           LL_I2C_DisableIT_EVT(_I2Cx);    
            break;
            case I2C_EVENT_INTERRUPT_BUFFER_ENABLE:     LL_I2C_EnableIT_BUF(_I2Cx);     
            break;
            case I2C_EVENT_INTERRUPT_BUFFER_DISABLE:    LL_I2C_DisableIT_BUF(_I2Cx);    
            break;
            case I2C_ERROR_INTERRUPT_ENABLE:            LL_I2C_EnableIT_ERR(_I2Cx);     
            break;
            case I2C_ERROR_INTERRUPT_DISABLE:           LL_I2C_DisableIT_ERR(_I2Cx);    
            break;
            case I2C_INTERRUPT_ENABLE_ALL:              LL_I2C_EnableIT_EVT(_I2Cx); 
            LL_I2C_EnableIT_BUF(_I2Cx);
            LL_I2C_EnableIT_ERR(_I2Cx);     
            break;
            case I2C_INTERRUPT_DISABLE_ALL:             LL_I2C_DisableIT_EVT(_I2Cx); 
            LL_I2C_DisableIT_BUF(_I2Cx);
            LL_I2C_DisableIT_ERR(_I2Cx);     
            break;          
            default:;
            }
        }
        
        void I2CIntr::ISR( IRQn_Type event )
        {                
            while(_I2Cx->SR1)
            {
                switch(POSITION_VAL(_I2Cx->SR1))
                {
                case I2C_SR1_SB_Pos : Master_SB_Handler(); break;
                case I2C_SR1_ADDR_Pos : Master_ADDR_Handler(); break;
                case I2C_SR1_BTF_Pos :  
                    if(_I2CState == MASTER_RX)
                        MasterReceive_BTF_Handler();
                    else 
                        MasterTransmit_BTF_Handler();
                    break;
                case I2C_SR1_ADD10_Pos : break;
                case I2C_SR1_STOPF_Pos : Slave_STOP_Handler(); break;
                case I2C_SR1_RXNE_Pos : Master_RxNE_Handler(); break;
                case I2C_SR1_TXE_Pos : Master_TxE_Handler(); break;
                case I2C_SR1_BERR_Pos : Master_BERR_Handler(); break;
                case I2C_SR1_ARLO_Pos : Master_AL_Handler(); break;
                case I2C_SR1_AF_Pos : Master_AF_Handler(); break;
                case I2C_SR1_OVR_Pos : Master_OVR_Handler(); break;
                case I2C_SR1_PECERR_Pos : break;
                default : return;
               }
            }
        }
        
        void I2CIntr::Slave_STOP_Handler()
        {
            LL_I2C_ClearFlag_STOP(_I2Cx);
            _I2CState = READY;
            I2C_LOG_STATES(I2C_LOG_STOP_INTR);
        }
        void I2CIntr::Master_ADDR_Handler()
        {
            if(_I2CState == MASTER_RX)
            {
                if(_Transaction.RxSize == 0U)
                {
                    /* Clear ADDR flag */
                    LL_I2C_ClearFlag_ADDR(_I2Cx);
                    
                    /* Generate Stop */
                    Stop();
                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_0);
                }
                else if(_Transaction.RxSize == 1U)   
                {
                    /* Disable Acknowledge */
                    _I2Cx->CR1 &= ~I2C_CR1_ACK;
                    
                    if((_I2Cx->CR2 & I2C_CR2_DMAEN) == I2C_CR2_DMAEN)
                    {
                        /* Disable Acknowledge ?? */ 
                        _I2Cx->CR1 &= ~I2C_CR1_ACK;
                        
                        /* Clear ADDR flag */
                        LL_I2C_ClearFlag_ADDR(_I2Cx);
                        
                        I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1_DMA);
                    }
                    else
                    {
                        /* Clear ADDR flag */
                        LL_I2C_ClearFlag_ADDR(_I2Cx);
                        
                        /* Disable EVT, BUF and ERR interrupt */
                        InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_DISABLE_ALL);                
                        
                        _I2CState = READY;
                        
                        /* Generate Stop */
                        Stop(); 
                        
                        I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_STOP);
                    }                    
                }
                else if(_Transaction.RxSize == 2U)   
                {
                    /* Enable Acknowledge */
                    _I2Cx->CR1 |= I2C_CR1_ACK;
                    
                    /* Clear ADDR flag */
                    LL_I2C_ClearFlag_ADDR(_I2Cx);
                    
                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2);
                    
                    if((_I2Cx->CR2 & I2C_CR2_DMAEN) == I2C_CR2_DMAEN)
                    {
                        /* Enable Last DMA bit */
                        _I2Cx->CR2 |= I2C_CR2_LAST;
                        
                        I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2_DMA);
                    }
                }                
                else
                {
                    /* Enable Acknowledge */
                    _I2Cx->CR1 |= I2C_CR1_ACK;
                    
                    if((_I2Cx->CR2 & I2C_CR2_DMAEN) == I2C_CR2_DMAEN)
                    {
                        /* Enable Last DMA bit */
                        _I2Cx->CR2 |= I2C_CR2_LAST;
                    }
                    
                    /* Clear ADDR flag */
                    LL_I2C_ClearFlag_ADDR(_I2Cx);
                    
                   I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_OTHER);
                }  
            }
            else if(_I2CState == MASTER_TX)
            {                
                /* Clear ADDR flag */
                LL_I2C_ClearFlag_ADDR(_I2Cx);
                
                if(_Transaction.TxSize > 0)
                {
                    _I2Cx->DR = (*_Transaction.TxBuf++);
                    _Transaction.TxSize--;
                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0);
                }
                else
                {
                    if(_Transaction.RepeatedStart)
                    {
                        InteruptControl(HAL::I2CIntr::I2C_EVENT_INTERRUPT_BUFFER_DISABLE);
                        
                        _I2CState = MASTER_RX;
                        
                         /* Enable Acknowledge */
                        _I2Cx->CR1 |= I2C_CR1_ACK;
        
                        /* Disable Pos */
                        //_I2Cx->CR1 &= ~I2C_CR1_POS;
                    
                        /* Generate Repeated Start */
                        Start();   
                        
                        I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_REPEATED_START);
                    }
                    else
                    {
		                 /* Disable EVT, BUF and ERR interrupt */
		                InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_DISABLE_ALL);
					 
                        _I2CState = READY;                    
                        /* Generate Stop */
                        Stop();  
                        I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_STOP);
                    }              
				}
            }
            else
            {
                
            }
        }
        void I2CIntr::Master_TxE_Handler()
        {
            if( _I2CState == MASTER_RX_REPEATED_START)
                return;
            
            if(_Transaction.TxSize > 0)
            {
                _I2Cx->DR = (*_Transaction.TxBuf++);
                _Transaction.TxSize--;
                //I2C_LOG_STATES(I2C_LOG_TXE);
            }   
           // I2C_LOG_STATES(I2C_LOG_TXE_DONE);
        }
        
        void I2CIntr::MasterTransmit_BTF_Handler()
        {
            if(_Transaction.TxSize > 0)
            {
                _I2Cx->DR = (*_Transaction.TxBuf++);
                _Transaction.TxSize--;
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_GT_0);
            }
            else
            {             
               if(_Transaction.RepeatedStart)
                {
                    if( _I2CState == MASTER_RX_REPEATED_START)
                    return;
                
                    InteruptControl(HAL::I2CIntr::I2C_EVENT_INTERRUPT_BUFFER_DISABLE);
                    
                    /* Enable Acknowledge */
                    _I2Cx->CR1 |= I2C_CR1_ACK;
                        
                    _I2CState = MASTER_RX_REPEATED_START;
                    
                    /* Disable Pos */
                    _I2Cx->CR1 &= ~I2C_CR1_POS;
    
                    /* Generate Start */
                    Start(); 
                    I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_REPEATED_START);
                }
                else
                {
                    /* Disable EVT, BUF and ERR interrupt */
                    InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_DISABLE_ALL);
                    
                    _I2CState = READY;
                    
                    /* Generate Stop */
                    Stop();  
                    I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_STOP);
                }
            }           
        }
        
        void I2CIntr::MasterReceive_BTF_Handler()
        {
            if( _I2CState == MASTER_RX_REPEATED_START)
                return;
            
            if(_Transaction.RxSize == 3U)
            {
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR ;
                _Transaction.RxSize--;   
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_3);
            }
            else if(_Transaction.RxSize == 2U)
            {
                /* Disable EVT, BUF and ERR interrupt */
                InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_DISABLE_ALL);
                
                /* Ready for next data transfer */
                _I2CState = READY;
                
                /* Generate Stop */
                _I2Cx->CR1 |= I2C_CR1_STOP;
                
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR;
                _Transaction.RxSize--;
                
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR;
                _Transaction.RxSize--;    
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_2);
            }
            else
            {
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR;
                _Transaction.RxSize--;
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_GT_3);
            }          
        }        
        void I2CIntr::Master_RxNE_Handler()
        {
            if(_Transaction.RxSize > 3U)
            {
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR;
                _Transaction.RxSize--;
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RXNE_SIZE_GT_3);
            }
            else if((_Transaction.RxSize == 2U) || (_Transaction.RxSize == 3U))
            {  
                /* Disable Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                /* Enable Pos */
                _I2Cx->CR1 |= I2C_CR1_POS;
                
                 I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RXNE_SIZE_2_OR_3);
            }
            else /* Last byte */ 
            {
                /* Disable EVT, BUF and ERR interrupt */
                InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_DISABLE_ALL);
                
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR;
                _Transaction.RxSize--;
                
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RXNE_LAST);
                /* Stop condition will be sent in "MasterReceive_BTF_Handler"*/
            }
        }
        void I2CIntr::Master_BERR_Handler()
        {
            I2C_LOG_STATES(I2C_LOG_BTF_MASTER_BERR);
            LL_I2C_ClearFlag_BERR(_I2Cx);
            /* Workaround: Start cannot be generated after a misplaced Stop */
            SoftReset();
            _I2CStatus = I2C_BUS_ERROR;
        }
        
        void I2CIntr::Master_AF_Handler()
        {
            _I2CStatus = I2C_ACK_FAIL;
            LL_I2C_ClearFlag_AF(_I2Cx);
            I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ACK_FAIL);
        }
        
        void I2CIntr::Master_AL_Handler()
        {
            _I2CStatus = I2C_ARB_LOST;
            LL_I2C_ClearFlag_ARLO(_I2Cx);
            I2C_LOG_STATES(I2C_LOG_BTF_MASTER_ARB_LOST);
        }
        
        void I2CIntr::Master_OVR_Handler()
        {
            _I2CStatus = I2C_DATA_OVR;
            LL_I2C_ClearFlag_OVR(_I2Cx);
            I2C_LOG_STATES(I2C_LOG_BTF_MASTER_DATA_OVR);
        }
        
        void I2CIntr::Master_SB_Handler()
        {
            if(_I2CState == MASTER_TX) 
            {
                _I2Cx->DR = _Transaction.SlaveAddress & I2C_DIR_WRITE; 
                I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
            }
            else if(_I2CState == MASTER_RX) 
            {
                _I2Cx->DR = _Transaction.SlaveAddress | I2C_DIR_READ;
                I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX);
            }
            else
            {
                /* Repeated start is handled here, clear the flag*/
                _Transaction.RepeatedStart = false;
                InteruptControl(HAL::I2CIntr::I2C_EVENT_INTERRUPT_BUFFER_ENABLE);
                _I2Cx->DR = _Transaction.SlaveAddress | I2C_DIR_READ;                
                _I2CState = MASTER_RX;
                I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX_REPEATED_START);
            }
        }
}
