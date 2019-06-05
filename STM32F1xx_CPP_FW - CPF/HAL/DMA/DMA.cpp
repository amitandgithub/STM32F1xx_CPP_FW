/******************
** FILE: DMA.cpp
**
** DESCRIPTION:
**  DMA functionality Implementation
**
** CREATED: 6/5/2019, by Amit Chaudhary
******************/
#include "DMA.h"

namespace HAL
{ 
    DMA::DMACallback_t   DMA::_DMA1Callbacks[7] = {nullptr,};
    
    DMA::DMA(uint32_t dma) : _DMA1Instance(nullptr)
#if defined (DMA2)
    ,_DMA2Instance(nullptr)
#endif
    {
        if(dma == 1)
        {
            _DMAx = DMA1;
        }
#if defined (DMA2)
        else if(dma == 2)
        {
            _DMAx = DMA2;
        }
#endif
        else
        {
            while(1);
        }
    }
    
    
    DMA::~DMA()
    {
        
    }
    
    DMA::DMAStatus_t DMA::HwInit(void *pInitStruct)
    {
//        /* Set DMA_InitStruct fields to default values */
//        LL_DMA_InitTypeDef DMA_InitStruct;
//        DMA_InitStruct.PeriphOrM2MSrcAddress  = 0x00000000U;
//        DMA_InitStruct.MemoryOrM2MDstAddress  = 0x00000000U;
//        DMA_InitStruct.Direction              = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
//        DMA_InitStruct.Mode                   = LL_DMA_MODE_NORMAL;
//        DMA_InitStruct.PeriphOrM2MSrcIncMode  = LL_DMA_PERIPH_NOINCREMENT;
//        DMA_InitStruct.MemoryOrM2MDstIncMode  = LL_DMA_MEMORY_NOINCREMENT;
//        DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
//        DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
//        DMA_InitStruct.NbData                 = 0x00000000U;
//        DMA_InitStruct.Priority               = LL_DMA_PRIORITY_LOW;

        
        return 1;
       
        
    }
    
    DMA::DMAStatus_t DMA::HwDeinit()
    {
        InterruptManager::GetInstance()->RegisterDeviceInterrupt(DMA1_Channel1_IRQn,0,this);
        InterruptManager::GetInstance()->RegisterDeviceInterrupt(DMA1_Channel2_IRQn,0,this);
        InterruptManager::GetInstance()->RegisterDeviceInterrupt(DMA1_Channel3_IRQn,0,this);
        InterruptManager::GetInstance()->RegisterDeviceInterrupt(DMA1_Channel4_IRQn,0,this);
        InterruptManager::GetInstance()->RegisterDeviceInterrupt(DMA1_Channel5_IRQn,0,this);
        InterruptManager::GetInstance()->RegisterDeviceInterrupt(DMA1_Channel6_IRQn,0,this);
        InterruptManager::GetInstance()->RegisterDeviceInterrupt(DMA1_Channel7_IRQn,0,this);  
        return 0;
    }
    
    DMA::DMAStatus_t DMA:: XferConfig(DMAConfig_t DMAConfig, uint32_t Channel)
    {
       if(DMAConfig == nullptr) return 1;
       
       LL_DMA_Init(_DMAx, Channel, DMAConfig);
       return 0;
        
    }
    
    DMA::DMAStatus_t DMA:: Xfer(uint32_t Channel, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
    {
        LL_DMA_SetMemoryAddress(_DMAx, Channel, DstAddress);
        
        LL_DMA_SetPeriphAddress(_DMAx, Channel, SrcAddress);
        
        LL_DMA_SetDataLength(_DMAx, Channel, DataLength); 
        return 0;
    }
    
    void DMA:: RegisterCallback(uint32_t Channel, DMACallback_t ChannelCB)
    {
       _DMA1Callbacks[Channel%7] = ChannelCB;
    }
    
    void DMA:: UnRegisterCallback(uint32_t Channel)
    {
       _DMA1Callbacks[Channel%7] = nullptr;
    }
    
    void DMA::ISR( IRQn_Type event ) // EXTI4_IRQn
    {
        
    }
    
    
    
    
    
    
}
