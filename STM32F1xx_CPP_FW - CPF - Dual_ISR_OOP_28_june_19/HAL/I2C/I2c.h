/******************
** CLASS: I2c
**
** DESCRIPTION:
**  Implements the I2C in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
**
** FILE: I2c.h
**
******************/
#ifndef I2c_h
#define I2c_h

#include"GpioOutput.h"
#include"CPP_HAL.h"
#include"stm32f1xx_ll_i2c.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Queue.h"
#include"DMA.h"
#include"I2CInt.h"
#include"I2CDMA.h"
#include"I2CBase.h"

namespace HAL
{     
    
    class I2c : public I2CInt
    {
        
    public:
        
        I2c(Pin_t scl, Pin_t sda, Hz_t Hz = 100000U);
        
        ~I2c(){};
        
        I2CStatus_t HwInit(void *pInitStruct = nullptr);
        
        I2CStatus_t HwDeinit();
        
        inline void ClockEnable();
        
        inline void ClockDisable();
        
        inline I2CState_t GetState();
        
        inline uint8_t SlaveRxDataAvailable();
        
        bool DetectSlave(uint8_t SlaveAddress);
        
        void ScanBus(uint8_t* pFoundDevices, uint8_t size);
        
        inline void Start();
        
        void Interrupt_Tx_Done_Handler(uint32_t StopFlag);
        
        void Interrupt_Rx_Done_Handler();
        
        void DMA_Tx_Done_Handler();
        
        void DMA_Rx_Done_Handler();
        
        void TxnDoneHandler(uint32_t StopFlag);
        
        inline void SendAddress(uint8_t SlaveAddress);
        
        inline void WriteDataRegister(uint8_t data);
        
        inline uint8_t ReadDataRegister();             
        
        I2CStatus_t     TxPoll(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen);        
        
        I2CStatus_t     RxPoll(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen);
        
        
        
        I2CStatus_t Post(Transaction_t* pTransaction, uint32_t Mode = 0);
        
        I2CStatus_t SlaveTx_Intr(uint8_t* pdata, uint32_t len, I2CCallback_t XferDoneCallback = nullptr );
        
        I2CStatus_t SlaveRx_Intr(uint8_t* pdata, uint32_t len, I2CCallback_t XferDoneCallback = nullptr );      
        
        inline uint8_t ReadRxData();
        
        inline bool StartConditionGenerated(uint32_t timeout);
        
        inline bool Busy(uint32_t timeout);
        
        inline bool SlaveAddressSent(uint32_t timeout);
        
        inline bool TransmitterEmpty(uint32_t timeout);
        
        inline bool TransferDone(uint32_t timeout);
        
        inline void SoftReset();
        
        inline bool ACKFail(uint32_t timeout);
        
        inline bool DataAvailableRXNE(uint32_t timeout);   
        
        inline bool StopCondition(uint32_t timeout);
        
        inline void SetRxFullCallback(I2CCallback_t I2CCallback);
        
        
        I2CStatus_t AddressMatch();
        
        virtual void ISR( IRQn_Type event );
        
        void InteruptControl(I2CInterrupt_t I2CInterrupt);
        
        void InterruptControl(bool enable, uint16_t InterruptFlag){ enable ? SET_BIT(m_I2Cx->CR2, InterruptFlag) : CLEAR_BIT(m_I2Cx->CR2, InterruptFlag) ;}
        
        void Enable_EVT_Interrupt(){SET_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN);}
        
        void Enable_BUF_Interrupt(){SET_BIT(m_I2Cx->CR2, I2C_CR2_ITBUFEN);}
        
        void Enable_ERR_Interrupt(){SET_BIT(m_I2Cx->CR2, I2C_CR2_ITERREN);}
        
        void Enable_EVT_BUF_ERR_Interrupt(){SET_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);}
        
        void Enable_EVT_ERR_Interrupt(){SET_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);}
        
        void Disable_EVT_Interrupt(){CLEAR_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN);}
        
        void Disable_BUF_Interrupt(){CLEAR_BIT(m_I2Cx->CR2, I2C_CR2_ITBUFEN);}
        
        void Disable_ERR_Interrupt(){CLEAR_BIT(m_I2Cx->CR2, I2C_CR2_ITERREN);}
        
        void Disable_EVT_BUF_ERR_Interrupt(){CLEAR_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);}
        
        void Disable_EVT_ERR_Interrupt(){CLEAR_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);}
        
        inline I2CStatus_t StartListening();
        
        inline bool StopFlagCleared(uint32_t timeout);
        
		void I2C1_DMA_Tx_Done_Handler();
        
		void I2C1_DMA_Rx_Done_Handler();
#if I2C_DMA
        
