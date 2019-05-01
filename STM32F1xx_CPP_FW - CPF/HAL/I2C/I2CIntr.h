/******************
** CLASS: I2CIntr
**
** DESCRIPTION:
**  Implements the I2C in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
**
** FILE: I2CIntr.h
**
******************/
#ifndef I2CIntr_h
#define I2CIntr_h

#include"GpioOutput.h"
#include"CPP_HAL.h"
#include"stm32f1xx_ll_i2c.h"
#include"InterruptManager.h"
#include"Callback.h"

namespace HAL
{
#define I2C_LOG_STATES_SIZE 500
#define I2C_LOG_STATES(log) (I2CStates[I2CStates_Idx++  % I2C_LOG_STATES_SIZE] = (log))
    
#define I2C_DEBUG 
    
    class I2CIntr : public InterruptSource
    {
    public:
        using Pin_t = HAL::Gpio::Pin_t;
        typedef uint32_t Hz_t ;
        //typedef uint32_t I2CStatus_t;
        typedef I2C_TypeDef* I2Cx_t;
        
        typedef struct
        {
            uint8_t  SlaveAddress;
            uint16_t TxSize;
            uint16_t RxSize; 
            uint8_t* TxBuf; 
            uint8_t* RxBuf;  
            bool RepeatedStart;
        }Transaction_t;
        
        typedef enum
        {
            I2C_OK,
            I2C_BUSY,
            I2C_BUSY_TIMEOUT,
            I2C_START_TIMEOUT,
            I2C_ADDR_TIMEOUT,
            I2C_BTF_TIMEOUT,
            I2C_RXNE_TIMEOUT,
            I2C_TXE_TIMEOUT,
            I2C_STOP_TIMEOUT,
            I2C_ACK_FAIL,
            I2C_ARB_LOST,
            I2C_INTR_PENDING,
            I2C_BUS_ERROR,
            I2C_DATA_OVR,
            I2C_INVALID_PARAMS,
        }I2CStatus_t;
        
        typedef enum
        {
            NONE,
            I2C_LOG_STOP_INTR,
            I2C_LOG_START_MASTER_TX,
            I2C_LOG_START_MASTER_RX,
            I2C_LOG_START_MASTER_TXRX,
            I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_0,
            I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1_DMA,
            I2C_LOG_ADDR_INTR_MASTER_RX_STOP,
            I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2_DMA,
            I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2,
            I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_OTHER,
            I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0,
            I2C_LOG_ADDR_INTR_MASTER_TX_REPEATED_START,
            I2C_LOG_ADDR_INTR_MASTER_TX_STOP,
            I2C_LOG_TXE,
            I2C_LOG_TXE_DONE,            
            I2C_LOG_BTF_MASTER_TX_GT_0,
            I2C_LOG_BTF_MASTER_TX_REPEATED_START,
            I2C_LOG_BTF_MASTER_TX_STOP,
            I2C_LOG_BTF_MASTER_RX_SIZE_3,
            I2C_LOG_BTF_MASTER_RX_SIZE_2,
            I2C_LOG_BTF_MASTER_RX_SIZE_GT_3,
            I2C_LOG_BTF_MASTER_RXNE_SIZE_GT_3,
            I2C_LOG_BTF_MASTER_RXNE_SIZE_2_OR_3,
            I2C_LOG_BTF_MASTER_RXNE_LAST,
            I2C_LOG_BTF_MASTER_BERR,
            I2C_LOG_BTF_MASTER_ACK_FAIL,
            I2C_LOG_BTF_MASTER_ARB_LOST,
            I2C_LOG_SB_MASTER_TX,
            I2C_LOG_SB_MASTER_RX,
            I2C_LOG_SB_MASTER_RX_REPEATED_START,
            I2C_LOG_BTF_MASTER_DATA_OVR,
        }I2CStates_t;
        
        typedef enum
        {
            RESET,
            BUSY,
            READY,
            MASTER_TX,
            MASTER_RX,
            SLAVE_TX,
            SLAVE_RX,
            MASTER_RX_REPEATED_START,
            MASTER_TX_ACK_FAIL,

        }I2CState_t;
        
        typedef enum 
        {
            I2C_EVENT_INTERRUPT_ENABLE,
            I2C_EVENT_INTERRUPT_DISABLE,
            I2C_EVENT_INTERRUPT_BUFFER_ENABLE,
            I2C_EVENT_INTERRUPT_BUFFER_DISABLE,
            I2C_ERROR_INTERRUPT_ENABLE,
            I2C_ERROR_INTERRUPT_DISABLE,
            I2C_INTERRUPT_ENABLE_ALL,
            I2C_INTERRUPT_DISABLE_ALL,
            
        }I2CInterrupt_t;
        
        static const uint32_t I2C_TIMEOUT           = 5000U;
        static const uint16_t I2C_DIR_WRITE         = 0xfff7U;
        static const uint16_t I2C_DIR_READ          = 0x0001U;
        static const uint16_t I2C_OWN_SLAVE_ADDRESS = 0x08<<1;
        
        I2CIntr(Pin_t scl, Pin_t sda, Hz_t Hz = 100000U);
        
        ~I2CIntr(){};
        
        I2CStatus_t HwInit(void *pInitStruct = nullptr);
        
        I2CStatus_t HwDeinit();
        
        inline void ClockEnable();
        
        inline void ClockDisable();
        
        inline I2CState_t GetState();
        
        bool DetectSlave(uint8_t SlaveAddress);
        
        void ScanBus(uint8_t* pFoundDevices, uint8_t size);
        
        inline void Start();
        
        inline void Stop();
        
