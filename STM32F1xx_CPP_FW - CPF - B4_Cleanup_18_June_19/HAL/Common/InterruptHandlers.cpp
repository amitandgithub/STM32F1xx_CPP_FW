////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file InterruptCallbacks.cpp
///
/// File to contain all the extern interrupt callbacks for S3 processor
///
/// Note that in order to link the symbols listed in the startup file vector table to the functions here, the
/// functions are declared with external C linkage so as to avoid a mismatch due to name mangling.

// SYSTEM INCLUDES
#include <stdint.h>

// C PROJECT INCLUDES
// (none)

// C++ PROJECT INCLUDES
#include "InterruptManager.h"
#include "DMA.h"

namespace HAL
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FUNCTION NAME: Nvic_Handler
///
/// The interrupt service routine common calls.
///
/// @par Full Description
/// The interrupt service routine common routine called from ISRs. This function calls the InterruptSource pointer
/// located in InterruptManager class
///
/// @param type The IRQ number that was called
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Nvic_Handler( InterruptManager::IRQn_t IRQNumber )
{
    //ASSERT_CRITICAL_IF(InterruptManager::m_IrqConfigs[type].m_pInterruptSource == NULL, "Null Pointer");

    if(InterruptManager::_InterruptSourceArray[IRQNumber] != NULL)
    {
        InterruptManager::_InterruptSourceArray[IRQNumber]->ISR(IRQNumber);
    }

    // Clear the interrupt
    InterruptManager::GetInstance()->ClearInterrupt(IRQNumber);
}


extern "C" void WWDG_IRQHandler()
{
    Nvic_Handler(WWDG_IRQn);
}

extern "C" void PVD_IRQHandler()
{
    Nvic_Handler(PVD_IRQn);
}

extern "C" void TAMPER_IRQHandler()
{
    Nvic_Handler(TAMPER_IRQn);
}
extern "C" void RTC_IRQHandler()
{
    Nvic_Handler(RTC_IRQn);
}

extern "C" void FLASH_IRQHandler()
{
    Nvic_Handler(FLASH_IRQn);
}

extern "C" void RCC_IRQHandler()
{
    Nvic_Handler(RCC_IRQn);
}

extern "C" void EXTI0_IRQHandler()
{
    Nvic_Handler(EXTI0_IRQn);
}

extern "C" void EXTI1_IRQHandler()
{
    Nvic_Handler(EXTI1_IRQn);
}

extern "C" void EXTI2_IRQHandler()
{
    Nvic_Handler(EXTI2_IRQn);
}

extern "C" void EXTI3_IRQHandler()
{
    Nvic_Handler(EXTI3_IRQn);
}

extern "C" void EXTI4_IRQHandler()
{
    Nvic_Handler(EXTI4_IRQn);
}

extern "C" void DMA1_Channel1_IRQHandler()
{
    //Nvic_Handler(DMA1_Channel1_IRQn);
     if(DMA::_DMA1Callbacks[0]) 
         DMA::_DMA1Callbacks[0]->CallbackFunction();
     
     NVIC_ClearPendingIRQ(DMA1_Channel1_IRQn); 
}

extern "C" void DMA1_Channel2_IRQHandler()
{
    //Nvic_Handler(DMA1_Channel2_IRQn);
     if(DMA::_DMA1Callbacks[1]) DMA::_DMA1Callbacks[1]->CallbackFunction();
     
     NVIC_ClearPendingIRQ(DMA1_Channel2_IRQn); 
}

extern "C" void DMA1_Channel3_IRQHandler()
{
   // Nvic_Handler(DMA1_Channel3_IRQn);
     if(DMA::_DMA1Callbacks[2]) DMA::_DMA1Callbacks[2]->CallbackFunction();
     
     NVIC_ClearPendingIRQ(DMA1_Channel3_IRQn); 
}

extern "C" void DMA1_Channel4_IRQHandler()
{
    //Nvic_Handler(DMA1_Channel4_IRQn);
     if(DMA::_DMA1Callbacks[3]) DMA::_DMA1Callbacks[3]->CallbackFunction();
     
     NVIC_ClearPendingIRQ(DMA1_Channel4_IRQn); 
}

