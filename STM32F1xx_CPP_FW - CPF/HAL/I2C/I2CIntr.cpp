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
             B13.HwInit();
        }
        
        I2CIntr::I2CStatus_t I2CIntr::HwInit(void *pInitStruct)
        {
            LL_I2C_InitTypeDef I2C_InitStruct;
            
            /* Set I2C_InitStruct fields to default values */
            I2C_InitStruct.PeripheralMode  = LL_I2C_MODE_I2C;
            I2C_InitStruct.ClockSpeed      = _hz;
            I2C_InitStruct.DutyCycle       = LL_I2C_DUTYCYCLE_2;
            I2C_InitStruct.OwnAddress1     = I2C_SLAVE_ADDRESS;
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
            
                       
                        
            if( (len == 0) || (pdata == nullptr) )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                return I2C_INVALID_PARAMS;                
            }
            
           if(_I2CState != READY)
                return I2C_BUSY;
            
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
            
            return I2C_OK;
        }
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterRx(uint16_t SlaveAddress,uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered )
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
            //LL_I2C_DisableBitPOS(_I2Cx);
            
            /* Generate Start */
            Start();
            
            /* Check if Start condition generated successfully*/
            if(StartConditionGenerated(I2C_TIMEOUT) == false)
            {          
                I2C_DEBUG_LOG(I2C_START_TIMEOUT);
                return I2C_START_TIMEOUT;                
            }
            
            /* Send slave address */
            WriteDataRegister(SlaveAddress | I2C_DIR_READ);
            
            /* Wait until ADDR flag is set */
            if(SlaveAddressSent(I2C_TIMEOUT) == false)
            {          
                I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
                return I2C_ADDR_TIMEOUT;                
            }          
            
            if(XferSize == 1)
            {
                /* Disable Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                //LL_I2C_AcknowledgeNextData(_I2Cx,LL_I2C_NACK);
                
                /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                software sequence must complete before the current byte end of transfer */
                __disable_irq();
                
                /* Clear ADDR flag */
                LL_I2C_ClearFlag_ADDR(_I2Cx);
                
                /* Generate Stop */
                _I2Cx->CR1 |= I2C_CR1_STOP;
                
                /* Re-enable IRQs */
                __enable_irq();             
            }
            else if(XferSize == 2)
            {
                /* Enable Pos */
                _I2Cx->CR1 |= I2C_CR1_POS;
                
                /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                software sequence must complete before the current byte end of transfer */
                __disable_irq();
                
                /* Clear ADDR flag */
                LL_I2C_ClearFlag_ADDR(_I2Cx);
                
                /* Disable Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                /* Re-enable IRQs */
                __enable_irq(); 
            }
            else
            {
                /* Enable Acknowledge */
                _I2Cx->CR1 |= I2C_CR1_ACK;
                
                /* Clear ADDR flag */
                LL_I2C_ClearFlag_ADDR(_I2Cx);
            }
            
            while(XferSize > 0U)
            {
                if(XferSize <= 3U)
                {
                    /* One byte */
                    if(XferSize == 1U)
                    {
                        /* Wait until RXNE flag is set */
                        if(DataAvailableRXNE(I2C_TIMEOUT) == true)      
                        {
                            I2C_DEBUG_LOG(I2C_RXNE_TIMEOUT);
                            return I2C_RXNE_TIMEOUT;
                        }
                        
                        /* Read data from DR */
                        (*pdata++) = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                    }
                    /* Two bytes */
                    else if(XferSize == 2U)
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
                        _I2Cx->CR1 |= I2C_CR1_STOP;
                        
                        /* Read data from DR */
                        (*pdata++)= _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                        
                        /* Re-enable IRQs */
                        __enable_irq();
                        
                        /* Read data from DR */
                        (*pdata++) = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
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
                        _I2Cx->CR1 &= ~I2C_CR1_ACK;
                        
                        /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                        software sequence must complete before the current byte end of transfer */
                        __disable_irq();
                        
                        /* Read data from DR */
                        (*pdata++) = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                        
                        /* Wait until BTF flag is set */
                        if(TransferDone(I2C_TIMEOUT) == false)
                        {         
                            I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
                            return I2C_BTF_TIMEOUT;                
                        }
                        
                        /* Generate Stop */
                        _I2Cx->CR1 |= I2C_CR1_STOP;
                        
                        /* Read data from DR */
                        (*pdata++) = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                        
                        /* Re-enable IRQs */
                        __enable_irq(); 
                        
                        /* Read data from DR */
                        (*pdata++) = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
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
                    (*pdata++) = _I2Cx->DR;
                    XferSize--;
                    Received_data++;
                    
                    if((bool)LL_I2C_IsActiveFlag_BTF(_I2Cx) == true)
                    {
                        /* Read data from DR */
                        (*pdata++) = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                    }
                }
            }       
            
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
                case I2C_SR1_BTF_Pos : MasterTransmit_BTF_Handler(); break;
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
        }
        void I2CIntr::Master_ADDR_Handler()
        {
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(_I2Cx);

			if(_Transaction.TxSize > 0)
            {
                _I2Cx->DR = (*_Transaction.TxBuf++);
                _Transaction.TxSize--;
            }
            else
            {
               /* Generate Stop */
                Stop();
                _I2CState = READY;
            }
        }
        void I2CIntr::Master_TxE_Handler()
        {
           if(_Transaction.TxSize > 0)
            {
                _I2Cx->DR = (*_Transaction.TxBuf++);
                _Transaction.TxSize--;
            }         
        }
        
        void I2CIntr::MasterTransmit_BTF_Handler()
        {
           if(_Transaction.TxSize > 0)
            {
                _I2Cx->DR = (*_Transaction.TxBuf++);
                _Transaction.TxSize--;
            }
            else
            {
                InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_DISABLE_ALL);
               /* Generate Stop */
                Stop(); 
                _I2CState = READY;
            }           
        }
        
        void I2CIntr::MasterReceive_BTF_Handler()
        {
          
        }
        
        void I2CIntr::Master_BERR_Handler()
        {
            LL_I2C_ClearFlag_BERR(_I2Cx);
            /* Workaround: Start cannot be generated after a misplaced Stop */
            SoftReset();
            _I2CStatus = I2C_BUS_ERROR;
        }
        
        void I2CIntr::Master_AF_Handler()
        {
            _I2CStatus = I2C_ACK_FAIL;
            LL_I2C_ClearFlag_AF(_I2Cx);
        }
        
        void I2CIntr::Master_AL_Handler()
        {
            _I2CStatus = I2C_ARB_LOST;
            LL_I2C_ClearFlag_ARLO(_I2Cx);
        }
        
        void I2CIntr::Master_OVR_Handler()
        {
            _I2CStatus = I2C_DATA_OVR;
            LL_I2C_ClearFlag_OVR(_I2Cx);
        }
        
       
        void I2CIntr::Master_RxNE_Handler()
        {
            uint16_t Xfercount;
            
            Xfercount = _Transaction.RxSize;
            
            if(Xfercount > 3U)
            {
                /* Read data from DR */
                (*_Transaction.TxBuf++) = _I2Cx->DR;
                _Transaction.RxSize--;
            }
            else if((Xfercount == 2U) || (Xfercount == 3U))
            {  
                /* Disable Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                /* Enable Pos */
                _I2Cx->CR1 |= I2C_CR1_POS;
            }
            else
            {
                
            }
        }
        
        void I2CIntr::Master_SB_Handler()
        {
            if(_I2CState == MASTER_TX) 
            {
                _I2Cx->DR = _Transaction.SlaveAddress & I2C_DIR_WRITE;  
            }
            else
            {
                _I2Cx->DR = _Transaction.SlaveAddress | I2C_DIR_READ;
            }                     
        }
        
        //        void I2CIntr::ISR( IRQn_Type event )
