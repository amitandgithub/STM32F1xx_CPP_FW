/******************
** CLASS: DMA
**
** DESCRIPTION:
**  Dma functionality implementation
**
** CREATED: 6/5/2019, by Amit Chaudhary
**
** FILE: DMA.h
**
******************/
#ifndef DMA_h
#define DMA_h

#include"chip_peripheral.h"
#include"InterruptManager.h"
#include"Callback.h"



namespace HAL
{      
    class DMA 
    {
    public:
        
        typedef LL_DMA_InitTypeDef DMAConfig_t;
        typedef uint32_t DMAStatus_t;
        typedef uint32_t DMAState_t;
        typedef Callback* DMACallback_t;
        
//        static constexpr uint32_t I2C1_DMA_TX_CCR          = DMA1_BASE + 8 + (I2C1_TX_DMA_CHANNEL-1) * 20; //0x6C
//        static constexpr uint32_t I2C1_DMA_RX_CCR          = DMA1_BASE + 8 + (I2C1_RX_DMA_CHANNEL-1) * 20; //0x80;
//        static constexpr uint32_t I2C1_DMA_TX_CNDTR        = I2C1_DMA_TX_CCR + 4; // 0x70;
//        static constexpr uint32_t I2C1_DMA_RX_CNDTR        = I2C1_DMA_RX_CCR + 4; // 0x84;
//        
//        static constexpr uint32_t I2C2_DMA_TX_CCR          = DMA1_BASE + 8 + (I2C2_TX_DMA_CHANNEL-1) * 20; // 0x44;
//        static constexpr uint32_t I2C2_DMA_RX_CCR          = DMA1_BASE + 8 + (I2C2_RX_DMA_CHANNEL-1) * 20; //0x58;
//        static constexpr uint32_t I2C2_DMA_TX_CNDTR        = I2C2_DMA_TX_CCR + 4; // 0x48;
//        static constexpr uint32_t I2C2_DMA_RX_CNDTR        = I2C2_DMA_RX_CCR + 4; //0x5C;
    
        enum
        {
          DMA_GL_FLAG = 0x00000001U,
          DMA_TC_FLAG = 0x00000002U,
          DMA_HC_FLAG = 0x00000004U,
          DMA_TE_FLAG = 0x00000008U
        };
        
        DMA(uint32_t dma);    
        
        ~DMA();
        
        DMAStatus_t HwInit(void *pInitStruct = nullptr);
        
        DMAStatus_t HwDeinit();
        
        void ClockEnable();
        
        void ClockDisable();
        
        DMAStatus_t XferConfig(DMAConfig_t* DMAConfig, uint32_t Channel);
        
        void Load(uint32_t Channel, uint32_t PeripheralAddress, uint32_t MemoryAddress, uint32_t DataLength, uint32_t DataDirection,uint32_t width);
        
        void ChanelHwInit(uint32_t Channel,uint32_t PeripheralAddress, uint32_t Config);
        
        void Load(uint32_t Channel, uint32_t MemoryAddress, uint16_t Len);
        
        void Load(uint32_t Channel, uint32_t Len);
          
        void EnableHalfTransferCompleteInterrupt(uint32_t Channel){LL_DMA_EnableIT_HT(m_DMAx, Channel);}
        
        void EnableTransferCompleteInterrupt(uint32_t Channel){LL_DMA_EnableIT_TC(m_DMAx, Channel);}
        
        void EnableTransferErrorInterrupt(uint32_t Channel){LL_DMA_EnableIT_TE(m_DMAx, Channel);}
        
        void DisableHalfTransferCompleteInterrupt(uint32_t Channel){LL_DMA_DisableIT_HT(m_DMAx, Channel);}
        
        void DisableTransferCompleteInterrupt(uint32_t Channel){LL_DMA_DisableIT_TC(m_DMAx, Channel);}
        
        void DisableTransferErrorInterrupt(uint32_t Channel){LL_DMA_DisableIT_TE(m_DMAx, Channel);}
        
        void DisableInterrupt(uint32_t Channel, uint32_t flags){CLEAR_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)m_DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, flags);}
        
        void EnableInterrupt(uint32_t Channel, uint32_t flags){SET_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)m_DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, flags);}
        
        void ClearFlag(uint32_t Channel, uint32_t flags);
        
        bool GetFlag(uint32_t Channel, uint32_t flags);
        
        uint32_t GetDataLen(uint32_t Channel);
        
        inline void Start();
        
        inline void Abort();

        DMA_TypeDef*        m_DMAx;            
    };
    
    inline void DMA::Start()
    {
        
    }
    
    inline void DMA::Abort()
    {
        
    }   
    
}
#endif //DMA_h