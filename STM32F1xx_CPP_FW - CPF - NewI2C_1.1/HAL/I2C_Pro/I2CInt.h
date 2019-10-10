/******************
** CLASS: I2CInt
**
** DESCRIPTION:
**  Implements the I2C in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
**
** FILE: I2CInt.h
**
******************/
#ifndef I2CInt_h
#define I2CInt_h

#include"I2CBase.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Queue.h"


namespace HAL
{    

    
//#define I2C_RX_METHOD_1
    
    class I2CInt : public InterruptSource, public I2CBase
    {
    public:
        
        typedef Callback* I2CCallback_t;        
        using I2CSlaveRxQueue_t = Utils::Queue<uint8_t,64U> ;
        using I2CSlaveTxQueue_t = Utils::Queue<uint8_t,64U> ;
        
        typedef enum 
        {
            I2C_RX_QUEUE_FULL_CALLBACK,
            I2C_TX_QUEUE_EMPTY_CALLBACK,
            I2C_SLAVE_TX_COMPLETE_CALLBACK,
            I2C_SLAVE_RX_COMPLETE_CALLBACK,            
        }I2CCallbackType_t;
             
        
		typedef struct
        {
            uint16_t            SlaveAddress;
			uint8_t             RepeatedStart;
			I2CStatus_t*        pStatus;
            uint16_t            TxLen;
            uint16_t            RxLen; 
            uint8_t*            TxBuf; 
            uint8_t*            RxBuf;
            I2CCallback_t       XferDoneCallback;
        }Transaction_t;
		
        using I2CTxnQueue_t = Utils::Queue<Transaction_t*,10U> ;
        
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
        
        I2CInt(Pin_t scl, Pin_t sda, Hz_t Hz = 100000U);
        
        ~I2CInt(){};
        
        I2CStatus_t HwInit(void *pInitStruct = nullptr);
        
        I2CStatus_t HwDeinit();
        
        void Interrupt_Tx_Done_Handler(uint32_t StopFlag);
        
        void Interrupt_Rx_Done_Handler();
        
        void DMA_Tx_Done_Handler();
        
        void DMA_Rx_Done_Handler();
                
        void TxnDoneHandler(uint32_t StopFlag);
        
        inline void SendAddress(uint8_t SlaveAddress);
        
        inline void WriteDataRegister(uint8_t data);
        
        inline uint8_t ReadDataRegister();
        
        I2CStatus_t MasterTx(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);
        
        I2CStatus_t MasterTx(Transaction_t* pTransaction);
        
        I2CStatus_t MasterRx(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr );
        
        I2CStatus_t MasterRx(Transaction_t* pTransaction);
        
        I2CStatus_t MasterTxRx(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen,
                               uint8_t RepeatedStart,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);
        
        I2CStatus_t MasterTxRx(Transaction_t* pTransaction);
        
        I2CStatus_t Post(Transaction_t* pTransaction, uint32_t Mode = 0);
        
        I2CStatus_t SlaveTx(uint8_t* pdata, uint32_t len, I2CCallback_t XferDoneCallback = nullptr );
        
        I2CStatus_t SlaveRx(uint8_t* pdata, uint32_t len, I2CCallback_t XferDoneCallback = nullptr );        
        
        inline void SetRxFullCallback(I2CCallback_t I2CCallback);
        
        void SetCallback(I2CCallbackType_t I2CCallbackType, I2CCallback_t I2CCallback);
        
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
        
        void Master_SB_Handler(); 
        
        void Master_ADDR_Handler(); 
        
        void Master_TxE_Handler();
        
        void Master_Tx_BTF_Handler();
        
        void Master_Rx_BTF_Handler(); 
        
        void Slave_Tx_BTF_Handler();
        
        void Slave_Rx_BTF_Handler(); 
        
        void Master_RxNE_Handler();
        
        void Slave_STOP_Handler();
        
        void Master_BERR_Handler();
        
        void Master_AF_Handler();
        
        void Master_AL_Handler();
        
        void Master_OVR_Handler();
        
        void Slave_RxNE_Handler();
        
        void Slave_ADDR_Handler();
        
        void Slave_AF_Handler();
        
        void Slave_TxE_Handler();
    
        
    private:        
        I2CCallback_t           m_TxQueueEmptyCallback;
        I2CCallback_t           m_RxQueueFullCallback;
        I2CCallback_t           m_SlaveTxDoneCallback;
        I2CCallback_t           m_SlaveRxDoneCallback;
        I2CTxnQueue_t           m_I2CTxnQueue;
        
    public:
        Transaction_t       m_Transaction;
        I2CSlaveRxQueue_t   m_I2CSlaveRxQueue;
        I2CSlaveTxQueue_t   m_I2CSlaveTxQueue;
    };
    
    
    
    inline void I2CInt::SetRxFullCallback(I2CCallback_t I2CCallback)
    {
        m_RxQueueFullCallback = I2CCallback;
    }

}
#endif //I2CInt_h
