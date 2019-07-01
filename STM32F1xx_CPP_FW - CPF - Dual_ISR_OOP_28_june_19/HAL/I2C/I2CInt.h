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

#include"InterruptManager.h"
#include"Callback.h"
#include"Queue.h"
#include"I2CBase.h"


#define I2C_RX_METHOD_1

namespace HAL
{    
    
    class I2CInt : public InterruptSource, public I2CBase
    {
    public:
        
        I2CInt();
        
        ~I2CInt(){};
        
		I2CStatus_t HwInit(void *pInitStruct = nullptr);
        
        I2CStatus_t HwDeinit();
		
		void SetCallback(I2CCallbackType_t I2CCallbackType, I2CCallback_t I2CCallback);
				
        I2CStatus_t MasterTx_Intr(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);
        
        I2CStatus_t MasterTx_Intr(Transaction_t* pTransaction);
        
        I2CStatus_t MasterRx_Intr(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr );
        
        I2CStatus_t MasterRx_Intr(Transaction_t* pTransaction);
        
        I2CStatus_t MasterTxRx_Intr(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen,
                                    uint8_t RepeatedStart,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);
        
        I2CStatus_t MasterTxRx_Intr(Transaction_t* pTransaction);
    
        
    public:        
        I2CCallback_t           m_TxQueueEmptyCallback;
        I2CCallback_t           m_RxQueueFullCallback;
        I2CCallback_t           m_SlaveTxDoneCallback;
        I2CCallback_t           m_SlaveRxDoneCallback;
        
    public:
        I2CSlaveRxQueue_t   m_I2CSlaveRxQueue;
        I2CSlaveTxQueue_t   m_I2CSlaveTxQueue;
    };

}
#endif //I2CInt_h
