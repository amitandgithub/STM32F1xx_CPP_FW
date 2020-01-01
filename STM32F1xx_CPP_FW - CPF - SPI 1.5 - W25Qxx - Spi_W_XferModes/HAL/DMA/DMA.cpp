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
    
    DMA::DMA(uint32_t dma) 
    {
        if(dma == 1)
        {
            m_DMAx = DMA1;
        }
#if defined (DMA2)
        else if(dma == 2)
        {
            m_DMAx = DMA2;
        }
#endif
    }
    
    
    DMA::~DMA()
    {

    }
    
    void DMA::ClockEnable()
    {
        if( m_DMAx == DMA1)
        {
            LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
        }
#if defined (DMA2)
        else if( m_DMAx == DMA2)
        {
            LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
        }
#endif        
        
    }
    
    void DMA::ClockDisable()
    {
        if( m_DMAx == DMA1)
        {
            LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA1);
        }
#if defined (DMA2)
        else if( m_DMAx == DMA2)
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
       
       LL_DMA_DisableChannel(m_DMAx, Channel);
       
       LL_DMA_Init(m_DMAx, Channel, DMAConfig);
       
//       LL_DMA_ConfigTransfer(m_DMAx, Channel, DMAConfig->Direction | \
//                        DMAConfig->Mode                   | \
//                        DMAConfig->PeriphOrM2MSrcIncMode  | \
//                        DMAConfig->MemoryOrM2MDstIncMode  | \
//                        DMAConfig->PeriphOrM2MSrcDataSize | \
//                        DMAConfig->MemoryOrM2MDstDataSize | \
//                        DMAConfig->Priority);
       
       LL_DMA_EnableChannel(m_DMAx, Channel);
       
       return 0;
        
    }
    
    DMA::DMAStatus_t DMA:: Load(uint32_t Channel, uint32_t PeripheralAddress, uint32_t MemoryAddress, uint32_t DataLength, uint32_t DataDirection )
    {
        LL_DMA_DisableChannel(m_DMAx, Channel);
        
        LL_DMA_SetMemoryAddress(m_DMAx, Channel, MemoryAddress);
        
        LL_DMA_SetPeriphAddress(m_DMAx, Channel, PeripheralAddress);
        
        LL_DMA_SetDataLength(m_DMAx, Channel, DataLength); 

        LL_DMA_SetDataTransferDirection(m_DMAx, Channel, DataDirection);
        
        LL_DMA_EnableChannel(m_DMAx, Channel);
        return 0;
    }
    
    void DMA::SetDataLen(uint32_t Channel, uint32_t Len)
    {
      LL_DMA_DisableChannel(m_DMAx, Channel);
      LL_DMA_SetDataLength(m_DMAx,Channel,Len);
      LL_DMA_EnableChannel(m_DMAx, Channel);
    } 
        
    uint32_t DMA::GetDataLen(uint32_t Channel)
    {
      uint32_t len;
      LL_DMA_DisableChannel(m_DMAx, Channel);
      len = LL_DMA_GetDataLength(m_DMAx,Channel);
      LL_DMA_EnableChannel(m_DMAx, Channel);
      return len;
    } 
    
    
    
}
