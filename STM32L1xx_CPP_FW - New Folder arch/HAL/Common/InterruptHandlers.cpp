
#include "InterruptManager.h"
#include"CPP_HAL.h"
#include "main.h"

namespace HAL
{

  //ISR in this file -> to user Callback = 24 CPU cucles

#define EXECUTE_ISR(IRQN)  if(InterruptManager::m_InterruptSourceArray[IRQN] != NULL)  InterruptManager::m_InterruptSourceArray[IRQN]->ISR()
#define CLEAR_IRQ(IRQN)    NVIC->ICPR[(((uint32_t)(int32_t)IRQN) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQN) & 0x1FUL))

extern "C" void WWDG_IRQHandler()
{
    EXECUTE_ISR(WWDG_IRQn);
    CLEAR_IRQ(WWDG_IRQn);
}

extern "C" void PVD_IRQHandler()
{
    EXECUTE_ISR(PVD_IRQn);
    CLEAR_IRQ(PVD_IRQn);
}

extern "C" void TAMPER_STAMP_IRQHandler()
{
    EXECUTE_ISR(TAMPER_STAMP_IRQn);
    CLEAR_IRQ(TAMPER_STAMP_IRQn);
}
extern "C" void RTC_WKUP_IRQHandler()
{
    EXECUTE_ISR(RTC_WKUP_IRQn);
    CLEAR_IRQ(RTC_WKUP_IRQn);
}

extern "C" void FLASH_IRQHandler()
{
    EXECUTE_ISR(FLASH_IRQn);
    CLEAR_IRQ(FLASH_IRQn);
}

extern "C" void RCC_IRQHandler()
{
    EXECUTE_ISR(RCC_IRQn);
    CLEAR_IRQ(RCC_IRQn);
}

extern "C" void EXTI0_IRQHandler()
{
    EXECUTE_ISR(EXTI0_IRQn);
    CLEAR_IRQ(EXTI0_IRQn);
}

extern "C" void EXTI1_IRQHandler()
{
    EXECUTE_ISR(EXTI1_IRQn);
    CLEAR_IRQ(EXTI1_IRQn);
}

extern "C" void EXTI2_IRQHandler()
{
    EXECUTE_ISR(EXTI2_IRQn);
    CLEAR_IRQ(EXTI2_IRQn);
}

extern "C" void EXTI3_IRQHandler()
{
    EXECUTE_ISR(EXTI3_IRQn);
    CLEAR_IRQ(EXTI3_IRQn);
}

extern "C" void EXTI4_IRQHandler()
{
    EXECUTE_ISR(EXTI4_IRQn);
    CLEAR_IRQ(EXTI4_IRQn);
}

extern "C" void DMA1_Channel1_IRQHandler()
{
    EXECUTE_ISR(DMA1_Channel1_IRQn);
    CLEAR_IRQ(DMA1_Channel1_IRQn);
}

extern "C" void DMA1_Channel2_IRQHandler()
{
    EXECUTE_ISR(DMA1_Channel2_IRQn);
    CLEAR_IRQ(DMA1_Channel2_IRQn);
}

extern "C" void DMA1_Channel3_IRQHandler()
{
  EXECUTE_ISR(DMA1_Channel3_IRQn);
  CLEAR_IRQ(DMA1_Channel3_IRQn);
}

extern "C" void DMA1_Channel4_IRQHandler()
{
  EXECUTE_ISR(DMA1_Channel4_IRQn);
  CLEAR_IRQ(DMA1_Channel4_IRQn);
}

extern "C" void DMA1_Channel5_IRQHandler()
{
   EXECUTE_ISR(DMA1_Channel5_IRQn);
  CLEAR_IRQ(DMA1_Channel5_IRQn);
}

extern "C" void DMA1_Channel6_IRQHandler()
{
    EXECUTE_ISR(DMA1_Channel6_IRQn);
  CLEAR_IRQ(DMA1_Channel6_IRQn);
}

extern "C" void DMA1_Channel7_IRQHandler()
{
  EXECUTE_ISR(DMA1_Channel7_IRQn);
  CLEAR_IRQ(DMA1_Channel7_IRQn);
}

extern "C" void ADC1_IRQHandler()
{
    EXECUTE_ISR(ADC1_IRQn);
    CLEAR_IRQ(ADC1_IRQn);
}

extern "C" void USB_HP_IRQHandler()
{
    EXECUTE_ISR(USB_HP_IRQn);
    CLEAR_IRQ(USB_HP_IRQn);
}

extern "C" void USB_LP_IRQHandler()
{
  //USB_LP_IRQn
#if USB_DEVICE
  extern PCD_HandleTypeDef hpcd_USB_FS;
   HAL_PCD_IRQHandler(&hpcd_USB_FS);
#endif
}

extern "C" void DAC_IRQHandler()
{
    EXECUTE_ISR(DAC_IRQn);
    CLEAR_IRQ(DAC_IRQn);
}

extern "C" void COMP_IRQHandler()
{
    EXECUTE_ISR(COMP_IRQn);
    CLEAR_IRQ(COMP_IRQn);
}

extern "C" void EXTI9_5_IRQHandler()
{
    EXECUTE_ISR(EXTI9_5_IRQn);
    CLEAR_IRQ(EXTI9_5_IRQn);
}

extern "C" void TIM9_IRQHandler()
{
    EXECUTE_ISR(TIM9_IRQn);
    CLEAR_IRQ(TIM9_IRQn);
}

extern "C" void TIM10_IRQHandler()
{
    EXECUTE_ISR(TIM10_IRQn);
    CLEAR_IRQ(TIM10_IRQn);
}

extern "C" void TIM11_IRQHandler()
{
    EXECUTE_ISR(TIM11_IRQn);
    CLEAR_IRQ(TIM11_IRQn);
}

extern "C" void TIM2_IRQHandler()
{
    EXECUTE_ISR(TIM2_IRQn);
    CLEAR_IRQ(TIM2_IRQn);
}

extern "C" void TIM3_IRQHandler()
{
    EXECUTE_ISR(TIM3_IRQn);
    CLEAR_IRQ(TIM3_IRQn);
}

extern "C" void TIM4_IRQHandler()
{
    EXECUTE_ISR(TIM4_IRQn);
    CLEAR_IRQ(TIM4_IRQn);
}
extern "C" void I2C1_EV_IRQHandler()
{
    EXECUTE_ISR(I2C1_EV_IRQn);
    CLEAR_IRQ(I2C1_EV_IRQn);
}

extern "C" void I2C1_ER_IRQHandler()
{
    EXECUTE_ISR(I2C1_ER_IRQn);
    CLEAR_IRQ(I2C1_ER_IRQn);
}

extern "C" void I2C2_EV_IRQHandler()
{
    EXECUTE_ISR(I2C2_EV_IRQn);
    CLEAR_IRQ(I2C2_EV_IRQn);
}

extern "C" void I2C2_ER_IRQHandler()
{
    EXECUTE_ISR(I2C2_ER_IRQn);
    CLEAR_IRQ(I2C2_ER_IRQn);
}

extern "C" void SPI1_IRQHandler()
{
    EXECUTE_ISR(SPI1_IRQn);
    CLEAR_IRQ(SPI1_IRQn);
}

extern "C" void SPI2_IRQHandler()
{
    EXECUTE_ISR(SPI2_IRQn);
    CLEAR_IRQ(SPI2_IRQn);
}

extern "C" void USART1_IRQHandler()
{
    EXECUTE_ISR(USART1_IRQn);
    CLEAR_IRQ(USART1_IRQn);
}

extern "C" void USART2_IRQHandler()
{
    EXECUTE_ISR(USART2_IRQn);
    CLEAR_IRQ(USART2_IRQn);
}

extern "C" void USART3_IRQHandler()
{
    EXECUTE_ISR(USART3_IRQn);
    CLEAR_IRQ(USART3_IRQn);
}

extern "C" void EXTI15_10_IRQHandler()
{
    EXECUTE_ISR(EXTI15_10_IRQn);
    CLEAR_IRQ(EXTI15_10_IRQn);
}

extern "C" void RTC_Alarm_IRQHandler()
{
    EXECUTE_ISR(RTC_Alarm_IRQn);
    CLEAR_IRQ(RTC_Alarm_IRQn);
}

extern "C" void USB_FS_WKUP_IRQHandler()
{
    EXECUTE_ISR(USB_FS_WKUP_IRQn);
    CLEAR_IRQ(USB_FS_WKUP_IRQn);
}

extern "C" void TIM6_IRQHandler()
{
    EXECUTE_ISR(TIM6_IRQn);
    CLEAR_IRQ(TIM6_IRQn);
}

extern "C" void TIM7_IRQHandler()
{
    EXECUTE_ISR(TIM7_IRQn);
    CLEAR_IRQ(TIM7_IRQn);
}

extern "C" void SysTick_Handler()
{
  
#if USB_DEVICE
    HAL_IncTick();
#endif
    
    if(InterruptManager::m_SysTick_Handler) InterruptManager::m_SysTick_Handler->ISR();
}

} // HAL