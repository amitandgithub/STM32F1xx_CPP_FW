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
    
    DMA* DMA::_DMA1Instance;
#if defined (DMA2)
    DMA* DMA::_DMA2Instance(nullptr)
#endif
    
    DMA::DMA(uint32_t dma) 
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
    
    void DMA::ClockEnable()
    {
        if( _DMAx == DMA1)
        {
            LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
        }
#if defined (DMA2)
        else if( _DMAx == DMA2)
        {
            LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
        }
#endif        
        
    }
    
    void DMA::ClockDisable()
    {
        if( _DMAx == DMA1)
        {
            LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA1);
        }
#if defined (DMA2)
        else if( _DMAx == DMA2)
        {
            LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA2);
        }
#endif        
        
    }
    
    
    
    DMA::DMAStatus_t DMA::HwInit(void *pInitStruct)
    {        
        ClockEnable();
        return 1;
    }
    
    DMA::DMAStatus_t DMA::HwDeinit()
    {
        return 0;
    }
    
    DMA::DMAStatus_t DMA:: XferConfig(DMAConfig_t* DMAConfig, uint32_t Channel)
    {
       if(DMAConfig == nullptr) return 1;
       
       LL_DMA_DisableChannel(_DMAx, Channel);
       
       LL_DMA_Init(_DMAx, Channel, DMAConfig);
       
//       LL_DMA_ConfigTransfer(_DMAx, Channel, DMAConfig->Direction | \
//                        DMAConfig->Mode                   | \
//                        DMAConfig->PeriphOrM2MSrcIncMode  | \
//                        DMAConfig->MemoryOrM2MDstIncMode  | \
//                        DMAConfig->PeriphOrM2MSrcDataSize | \
//                        DMAConfig->MemoryOrM2MDstDataSize | \
//                        DMAConfig->Priority);
       
       LL_DMA_EnableChannel(_DMAx, Channel);
       
       return 0;
        
    }
    
    DMA::DMAStatus_t DMA:: Load(uint32_t Channel, uint32_t PeripheralAddress, uint32_t MemoryAddress, uint32_t DataLength, uint32_t DataDirection )
    {
        LL_DMA_DisableChannel(_DMAx, Channel);
        
        LL_DMA_SetMemoryAddress(_DMAx, Channel, MemoryAddress);
        
        LL_DMA_SetPeriphAddress(_DMAx, Channel, PeripheralAddress);
        
        LL_DMA_SetDataLength(_DMAx, Channel, DataLength); 

		LL_DMA_SetDataTransferDirection(_DMAx, Channel, DataDirection);
        
        LL_DMA_EnableChannel(_DMAx, Channel);
        return 0;
    }
    
    void DMA:: RegisterCallback(uint32_t Channel, DMACallback_t ChannelCB)
    {
       _DMA1Callbacks[(Channel-1)%7] = ChannelCB;
    }
    
    void DMA:: UnRegisterCallback(uint32_t Channel)
    {
       _DMA1Callbacks[(Channel-1)%7] = nullptr;
    }
    
    void DMA::SetDataLen(uint32_t Channel, uint32_t Len)
    {
      LL_DMA_DisableChannel(_DMAx, Channel);
      LL_DMA_SetDataLength(_DMAx,Channel,Len);
      LL_DMA_EnableChannel(_DMAx, Channel);
    } 
        
    uint32_t DMA::GetDataLen(uint32_t Channel)
    {
      uint32_t len;
      LL_DMA_DisableChannel(_DMAx, Channel);
      len = LL_DMA_GetDataLength(_DMAx,Channel);
      LL_DMA_EnableChannel(_DMAx, Channel);
      return len;
    } 
    
    
    
}
