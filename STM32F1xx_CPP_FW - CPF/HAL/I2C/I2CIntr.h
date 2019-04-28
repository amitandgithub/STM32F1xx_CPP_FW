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
            RESET,
            BUSY,
            READY,
            MASTER_TX,
            MASTER_RX,
            SLAVE_TX,
            SLAVE_RX,
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
        
        static const uint32_t I2C_TIMEOUT       = 5000U;
        static const uint16_t I2C_DIR_WRITE     = 0xfff7U;
        static const uint16_t I2C_DIR_READ      = 0x0001U;
        static const uint16_t I2C_SLAVE_ADDRESS = 0x08<<1;
        
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
        uint32_t start,stop;
        
    };
    
    void I2CIntr::Start()
    {
        LL_I2C_GenerateStartCondition(_I2Cx);
        start++;
    }
    
    void I2CIntr::Stop()
    {
        LL_I2C_GenerateStopCondition(_I2Cx);
        /* Wait Until STOP falg is set by HW */
        while(_I2Cx->CR1 & (I2C_CR1_STOP));
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