extern "C" void DMA1_Channel5_IRQHandler()
{
   // Nvic_Handler(DMA1_Channel5_IRQn);
     if(DMA::_DMA1Callbacks[4]) DMA::_DMA1Callbacks[4]->CallbackFunction();
     
     NVIC_ClearPendingIRQ(DMA1_Channel5_IRQn); 
}

extern "C" void DMA1_Channel6_IRQHandler()
{
    //Nvic_Handler(DMA1_Channel6_IRQn);
     if(DMA::_DMA1Callbacks[5]) DMA::_DMA1Callbacks[5]->CallbackFunction();
     
     NVIC_ClearPendingIRQ(DMA1_Channel6_IRQn); 
}

extern "C" void DMA1_Channel7_IRQHandler()
{
   // Nvic_Handler(DMA1_Channel7_IRQn);
    if(DMA::_DMA1Callbacks[6]) DMA::_DMA1Callbacks[6]->CallbackFunction();
    
    NVIC_ClearPendingIRQ(DMA1_Channel7_IRQn); 
}

extern "C" void ADC1_2_IRQHandler()
{
    Nvic_Handler(ADC1_2_IRQn);
}

extern "C" void USB_HP_CAN1_TX_IRQHandler()
{
    Nvic_Handler(USB_HP_CAN1_TX_IRQn);
}

extern "C" void USB_LP_CAN1_RX0_IRQHandler()
{
    Nvic_Handler(USB_LP_CAN1_RX0_IRQn);
}

extern "C" void CAN1_RX1_IRQHandler()
{
    Nvic_Handler(CAN1_RX1_IRQn);
}

extern "C" void CAN1_SCE_IRQHandler()
{
    Nvic_Handler(CAN1_SCE_IRQn);
}

extern "C" void EXTI9_5_IRQHandler()
{
    Nvic_Handler(EXTI9_5_IRQn);
}

extern "C" void TIM1_BRK_IRQHandler()
{
    Nvic_Handler(TIM1_BRK_IRQn);
}

extern "C" void TIM1_UP_IRQHandler()
{
    Nvic_Handler(TIM1_UP_IRQn);
}

extern "C" void TIM1_TRG_COM_IRQHandler()
{
    Nvic_Handler(TIM1_TRG_COM_IRQn);
}

extern "C" void TIM1_CC_IRQHandler()
{
    Nvic_Handler(TIM1_CC_IRQn);
}

extern "C" void TIM2_IRQHandler()
{
    Nvic_Handler(TIM2_IRQn);
}

extern "C" void TIM3_IRQHandler()
{
    Nvic_Handler(TIM3_IRQn);
}

extern "C" void TIM4_IRQHandler()
{
    Nvic_Handler(TIM4_IRQn);
}
extern "C" void I2C1_EV_IRQHandler()
{
    Nvic_Handler(I2C1_EV_IRQn);
}

extern "C" void I2C1_ER_IRQHandler()
{
    Nvic_Handler(I2C1_ER_IRQn);
}

extern "C" void I2C2_EV_IRQHandler()
{
    Nvic_Handler(I2C2_EV_IRQn);
}

extern "C" void I2C2_ER_IRQHandler()
{
    Nvic_Handler(I2C2_ER_IRQn);
}

extern "C" void SPI1_IRQHandler()
{
    Nvic_Handler(SPI1_IRQn);
}

extern "C" void SPI2_IRQHandler()
{
    Nvic_Handler(SPI2_IRQn);
}

extern "C" void USART1_IRQHandler()
{
    Nvic_Handler(USART1_IRQn);
}

extern "C" void USART2_IRQHandler()
{
    Nvic_Handler(USART2_IRQn);
}

extern "C" void USART3_IRQHandler()
{
    Nvic_Handler(USART3_IRQn);
}

extern "C" void EXTI15_10_IRQHandler()
{
    Nvic_Handler(EXTI15_10_IRQn);
}

extern "C" void RTC_Alarm_IRQHandler()
{
    Nvic_Handler(RTC_Alarm_IRQn);
}

extern "C" void USBWakeUp_IRQHandler()
{
    Nvic_Handler(USBWakeUp_IRQn);
}

} // HAL