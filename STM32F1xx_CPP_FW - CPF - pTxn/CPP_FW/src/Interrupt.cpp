/*
 * Interrupt.cpp
 *
 *  Created on: 12-Oct-2017
 *      Author: Amit Chaudhary
 */

#include "Interrupt.hpp"
#include "core_cm3.h"
#include "stm32f1xx_ll_exti.h"

namespace Peripherals
{
    
//__no_init Interrupt::ISR Vectors_RAM[64] @ 0x20000100;
    
__no_init Interrupt::ISR Interrupt::Vectors_RAM[TOTAL_INTERRUPT_VECTORS] @ 0x20000100;

typedef uint8_t u8;


Status_t Interrupt::RegisterInterrupt(IRQn eIRQn, uint8_t Priority, uint8_t SubPriority )
{
    EnableInterrupt(eIRQn);
    
    HAL_NVIC_SetPriority(static_cast<IRQn_Type>(eIRQn), Priority, SubPriority);
    
    return  true;
}


Status_t Interrupt::DisableInterrupt(IRQn eIRQn)
{
    Status_t Status = false;
//    if( (eIRQn>= InterruptManager::EXTI5_IRQHandler ) && (eIRQn<= InterruptManager::EXTI9_IRQHandler )  )
//        eIRQn = InterruptManager::EXTI9_5_IRQHandler;
//	else if( (eIRQn>= InterruptManager::EXTI10_IRQHandler ) && (eIRQn<= InterruptManager::EXTI15_IRQHandler )  )
//        eIRQn = InterruptManager::EXTI15_10_IRQHandler;
    
    /* Disable the Selected IRQ Channels -------------------------------------*/
    NVIC->ICER[static_cast<u8>(eIRQn) >> 0x05] = (uint32_t)0x01 << (static_cast<u8>(eIRQn) & (u8)0x1F);
    Status = true;

    return  Status;
}

Status_t Interrupt::EnableInterrupt(IRQn eIRQn)
{
    Status_t Status = false;
    
//    if( (eIRQn>= InterruptManager::EXTI5_IRQHandler ) && (eIRQn<= InterruptManager::EXTI9_IRQHandler )  )
//        eIRQn = InterruptManager::EXTI9_5_IRQHandler;
//    else if( (eIRQn>= InterruptManager::EXTI10_IRQHandler ) && (eIRQn<= InterruptManager::EXTI15_IRQHandler )  )
//        eIRQn = InterruptManager::EXTI15_10_IRQHandler;
    
    /* Enable the Selected IRQ Channels --------------------------------------*/
    NVIC->ISER[( static_cast<u8>(eIRQn) >> 0x05)] = (uint32_t)0x01 << (static_cast<u8>(eIRQn) & (u8)0x1F);
    Status = true;

    return  Status;

}
Status_t Interrupt::NVICConfig(IRQn eIRQn, u8 Priority, u8 SubPriority)
{
    Status_t Status = false;
    EnableInterrupt(eIRQn);
    return Status;
}


Status_t Interrupt::RegisterInterrupt_Vct_Table(ISR pISR, IRQn eIRQn,  uint8_t Priority , uint8_t SubPriority)
{
    if(pISR != nullptr)
    {    
        
        // NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
        EnableInterrupt(eIRQn);
        
        HAL_NVIC_SetPriority(static_cast<IRQn_Type>(eIRQn), Priority, SubPriority);
        
        Vectors_RAM[16 + eIRQn] = pISR;
    }
    
    return true;
}

void Interrupt::Relocate_Vector_Table()
{
    
    uint32_t *Ram_Start  = (uint32_t*)Vectors_RAM;
    uint32_t *ROM_Start = (uint32_t*)0x08000000;
    
    for(uint32_t start = 0; start<= 0x3E; start++)
    {
        Ram_Start[start] = ROM_Start[start];
    }
    
    SCB->VTOR = (uint32_t)Vectors_RAM;// | (0U & (uint32_t)0x1FFFFF80);
}

} // namespace Peripherals



