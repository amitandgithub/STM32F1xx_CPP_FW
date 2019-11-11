/*
 * Interrupt.cpp
 *
 *  Created on: 12-Oct-2017
 *      Author: Amit Chaudhary
 */

#include "Interrupt.h"
#include "core_cm3.h"
#include "stm32f1xx_ll_exti.h"

namespace HAL
{
    
__no_init Interrupt::ISR Interrupt::Vectors_RAM[TOTAL_INTERRUPT_VECTORS] @ 0x20000100;



Interrupt::Status_t Interrupt::RegisterInterrupt(IRQn eIRQn, uint8_t Priority, uint8_t SubPriority )
{
    EnableInterrupt(eIRQn);
    
    HAL_NVIC_SetPriority(static_cast<IRQn_Type>(eIRQn), Priority, SubPriority);
    
    return  HAL_STATUS_SUCCESS;
}


Interrupt::Status_t Interrupt::DisableInterrupt(IRQn eIRQn)
{
    Status_t Status = HAL_STATUS_SUCCESS;
    
    /* Disable the Selected IRQ Channels -------------------------------------*/
    NVIC->ICER[static_cast<uint8_t>(eIRQn) >> 0x05] = (uint32_t)0x01 << (static_cast<uint8_t>(eIRQn) & (uint8_t)0x1F);
    Status = HAL_STATUS_SUCCESS;

    return  Status;
}

Interrupt::Status_t Interrupt::EnableInterrupt(IRQn eIRQn)
{
    Interrupt::Status_t Status = HAL_STATUS_SUCCESS;
    
    /* Enable the Selected IRQ Channels --------------------------------------*/
    NVIC->ISER[( static_cast<uint8_t>(eIRQn) >> 0x05)] = (uint32_t)0x01 << (static_cast<uint8_t>(eIRQn) & (uint8_t)0x1F);
    Status = HAL_STATUS_SUCCESS;

    return  Status;

}
Interrupt::Status_t Interrupt::NVICConfig(IRQn eIRQn, uint8_t Priority, uint8_t SubPriority)
{
    Interrupt::Status_t Status = HAL_STATUS_SUCCESS;
    EnableInterrupt(eIRQn);
    return Status;
}


Interrupt::Status_t Interrupt::RegisterInterrupt_Vct_Table(ISR pISR, IRQn eIRQn,  uint8_t Priority , uint8_t SubPriority)
{
    if(pISR != nullptr)
    {    
        
        // NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
        EnableInterrupt(eIRQn);
        
        HAL_NVIC_SetPriority(static_cast<IRQn_Type>(eIRQn), Priority, SubPriority);
        
        Vectors_RAM[16 + eIRQn] = pISR;
    }
    
    return HAL_STATUS_SUCCESS;
}

void Interrupt::Relocate_Vector_Table()
{    
    uint32_t *Ram_Start  = (uint32_t*)Vectors_RAM;
    uint32_t *ROM_Start  = (uint32_t *)DEVICE_FLASH_START;
    
    for(uint32_t start = 0; start<= 0x3E; start++)
    {
        Ram_Start[start] = ROM_Start[start];
    }
    
    SCB->VTOR = (uint32_t)Vectors_RAM;
}

} // namespace HAL



