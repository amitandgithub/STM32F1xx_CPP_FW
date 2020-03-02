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
        
        DMAStatus_t Load(uint32_t Channel, uint32_t PeripheralAddress, uint32_t MemoryAddress, uint32_t DataLength, uint32_t DataDirection,uint32_t width = LL_DMA_PDATAALIGN_BYTE);
        
        void EnableHalfTransferCompleteInterrupt(uint32_t Channel){LL_DMA_EnableIT_HT(m_DMAx, Channel);}
        
        void EnableTransferCompleteInterrupt(uint32_t Channel){LL_DMA_EnableIT_TC(m_DMAx, Channel);}
        
        void EnableTransferErrorInterrupt(uint32_t Channel){LL_DMA_EnableIT_TE(m_DMAx, Channel);}
        
        void DisableHalfTransferCompleteInterrupt(uint32_t Channel){LL_DMA_DisableIT_HT(m_DMAx, Channel);}
        
        void DisableTransferCompleteInterrupt(uint32_t Channel){LL_DMA_DisableIT_TC(m_DMAx, Channel);}
        
        void DisableTransferErrorInterrupt(uint32_t Channel){LL_DMA_DisableIT_TE(m_DMAx, Channel);}
        
        void DisableInterrupt(uint32_t Channel, uint32_t flags){CLEAR_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)m_DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, flags);}
        
        void EnableInterrupt(uint32_t Channel, uint32_t flags){SET_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)m_DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, flags);}
        
        void SetDataLen(uint32_t Channel, uint32_t Len);
        
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