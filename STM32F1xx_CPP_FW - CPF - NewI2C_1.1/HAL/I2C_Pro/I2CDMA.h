/******************
** CLASS: I2CDMA
**
** DESCRIPTION:
**  Implements the I2C in DMA mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
**
** FILE: I2CDMA.h
**
******************/
#ifndef I2CDMA_h
#define I2CDMA_h

#include"I2CInt.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Queue.h"
#include"DMA.h"


namespace HAL
{    
    
#define _DMA DMA::GetInstance(1)
    
    class I2CDMA : public I2CInt
    {
    public:        
        static const uint32_t I2C1_RX_DMA_CHANNEL = 7;
        static const uint32_t I2C1_TX_DMA_CHANNEL = 6;
        static const uint32_t I2C2_RX_DMA_CHANNEL = 5;
        static const uint32_t I2C2_TX_DMA_CHANNEL = 4;      
        
        I2CDMA(Pin_t scl, Pin_t sda, Hz_t Hz = 100000U);
        
        ~I2CDMA(){};
        
        I2CStatus_t HwInit(void *pInitStruct = nullptr);
        
        I2CStatus_t HwDeinit();
        
        void DMA_Tx_Done_Handler();
        
        void DMA_Rx_Done_Handler();
        
        I2CStatus_t MasterTx_DMA(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);
        
        I2CStatus_t MasterRx_DMA(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);        
        
        I2CStatus_t MasterTxRx_DMA(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen,
                               uint8_t RepeatedStart,I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);
        
        I2CStatus_t MasterTxRx_DMA(Transaction_t* pTransaction);
		
		void I2C1_DMA_Tx_Done_Handler();

		void I2C1_DMA_Rx_Done_Handler();
        
    class I2C1_DMA_Rx_Callback : public Callback
    {
    public:
        I2C1_DMA_Rx_Callback(I2CDMA* This):m_This(This){};
        virtual void CallbackFunction();
    private:
        I2CDMA* m_This;
    };
    
    class I2C1_DMA_Tx_Callback : public Callback
    {
    public:
        I2C1_DMA_Tx_Callback(I2CDMA* This):m_This(This){};
        virtual void CallbackFunction();
    private:
        I2CDMA* m_This;
    };
    
    class I2C2_DMA_Rx_Callback : public Callback
    {
    public:
        I2C2_DMA_Rx_Callback(I2CDMA* This):m_This(This){};
        virtual void CallbackFunction();
    private:
        I2CDMA* m_This;
    };
    
    class I2C2_DMA_Tx_Callback : public Callback
    {
    public:
        I2C2_DMA_Tx_Callback(I2CDMA* This):m_This(This){};
        virtual void CallbackFunction();
    private:
        I2CDMA* m_This;
    };
    
        
    private:                   
        I2C1_DMA_Rx_Callback m_I2C1_DMA_Rx_Callback;
        I2C1_DMA_Tx_Callback m_I2C1_DMA_Tx_Callback;
        I2C2_DMA_Rx_Callback m_I2C2_DMA_Rx_Callback;
        I2C2_DMA_Tx_Callback m_I2C2_DMA_Tx_Callback;
    };

}
#endif //I2CDMA_h
