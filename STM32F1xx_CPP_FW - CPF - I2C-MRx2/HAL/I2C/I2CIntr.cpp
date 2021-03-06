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
            
            _RxQueueFullCallback = nullptr;
            _TxQueueEmptyCallback = nullptr;
            _SlaveRxDoneCallback = nullptr;
            _SlaveTxDoneCallback = nullptr;
            
            _I2CStatus = I2C_OK;
            
#ifdef I2C_INTR_DEBUG
            I2CStates_Idx = 0;
#endif
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
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C1_EV_IRQn,5,this);
                InterruptManager::GetInstance()->RegisterDeviceInterrupt(I2C1_ER_IRQn,4,this);
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
        I2CIntr::I2CStatus_t I2CIntr:: MasterTx(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        { 
            if(_I2CState != READY)
                return I2C_BUSY;
            
            if( (TxLen == 0) || (TxBuf == nullptr) || (pStatus == nullptr) )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                _I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }          
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true )
            {
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;
            }
            
            _Transaction.SlaveAddress       = SlaveAddress;
            _Transaction.TxBuf              = TxBuf;
            _Transaction.TxLen              = TxLen;
            _Transaction.RxBuf              = nullptr;
            _Transaction.RxLen              = 0;  
            _Transaction.RepeatedStart      = 0;  
            _Transaction.pStatus            = pStatus; 
            _Transaction.XferDoneCallback   = XferDoneCallback;
            
            _I2CState = MASTER_TX;
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_ENABLE_ALL);
            
            /* Generate Start */
            Start();
            I2C_LOG_STATES(I2C_LOG_START_MASTER_TX);
            
            return I2C_OK;
        }
        
        void I2CIntr::Stop()
        {            
            //LL_I2C_GenerateStopCondition(_I2Cx);
            /* Generate Stop */
            //_I2Cx->CR1 |= I2C_CR1_STOP;
            
            if( (_Transaction.RxLen == 0) && (_Transaction.TxLen == 0) )
            {
                (*_Transaction.pStatus) = I2C_XFER_DONE;
                _I2CState = READY;
                
                // Transaction ended here, call the completion callback
                if(_Transaction.XferDoneCallback)
                    _Transaction.XferDoneCallback->CallbackFunction();
                
                // Check if there is some transaction pending
                if(_I2CTxnQueue.Available())
                {
                    _I2CTxnQueue.Read(_pCurrentTxn);

                    if(_pCurrentTxn)
                    {
                        _Transaction.SlaveAddress       = _pCurrentTxn->SlaveAddress;
                        _Transaction.TxBuf              = _pCurrentTxn->TxBuf;
                        _Transaction.TxLen              = _pCurrentTxn->TxLen;
                        _Transaction.RxBuf              = _pCurrentTxn->RxBuf;
                        _Transaction.RxLen              = _pCurrentTxn->RxLen;  
                        _Transaction.RepeatedStart      = _pCurrentTxn->RepeatedStart; 
                        _Transaction.pStatus            = _pCurrentTxn->pStatus; 
                        _Transaction.XferDoneCallback   = _pCurrentTxn->XferDoneCallback;
                    }
                    
                    if(_Transaction.TxLen)
                        _I2CState = MASTER_TX;
                    else
                        _I2CState = MASTER_RX;
                    
                    /* Disable Pos */
                    _I2Cx->CR1 &= ~I2C_CR1_POS;
                    
                    InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_ENABLE_ALL);                    
                    
                    if (StopFlagCleared(I2C_TIMEOUT) == true)
                    {
                        _I2CStatus = I2C_STOP_TIMEOUT;
                        (*_Transaction.pStatus) = I2C_STOP_TIMEOUT;
                        while(1);
                    } 
                    
                    /* Generate Start */
                    Start();
                    
                    I2C_LOG_STATES(I2C_LOG_TXN_DEQUEUED); 
                 
                   // MasterTxRx(_pCurrentTxn);
                }
                else
                {
                    I2C_LOG_STATES(I2C_LOG_TXN_QUEUE_EMPTY);                     
                    /*Disable EVT, BUF and ERR interrupt */
                    InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_DISABLE_ALL);
                }                
                
            }
            
            /* Wait with timeout Until STOP flag is set by HW */
            if (StopFlagCleared(I2C_TIMEOUT) == true)
            {
                _I2CStatus = I2C_STOP_TIMEOUT;
                (*_Transaction.pStatus) = I2C_STOP_TIMEOUT;
                while(1);
            }                 
        }
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterTx(Transaction_t* pTransaction)
        { 
            if( pTransaction == nullptr )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                _I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }
            
            return MasterTx(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen,pTransaction->pStatus, pTransaction->XferDoneCallback);
        }
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterRx(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        {            
            if(_I2CState != READY)
                return I2C_BUSY;
            
            if( (RxLen == 0) || (RxBuf == nullptr) || (pStatus == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                _I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }
            
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {          
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            _Transaction.SlaveAddress = SlaveAddress;
            _Transaction.TxBuf              = nullptr;
            _Transaction.TxLen              = 0;
            _Transaction.RxBuf              = RxBuf;
            _Transaction.RxLen              = RxLen;  
            _Transaction.RepeatedStart      = 0;  
            _Transaction.pStatus            = pStatus; 
            _Transaction.XferDoneCallback   = XferDoneCallback;
            
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
        
        I2CIntr::I2CStatus_t I2CIntr:: MasterRx(Transaction_t* pTransaction)
        { 
            if( pTransaction == nullptr )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                _I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }
            
            return MasterRx(pTransaction->SlaveAddress, pTransaction->RxBuf ,pTransaction->RxLen,pTransaction->pStatus, pTransaction->XferDoneCallback);
        }
        
        I2CIntr::I2CStatus_t I2CIntr::MasterTxRx(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen, uint8_t RepeatedStart,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback)
        {
            if(_I2CState != READY)
                return I2C_BUSY;
            
            if( ((TxBuf == nullptr) && (RxBuf == nullptr)) || (pStatus == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                _I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {          
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            _Transaction.SlaveAddress       = SlaveAddress;
            _Transaction.TxBuf              = TxBuf;
            _Transaction.TxLen              = TxLen;
            _Transaction.RxBuf              = RxBuf;
            _Transaction.RxLen              = RxLen;  
            _Transaction.RepeatedStart      = RepeatedStart;  
            _Transaction.pStatus            = pStatus; 
            _Transaction.XferDoneCallback   = XferDoneCallback;
            
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
        
        I2CIntr::I2CStatus_t I2CIntr::MasterTxRx(Transaction_t* pTransaction)
        {
            if( pTransaction == nullptr )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                _I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }
            
            return MasterTxRx(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen, pTransaction->RxBuf ,
                              pTransaction->RxLen, pTransaction->RepeatedStart,pTransaction->pStatus, pTransaction->XferDoneCallback);            
        }
        
        I2CIntr::I2CStatus_t I2CIntr::Post(Transaction_t* pTransaction)
        {            
            if( (pTransaction == nullptr) || ((pTransaction->TxBuf == nullptr) && (pTransaction->RxBuf == nullptr)) || (pTransaction->pStatus == nullptr)  )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                _I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }
            
            if(_I2CState != READY)
            {      
                if( !_I2CTxnQueue.Write(pTransaction) )
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
                return MasterTxRx(pTransaction->SlaveAddress, pTransaction->TxBuf ,pTransaction->TxLen, pTransaction->RxBuf ,
                                  pTransaction->RxLen, pTransaction->RepeatedStart,pTransaction->pStatus,pTransaction->XferDoneCallback);  
            }                      
        }
        
        I2CIntr::I2CStatus_t I2CIntr::SlaveRx(uint8_t* RxBuf, uint32_t RxLen, I2CCallback_t XferDoneCallback )
        {
#if 0
            if(_I2CState != READY)
                return I2C_BUSY;
            
            if( (RxLen == 0) || (RxBuf == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                _I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }            
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {          
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            _I2CState = SLAVE_RX;   
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            /* Enable Address Acknowledge */
            _I2Cx->CR1 |= I2C_CR1_ACK;
            
            InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_ENABLE_ALL);
            
            /* Wait until ADDR flag is set */
            
            I2C_LOG_STATES(I2C_LOG_START_SLAVE_RX);
            
            I2C_DEBUG_LOG(I2C_OK);
#endif
            return I2C_OK; 
        }
        
        I2CIntr::I2CStatus_t I2CIntr::SlaveTx(uint8_t* TxBuf, uint32_t TxLen, I2CCallback_t XferDoneCallback )
        {          
#if 0
            if(_I2CState != READY)
                return I2C_BUSY;
            
            if( (TxLen == 0) || (TxBuf == nullptr) )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                _I2CStatus = I2C_INVALID_PARAMS;
                return I2C_INVALID_PARAMS;                
            }          
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true )
            {
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;
            }
            
            //_Transaction.SlaveAddress       = SlaveAddress;
            _Transaction.TxBuf              = TxBuf;
            _Transaction.TxLen              = TxLen;
            _Transaction.RxBuf              = nullptr;
            _Transaction.RxLen              = 0;  
            _Transaction.RepeatedStart      = 0;  
            _Transaction.XferDoneCallback   = XferDoneCallback;
            
            _I2CState = SLAVE_TX;
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_ENABLE_ALL);
            
            /* Generate Start */
            // Start();
            I2C_LOG_STATES(I2C_LOG_START_MASTER_TX);
#endif
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
        
        void I2CIntr::SetCallback(I2CCallbackType_t I2CCallbackType, I2CCallback_t I2CCallback)
        {
            switch(I2CCallbackType)
            {
            case I2C_RX_QUEUE_FULL_CALLBACK: _RxQueueFullCallback = I2CCallback; break;
            case I2C_TX_QUEUE_EMPTY_CALLBACK: _TxQueueEmptyCallback = I2CCallback; break;
            case I2C_SLAVE_TX_COMPLETE_CALLBACK: _SlaveTxDoneCallback = I2CCallback; break;
            case I2C_SLAVE_RX_COMPLETE_CALLBACK: _SlaveRxDoneCallback = I2CCallback; break;
            default: break; 
            }            
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
            default:                                    break; 
            }
        }
        
        void I2CIntr::ISR( IRQn_Type event )
        {                
            // while(_I2Cx->SR1)
            // {
            switch(POSITION_VAL(_I2Cx->SR1))
            {
            case I2C_SR1_SB_Pos : Master_SB_Handler(); 
            break;
            case I2C_SR1_ADDR_Pos:                    
                if((_I2CState == MASTER_TX) || (_I2CState == MASTER_RX) )  Master_ADDR_Handler();
                else if( /*(_I2CState == SLAVE_RX_LISTENING) || */ (_I2CState == READY) || (_I2CState == SLAVE_RX)) Slave_ADDR_Handler(); // changing to Slave Tx here
                else                                                                    while(1);/* Fatal Error*/                        
                break;
                
            case I2C_SR1_BTF_Pos :  
                if     (_I2CState == MASTER_RX)     Master_Rx_BTF_Handler();
                else if(_I2CState == MASTER_TX)     Master_Tx_BTF_Handler();
                else if(_I2CState == SLAVE_RX )     Slave_Rx_BTF_Handler();
                else if(_I2CState == SLAVE_TX )     Slave_Tx_BTF_Handler(); 
                else if(_I2CState == MASTER_RX_REPEATED_START )     return;
                else                                while(1);/* Fatal Error*/ 
                break;
                
            case I2C_SR1_ADD10_Pos : 
                break;
                
            case I2C_SR1_STOPF_Pos : Slave_STOP_Handler(); 
            break;
            
            case I2C_SR1_RXNE_Pos : 
                if     (_I2CState == MASTER_RX)     Master_RxNE_Handler(); 
                else if(_I2CState == SLAVE_RX )     Slave_RxNE_Handler();  
                else                                while(1);/* Fatal Error*/ 
                break;
                
            case I2C_SR1_TXE_Pos : 
                if     (_I2CState == MASTER_TX)     Master_TxE_Handler(); 
                else if(_I2CState == SLAVE_TX )     Slave_TxE_Handler(); 
                else                                while(1);/* Fatal Error*/                           
                break;
                
            case I2C_SR1_BERR_Pos : 
                Master_BERR_Handler();
                break;
                
            case I2C_SR1_ARLO_Pos : 
                Master_AL_Handler(); 
                break;
                
            case I2C_SR1_AF_Pos :
                if     (_I2CState == MASTER_TX)     Master_AF_Handler(); 
                else if(_I2CState == SLAVE_TX )     Slave_AF_Handler();
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
            if(_I2CState == MASTER_TX) 
            {
                _I2Cx->DR = _Transaction.SlaveAddress & I2C_DIR_WRITE; 
                I2C_LOG_STATES(I2C_LOG_SB_MASTER_TX);
            }
            else if(_I2CState == MASTER_RX) 
            {
                /* start listening RxNE and TxE interrupts */                
                InteruptControl(HAL::I2CIntr::I2C_EVENT_INTERRUPT_BUFFER_ENABLE);
                
                _I2Cx->DR = _Transaction.SlaveAddress | I2C_DIR_READ;

				if(_Transaction.RxLen == 2U)
				{
					/* Enable Pos */
                    _I2Cx->CR1 |= I2C_CR1_POS;

					I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX_2);					
				}
                
                I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX);
            }
            else if(_I2CState == MASTER_RX_REPEATED_START)
            {
                /* Repeated start is handled here, clear the flag*/
                _Transaction.RepeatedStart = 0;
                
                /* start listening RxNE and TxE interrupts */                
                InteruptControl(HAL::I2CIntr::I2C_EVENT_INTERRUPT_BUFFER_ENABLE);
                
                _I2Cx->DR = _Transaction.SlaveAddress | I2C_DIR_READ;  
                
                _I2CState = MASTER_RX;
                I2C_LOG_STATES(I2C_LOG_SB_MASTER_RX_REPEATED_START);
            }
            else
            {
                while(1);
            }
        }
        
        void I2CIntr::Master_ADDR_Handler()
        {
            if(_I2CState == MASTER_RX)
            {
                if(_Transaction.RxLen == 0U)
                {
                    /* Clear ADDR flag */
                    LL_I2C_ClearFlag_ADDR(_I2Cx);
                    
                    /* Generate Stop */
                    _I2Cx->CR1 |= I2C_CR1_STOP;
                    
                    Stop();
                    
                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_0);
                }
                else if(_Transaction.RxLen == 1U)   
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
                       // InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_DISABLE_ALL);                
                        
                       // _I2CState = READY;
                        
                        /* Generate Stop */
                    	_I2Cx->CR1 |= I2C_CR1_STOP;
                        
                        I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1);
                    }                    
                }
                else if(_Transaction.RxLen == 2U)   
                {                    
                     // earlier /* Enable Acknowledge */
                    _I2Cx->CR1 |= I2C_CR1_ACK;  
                    
                    /* Enable Pos, Semd NACK after receiving 2nd byte, This case is only for Rx == 2 bytes */
                    _I2Cx->CR1 |= I2C_CR1_POS;      

                   /* Clear ADDR flag */
                    LL_I2C_ClearFlag_ADDR(_I2Cx);
                    
                    /* Disable Acknowledge */ 
                   // _I2Cx->CR1 &= ~I2C_CR1_ACK;
                    
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
                
                if(_Transaction.TxLen > 0)
                {
                    _I2Cx->DR = (*_Transaction.TxBuf++);
                    _Transaction.TxLen--;
                    I2C_LOG_STATES(I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0);
                }
                else if(_Transaction.RxLen > 0)  // Need to receive data
                {             
                    if( _I2CState == MASTER_RX_REPEATED_START)
                        return;
                    
                    if(_Transaction.RepeatedStart)
                    {
                        /* Generate Start */
                        _I2Cx->CR1 |= I2C_CR1_START;
                        
                        _I2CState = MASTER_RX_REPEATED_START;
                        
                        I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_REPEATED_START);
                    }
                    else
                    {
                        /* Generate Stop */
                        _I2Cx->CR1 |= I2C_CR1_STOP;
                        
                        Stop();
                        
                        _I2CState = MASTER_RX;
                        
                        I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_STOP);
                    }
                    
                    InteruptControl(HAL::I2CIntr::I2C_EVENT_INTERRUPT_BUFFER_DISABLE);
                    
                    /* Enable Acknowledge */
                    _I2Cx->CR1 |= I2C_CR1_ACK;
                    
                    /* Disable Pos */
                    _I2Cx->CR1 &= ~I2C_CR1_POS;                
                }    
                else
                {
                    // Nothing to transmit and receive                 
                    
                    /* Generate Stop */
                    _I2Cx->CR1 |= I2C_CR1_STOP;
                    
                    Stop(); 
                }
            }
            else
            {
                
            }
        }
        
         void I2CIntr::Master_RxNE_Handler()
        {
            if(_Transaction.RxLen > 3U)
            {
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR;
                _Transaction.RxLen--;
                I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_GT_3);
            }
            else if((_Transaction.RxLen == 2U) || (_Transaction.RxLen == 3U))
            {  
                // This is already done in ADDR for Rx 2 byte only
                // Need to do here if Rx>2, In this case it will not be done in ADDR
                
                /* Disable Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                /* Enable Pos */
                _I2Cx->CR1 |= I2C_CR1_POS;
                
                I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_SIZE_2_OR_3);
            }
            else /* Last byte */ 
            {
                /* Disable EVT, BUF and ERR interrupt */
                // ??
                InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_DISABLE_ALL);
                
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR;
                _Transaction.RxLen--;
                
                I2C_LOG_STATES(I2C_LOG_RXNE_MASTER_LAST);
                
                /* ??Stop condition will be sent in "MasterReceive_BTF_Handler"*/
                 Stop();
            }
        }
        
        void I2CIntr::Master_TxE_Handler()
        {
            if( _I2CState == MASTER_RX_REPEATED_START)
                return;
            
            if(_Transaction.TxLen > 0)
            {
                _I2Cx->DR = (*_Transaction.TxBuf++);
                _Transaction.TxLen--;
                //I2C_LOG_STATES(I2C_LOG_TXE);
            } 
			else
			{				
				/* Generate Stop */
                _I2Cx->CR1 |= I2C_CR1_STOP;
              // TxE and BTF are cleared by hardware by the Stop condition

				Stop();
				
				I2C_LOG_STATES(I2C_LOG_TXE_DONE);
			}
        }
        
        void I2CIntr::Master_Rx_BTF_Handler()
        {
            if( _I2CState == MASTER_RX_REPEATED_START)
                return;
            
            if(_Transaction.RxLen == 3U)
            {
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR ;
                _Transaction.RxLen--;   
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_3);
            }
            else if(_Transaction.RxLen == 2U)
            {
                /* Disable EVT and ERR interrupt */
                InteruptControl(HAL::I2CIntr::I2C_EVENT_INTERRUPT_DISABLE);                
                InteruptControl(HAL::I2CIntr::I2C_ERROR_INTERRUPT_DISABLE);
                
                /* Ready for next data transfer */
                _I2CState = READY;
                
                (*_Transaction.pStatus) = I2C_XFER_DONE;
                
                /* Generate Stop */
                _I2Cx->CR1 |= I2C_CR1_STOP;
                
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR;
                _Transaction.RxLen--;
                
                /* Read data from DR (shift register) */
                (*_Transaction.RxBuf++) = _I2Cx->DR;
                _Transaction.RxLen--;  
                
//                if(_Transaction.XferDoneCallback)
//                    _Transaction.XferDoneCallback->CallbackFunction();
                
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED);
                
                Stop();
            }
            else
            {
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR;
                _Transaction.RxLen--;
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_RX_SIZE_GT_3);
            }          
        }        
        
        void I2CIntr::Master_Tx_BTF_Handler()
        {
            if(_Transaction.TxLen > 0)
            {
                _I2Cx->DR = (*_Transaction.TxBuf++);
                _Transaction.TxLen--;
                I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_GT_0);
            }
            else if(_Transaction.RxLen > 0)  // Need to receive data
            {             
                
                if( _I2CState == MASTER_RX_REPEATED_START)
                    return;
                
                if(_Transaction.RepeatedStart)
                {
                    /* Generate Start */
                    _I2Cx->CR1 |= I2C_CR1_START;
                    
                    _I2CState = MASTER_RX_REPEATED_START;
                    
                    I2C_LOG_STATES(I2C_LOG_BTF_MASTER_TX_REPEATED_START);
                }
                else
                {
                    /* Generate Stop */
                    _I2Cx->CR1 |= I2C_CR1_STOP;
                    
                    Stop();
                    
                    _I2CState = MASTER_RX;

                    /* Generate Start */
                    Start();
                    
                    I2C_LOG_STATES(I2C_LOG_STOP_MASTER_RX_WITHOUT_REPEATED_START);
                }
                
                InteruptControl(HAL::I2CIntr::I2C_EVENT_INTERRUPT_BUFFER_DISABLE);
                
                /* Enable Acknowledge */
                _I2Cx->CR1 |= I2C_CR1_ACK;
                
                /* Disable Pos */
                _I2Cx->CR1 &= ~I2C_CR1_POS;                
            }    
            else
            {
                // Nothing to transmit and receive                 
                
                /* Generate Stop */
                _I2Cx->CR1 |= I2C_CR1_STOP;
                
                Stop();
            }
        }
        
        void I2CIntr::Slave_ADDR_Handler()
        {            
            if( (_I2Cx->SR2 & LL_I2C_SR2_DUALF) == RESET)
            {
                //SlaveAddrCode = hi2c->Init.OwnAddress1;
                _Transaction.SlaveAddress = (_I2Cx->OAR1 & 0x00FE); // Bit 1-7 are address
            }
            else
            {
                //SlaveAddrCode = hi2c->Init.OwnAddress2;
                _Transaction.SlaveAddress = (_I2Cx->OAR2 & 0x00FE); // Bit 1-7 are address
            }            
            /* Transfer Direction requested by Master */
            if( (_I2Cx->SR2 & LL_I2C_SR2_TRA) == RESET)
            {
                //TransferDirection = I2C_DIRECTION_TRANSMIT;
                _I2CState = SLAVE_RX;               
            }
            else
            {
                _I2CState = SLAVE_TX;
                //_Transaction.SlaveAddress |= I2C_DIR_READ;
            }             
        }
        
        void I2CIntr::Slave_Tx_BTF_Handler()
        {
            uint8_t Data;
            if(_I2CSlaveTxQueue.IsQueueEmpty() != true)
            {
                _I2CSlaveTxQueue.Read(Data);
                
                _I2Cx->DR = Data;
                
                I2C_LOG_STATES(I2C_LOG_BTF_SLAVE_TX);
            }
            else
            {                
                _I2Cx->DR = 0xDE; // Send some junk data
                
                I2C_LOG_STATES(I2C_LOG_BTF_SLAVE_TX_QUEUE_EMPTY);
                
                if(_TxQueueEmptyCallback)
                    _TxQueueEmptyCallback->CallbackFunction();
            }
        }
        
        void I2CIntr::Slave_Rx_BTF_Handler()
        {
#if 0
            if(_Transaction.RxLen != 0U)
            {
                (*_Transaction.RxBuf++) = _I2Cx->DR ;
                _Transaction.RxLen--;   
                I2C_LOG_STATES(I2C_LOG_BTF_SLAVE_RX);
            }
#endif
            /* Queue the received data */
            if(_I2CSlaveRxQueue.IsQueueFull() == false)
            {
                _I2CSlaveRxQueue.Write(_I2Cx->DR);
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
            if(_I2CSlaveTxQueue.IsQueueEmpty() != true)
            {
                _I2CSlaveTxQueue.Read(Data);
                
                _I2Cx->DR = Data;
                
                I2C_LOG_STATES(I2C_LOG_TxE_SLAVE_TX);
            }
            else
            {                
                _I2Cx->DR = 0xDE; // Send some junk data
                
                I2C_LOG_STATES(I2C_LOG_TxE_SLAVE_TX_QUEUE_EMPTY);
                
                if(_TxQueueEmptyCallback)
                    _TxQueueEmptyCallback->CallbackFunction();
            }            
        }
        
        void I2CIntr::Slave_RxNE_Handler()
        {
#if 0
            if(_Transaction.RxLen != 0U)
            {
                /* Read data from DR */
                (*_Transaction.RxBuf++) = _I2Cx->DR;
                _Transaction.RxLen--;
                
                I2C_LOG_STATES(I2C_LOG_RXNE_SLAVE_RX);
                
                if((_Transaction.RxLen == 0U) && (_I2CState == SLAVE_RX))
                {
                    /* Disable EVT, BUF and ERR interrupt */
                    InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_DISABLE_ALL);
                    
                    _I2CState = READY;
                    
                    if(_Transaction.XferDoneCallback)
                        _Transaction.XferDoneCallback->CallbackFunction();
                }
            }
#endif
            /* Queue the received data */
            if(_I2CSlaveRxQueue.IsQueueFull() == false)
            {
                _I2CSlaveRxQueue.Write(_I2Cx->DR);
                I2C_LOG_STATES(I2C_LOG_RXNE_SLAVE_RX_QUEUED);
            }
            else
            {
                if(_RxQueueFullCallback)
                    _RxQueueFullCallback->CallbackFunction();
                
                I2C_LOG_STATES(I2C_LOG_RXNE_SLAVE_RX_QUEUE_FULL);
            }           
        }
        
        void I2CIntr::Slave_STOP_Handler()
        {
            LL_I2C_ClearFlag_STOP(_I2Cx);            
            
            /* Execute the RxDone Callback */
            if(_SlaveRxDoneCallback)
                _SlaveRxDoneCallback->CallbackFunction();
            
            _I2CState = READY;
            
            I2C_LOG_STATES(I2C_LOG_STOPF_FLAG);
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
        
        void I2CIntr::Slave_AF_Handler()
        {            
            LL_I2C_ClearFlag_AF(_I2Cx);
            
            //_I2CStatus = I2C_ACK_FAIL;
            _I2CState = READY;
            
            if(_SlaveTxDoneCallback)
                _SlaveTxDoneCallback->CallbackFunction();
            
            I2C_LOG_STATES(I2C_LOG_AF_SLAVE_ACK_FAIL);
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
        
        
}