#endif  
        inline void LogStates(I2CLogs_t log);
    private:
        GpioOutput              m_sclPin;
        GpioOutput              m_sdaPin;
        Hz_t                    m_hz;
        I2Cx_t                  m_I2Cx;            
        Transaction_t           m_Transaction;

        I2CTxnQueue_t           m_I2CTxnQueue;
        
        /* It must be volatile becoz it is shared between ISR and main loop */
        volatile I2CState_t     m_I2CState; 
        /* It must be volatile becoz it is shared between ISR and main loop */
        volatile I2CStatus_t    m_I2CStatus;
        
#if I2C_DMA

#endif
        
    public:         
#ifdef I2C_DEBUG
        I2CLogs_t       I2CStates[I2C_LOG_STATES_SIZE];
        volatile uint32_t        I2CStates_Idx;
        Utils::DebugLog<DBG_LOG_TYPE>* dbg_log_instance;
#endif
    };   
    
    
    
    inline uint8_t I2c::ReadRxData()
    {
        uint8_t Data;
        m_I2CSlaveRxQueue.Read(Data);
        return Data;
    }
    
    bool I2c::StartConditionGenerated(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_SB(m_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_SB(m_I2Cx);
    }
    
    inline uint8_t I2c::SlaveRxDataAvailable()
    {
        return m_I2CSlaveRxQueue.Available();
    }
    
    bool I2c::Busy(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_BUSY(m_I2Cx) == 1U) );
        return (bool)LL_I2C_IsActiveFlag_BUSY(m_I2Cx);
    }
    
    bool I2c::StopFlagCleared(uint32_t timeout)
    {   
#if 1
        static int StopCountMax;
        uint32_t Timeout=0;
        
        while( (timeout--) && (m_I2Cx->CR1 & (I2C_CR1_STOP)) )
        {            
            Timeout++;          
        }
        
        if(StopCountMax<Timeout) 
            StopCountMax = Timeout;
#else
        while( (timeout--) && (m_I2Cx->CR1 & (I2C_CR1_STOP)) );        
#endif
        return (bool)(m_I2Cx->CR1 & (I2C_CR1_STOP));
    }
    
    
    bool I2c::SlaveAddressSent(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_ADDR(m_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_ADDR(m_I2Cx);
    } 
    bool I2c::TransmitterEmpty(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_TXE(m_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_TXE(m_I2Cx);
    }
    
    bool I2c::TransferDone(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_BTF(m_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_BTF(m_I2Cx);
    }
    void I2c::SoftReset()
    {
        LL_I2C_EnableReset(m_I2Cx);
        LL_I2C_DisableReset(m_I2Cx);
        HwInit();
    }
    bool I2c::ACKFail(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_AF(m_I2Cx) == 1U) );
        return (bool)LL_I2C_IsActiveFlag_AF(m_I2Cx);
    }
    
    bool I2c::DataAvailableRXNE(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_RXNE(m_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_RXNE(m_I2Cx);
    }
    
    I2c::I2CStatus_t I2c::StartListening()
    {
        /* Wait until BUSY flag is reset */
        if( Busy(I2C_TIMEOUT) == true ) 
        {          
            //I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
            return I2C_BUSY_TIMEOUT;                
        }        
        /* Disable Pos */
        m_I2Cx->CR1 &= ~I2C_CR1_POS;
        
        /* Enable Address Acknowledge */
        m_I2Cx->CR1 |= I2C_CR1_ACK;
        
        // m_I2CState = SLAVE_RX_LISTENING;
        m_I2CState = READY;
        
        Enable_EVT_BUF_ERR_Interrupt();
        
        return I2C_OK;
    }
    
    I2c::I2CState_t I2c::GetState()
    {
        return m_I2CState;
    }
    
    
    
#ifdef I2C_DEBUG     
#pragma inline = forced
    void I2c::LogStates(I2CLogs_t log)
    {
#if 0
        I2CStates[I2CStates_Idx++  % I2C_LOG_STATES_SIZE] = log;
        if(I2CStates_Idx == I2C_LOG_STATES_SIZE )
            I2CStates_Idx = 0;
#else
        dbg_log_instance->Log(log);
#endif
    }
    
    //#define I2C_LOG_STATES(log) (I2CStates[I2CStates_Idx++  % I2C_LOG_STATES_SIZE] = (log))
#define I2C_DEBUG_LOG(log) (m_I2CStatus = (log))  // 108 bytes of ROM
#define I2C_LOG_STATES(log) LogStates(log)
#define I2C_LOG_EVENTS(log) //LogStates(log)
    
#else
    
#define I2C_LOG_STATES(log)
#define I2C_DEBUG_LOG(log)
#define I2C_LOG_EVENTS(log)
    
#endif    
    
}
#endif //I2c_h