//        {
//            I2Cx_t _I2Cx = _I2Cx;
//            
//          //  while(_I2Cx->SR1)
//          //  {
//                if(event == I2C1_EV_IRQn )   
//                {
//                    if((bool)LL_I2C_IsActiveFlag_RXNE(_I2Cx) == true)
//                    {
//                        Master_RxNE_Handler();
//                    }
//                     if((bool)LL_I2C_IsActiveFlag_ADDR(_I2Cx) == true)
//                    {
//                        Master_ADDR_Handler();
//                    }
//                     if((bool)LL_I2C_IsActiveFlag_TXE(_I2Cx) == true)
//                    {
//                        Master_TxE_Handler();
//                    }
//                     if((bool)LL_I2C_IsActiveFlag_BTF(_I2Cx) == true)
//                    {
//                        MasterTransmit_BTF_Handler();
//                    }
//                     if((bool)LL_I2C_IsActiveFlag_SB(_I2Cx) == true)
//                    {
//                        Master_SB_Handler();
//                    }
//                     if((bool)LL_I2C_IsActiveFlag_STOP(_I2Cx) == true)
//                    {
//                        Master_STOP_Handler();
//                    }
//                     if((bool)LL_I2C_IsActiveFlag_ARLO(_I2Cx) == true)
//                    {
//                        
//                    }
//                     if((bool)LL_I2C_IsActiveFlag_ADD10(_I2Cx) == true)
//                    {
//                        
//                    }
//                }
//                else if(event == I2C1_ER_IRQn )
//                {
//                    if((bool)LL_I2C_IsActiveFlag_BERR(_I2Cx) == true)
//                    {
//                        LL_I2C_ClearFlag_BERR(_I2Cx);
//                        /* Workaround: Start cannot be generated after a misplaced Stop */
//                        SoftReset();
//                        _I2CStatus = I2C_BUS_ERROR;
//                    }
//                    else  if((bool)LL_I2C_IsActiveFlag_AF(_I2Cx) == true)
//                    {                    
//                        _I2CStatus = I2C_ACK_FAIL;
//                        LL_I2C_ClearFlag_AF(_I2Cx);
//                    }
//                    else  if((bool)LL_I2C_IsActiveFlag_ARLO(_I2Cx) == true)
//                    {
//                        _I2CStatus = I2C_ARB_LOST;
//                        LL_I2C_ClearFlag_ARLO(_I2Cx);
//                    }
//                    else  if((bool)LL_I2C_IsActiveFlag_OVR(_I2Cx) == true)
//                    {
//                        _I2CStatus = I2C_DATA_OVR;
//                        LL_I2C_ClearFlag_OVR(_I2Cx);
//                    }
//                    
//                }            
//            //}
//            
//        }
       // POSITION_VAL(VAL)
}