        inline void SendAddress(uint8_t SlaveAddress);
        
        inline void WriteDataRegister(uint8_t data);
        
        inline uint8_t ReadDataRegister();
        
        I2CStatus_t MasterTx(uint16_t SlaveAddress,uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered = nullptr );
        
        I2CStatus_t MasterRx(uint16_t SlaveAddress,uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered = nullptr );
        
        I2CStatus_t MasterTxRx(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen, bool RepeatedStart);
        
        I2CStatus_t SlaveTx(uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered = nullptr );
        
        I2CStatus_t SlaveRx(uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered = nullptr );
        
        inline bool Busy(uint32_t timeout);
        
        inline bool StartConditionGenerated(uint32_t timeout);
        
        inline bool SlaveAddressSent(uint32_t timeout);
        
        inline bool TransmitterEmpty(uint32_t timeout);
        
        inline bool TransferDone(uint32_t timeout);
        
        inline void SoftReset();
        
        inline bool ACKFail(uint32_t timeout);
        
        inline bool DataAvailableRXNE(uint32_t timeout);   
        
        inline bool StopCondition(uint32_t timeout);
        
        I2CStatus_t AddressMatch();
        
        virtual void ISR( IRQn_Type event );
        
        void InteruptControl(I2CInterrupt_t I2CInterrupt);
        
        inline I2CStatus_t StartListening();
        
        inline bool StopFlagCleared(uint32_t timeout);
        
        void Master_SB_Handler(); 
        
        void Master_ADDR_Handler(); 
            
        void Master_TxE_Handler();
        
        void MasterTransmit_BTF_Handler();
        
        void MasterReceive_BTF_Handler(); 
            
        void Master_RxNE_Handler();
        
        void Slave_STOP_Handler();
        
        void Master_BERR_Handler();
        
        void Master_AF_Handler();
        
        void Master_AL_Handler();
        
        void Master_OVR_Handler();
        
    private:
        GpioOutput      _sclPin;
        GpioOutput      _sdaPin;
        Hz_t            _hz;
        I2Cx_t          _I2Cx;
        I2CState_t      _I2CState;
        I2CStatus_t     _I2CStatus;
        Transaction_t   _Transaction;
        uint32_t        start;
        uint32_t        stop;
        I2CStates_t     I2CStates[I2C_LOG_STATES_SIZE];
        uint32_t        I2CStates_Idx;
        
    };
    
    void I2CIntr::Start()
    {
        LL_I2C_GenerateStartCondition(_I2Cx);
        start++;
    }
    
    void I2CIntr::Stop()
    {
        LL_I2C_GenerateStopCondition(_I2Cx);
        /* Wait with timeout Until STOP falg is set by HW */
        //while(_I2Cx->CR1 & (I2C_CR1_STOP));
        if (StopFlagCleared(I2C_TIMEOUT) == true)
            _I2CStatus = I2C_STOP_TIMEOUT;
    }
    
    void I2CIntr::SendAddress(uint8_t SlaveAddress)
    {
        
    }
    
    void I2CIntr::WriteDataRegister(uint8_t data)
    {
        LL_I2C_TransmitData8(_I2Cx, data);
    }
    
    uint8_t I2CIntr::ReadDataRegister()
    {
        return LL_I2C_ReceiveData8(_I2Cx);
    }
    
    bool I2CIntr::Busy(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_BUSY(_I2Cx) == 1U) );
        return (bool)LL_I2C_IsActiveFlag_BUSY(_I2Cx);
    }
    
    bool I2CIntr::StopFlagCleared(uint32_t timeout)
    {
        while( (timeout--) && (_I2Cx->CR1 & (I2C_CR1_STOP)) );
        return (bool)(_I2Cx->CR1 & (I2C_CR1_STOP));
    }
    
    bool I2CIntr::StartConditionGenerated(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_SB(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_SB(_I2Cx);
    }
    
    bool I2CIntr::SlaveAddressSent(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_ADDR(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_ADDR(_I2Cx);
    } 
    bool I2CIntr::TransmitterEmpty(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_TXE(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_TXE(_I2Cx);
    }
    
    bool I2CIntr::TransferDone(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_BTF(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_BTF(_I2Cx);
    }
    void I2CIntr::SoftReset()
    {
        LL_I2C_EnableReset(_I2Cx);
        LL_I2C_DisableReset(_I2Cx);
        HwInit();
    }
    bool I2CIntr::ACKFail(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_AF(_I2Cx) == 1U) );
        return (bool)LL_I2C_IsActiveFlag_AF(_I2Cx);
    }
    
    bool I2CIntr::DataAvailableRXNE(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_RXNE(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_RXNE(_I2Cx);
    }
    
    bool I2CIntr::StopCondition(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_STOP(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_STOP(_I2Cx);
    }
    I2CIntr::I2CStatus_t I2CIntr::StartListening()
    {
        /* Wait until BUSY flag is reset */
        if( Busy(I2C_TIMEOUT) == true ) 
        {          
            //I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
            return I2C_BUSY_TIMEOUT;                
        }        
        /* Disable Pos */
        _I2Cx->CR1 &= ~I2C_CR1_POS;
        
        /* Enable Address Acknowledge */
        _I2Cx->CR1 |= I2C_CR1_ACK;
        
        return I2C_OK;
    }
    
    I2CIntr::I2CState_t I2CIntr::GetState()
    {
             return _I2CState;
    }
    
#if defined (I2C_DEBUG)
#define I2C_DEBUG_LOG(log) (_I2CStatus = (log))  // 108 bytes of ROM
#else 
#define I2C_DEBUG_LOG(log)
#endif
    
}
#endif //I2CIntr_h
