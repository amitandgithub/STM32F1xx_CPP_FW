/******************
** FILE: InterruptManager.cpp
**
** DESCRIPTION:
**  InterruptManager class functionality
**
** CREATED: 3/30/2019, by Amit Chaudhary
******************/

#include "InterruptManager.h"

namespace HAL
{

InterruptSource*    InterruptManager::m_InterruptSourceArray[TOTAL_INTERRUPT_VECTORS];    
    
bool InterruptManager::RegisterDeviceInterrupt( IRQn_t IRQNumber, uint32_t priority, InterruptSource* const pInterruptSource )
{    
    if(pInterruptSource)
    {
        m_InterruptSourceArray[IRQNumber] = pInterruptSource;
        
        /* Enable the Selected IRQ Channels */
        NVIC->ISER[( static_cast<uint8_t>(IRQNumber) >> 0x05)] = (uint32_t)0x01 << (static_cast<uint8_t>(IRQNumber) & (uint8_t)0x1F);
        
        NVIC_SetPriority(IRQNumber, priority);
        
        return true;
    }    
    return false;
}

bool InterruptManager::UnRegisterDeviceInterrupt(IRQn_t IRQNumber)
{    
    m_InterruptSourceArray[IRQNumber] = nullptr;
    
    /* Disable the Selected IRQ Channels -------------------------------------*/
    NVIC->ICER[static_cast<uint8_t>(IRQNumber) >> 0x05] = (uint32_t)0x01 << (static_cast<uint8_t>(IRQNumber) & (uint8_t)0x1F);
    
    return true;

}

InterruptManager::Status_t InterruptManager::EnableInterrupt(IRQn_t IRQNumber)
{
    InterruptManager::Status_t Status = HAL_STATUS_SUCCESS;
    
    /* Enable the Selected IRQ Channels --------------------------------------*/
    NVIC->ISER[( static_cast<uint8_t>(IRQNumber) >> 0x05)] = (uint32_t)0x01 << (static_cast<uint8_t>(IRQNumber) & (uint8_t)0x1F);
    Status = HAL_STATUS_SUCCESS;

    return  Status;
}   

}