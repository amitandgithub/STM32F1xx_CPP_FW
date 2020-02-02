/*
 * Interrupt.h
 *
 *  Created on: 12-Oct-2017
 *      Author: Amit Chaudhary
 */

#ifndef APP_INC_INTERRUPTMANAGER_HPP_
#define APP_INC_INTERRUPTMANAGER_HPP_

#include "stm32f1xx_hal.h"
#include "CPP_HAL.h"

namespace HAL
{


class Interrupt
{

public:
    static const uint32_t DEVICE_FLASH_START = 0x08000000;
    static const uint32_t TOTAL_INTERRUPT_VECTORS = 64;
    typedef HAL::HAL_Status_t Status_t;
	typedef void(*ISR)(void);
	typedef enum {DISABLE = 0, ENABLE = !DISABLE}InterruptState;
	typedef enum
	{
        NMI_Handler = -14,
	    HardFault_Handler = -13,
	    MemManage_Handler = -12,
	    BusFault_Handler = -11,
	    UsageFault_Handler = -10,
//        _Rsvd1 = -9,                         
//		_Rsvd2 = -8,                        
//		_Rsvd3 = -7,                       
//		_Rsvd4 = -6, 
	    SVC_Handler = -5,
	    DebugMon_Handler = -4,
//        _Rsvd5 = -3,
	    PendSV_Handler = -2,
	    SysTick_Handler = -1,
            WWDG_IRQHandler = 0,
            PVD_IRQHandler = 1,
            TAMPER_IRQHandler = 2,
            RTC_IRQHandler = 3,
            FLASH_IRQHandler = 4,
            RCC_IRQHandler = 5,
            EXTI0_IRQHandler = 6,
            EXTI1_IRQHandler = 7,
            EXTI2_IRQHandler = 8,
            EXTI3_IRQHandler = 9,
            EXTI4_IRQHandler = 10,
            DMA1_Channel1_IRQHandler = 11,
            DMA1_Channel2_IRQHandler = 12,
            DMA1_Channel3_IRQHandler = 13,
            DMA1_Channel4_IRQHandler = 14,
            DMA1_Channel5_IRQHandler = 15,
            DMA1_Channel6_IRQHandler = 16,
            DMA1_Channel7_IRQHandler = 17,
            ADC1_2_IRQHandler = 18,
            USB_HP_CAN1_TX_IRQHandler = 19,
            USB_LP_CAN1_RX0_IRQHandler = 20,
            CAN1_RX1_IRQHandler = 21,
            CAN1_SCE_IRQHandler = 22,
            EXTI9_5_IRQHandler = 23,
            TIM1_BRK_IRQHandler = 24,
            TIM1_UP_IRQHandler = 25,
            TIM1_TRG_COM_IRQHandler = 26,
            TIM1_CC_IRQHandler = 27,
            TIM2_IRQHandler = 28,
            TIM3_IRQHandler = 29,
            TIM4_IRQHandler = 30,
            I2C1_EV_IRQHandler = 31,
            I2C1_ER_IRQHandler = 32,
            I2C2_EV_IRQHandler = 33,
            I2C2_ER_IRQHandler = 34,
            SPI1_IRQHandler = 35,
            SPI2_IRQHandler = 36,
            USART1_IRQHandler = 37,
            USART2_IRQHandler = 38,
            USART3_IRQHandler = 39,
            EXTI15_10_IRQHandler = 40,
            RTCAlarm_IRQHandler = 41,
            USBWakeUp_IRQHandler = 42,
            EXTI5_IRQHandler = 43,
            EXTI6_IRQHandler = 44,
            EXTI7_IRQHandler = 45,
            EXTI8_IRQHandler = 46,
            EXTI9_IRQHandler = 47,
            EXTI10_IRQHandler = 48,
            EXTI11_IRQHandler = 49,
            EXTI12_IRQHandler = 50,
            EXTI13_IRQHandler = 51,
            EXTI14_IRQHandler = 52,
            EXTI15_IRQHandler = 53
	}IRQn;
    
   
    Interrupt(){};
    
    ~Interrupt(){};
    
    static void Relocate_Vector_Table();
     
	static Status_t RegisterInterrupt(IRQn eIRQn,  uint8_t Priority = 15, uint8_t SubPriority = 0 );
    
    static Status_t RegisterInterrupt_Vct_Table(ISR pISR, IRQn eIRQn,  uint8_t Priority = 15, uint8_t SubPriority = 0 );
    
	static Status_t EnableInterrupt(IRQn eIRQn);//{NVIC_EnableIRQ( eIRQn);}
    
	static Status_t DisableInterrupt(IRQn eIRQn);
    
	static Status_t NVICConfig(IRQn eIRQn, uint8_t Priority, uint8_t SubPriority);
    
    static Status_t GetInterruptStatus(IRQn eIRQn);
    
    static Status_t SetInterruptPriority(IRQn eIRQn,uint8_t Priority, uint8_t SubPriority);
    
    static Status_t GetInterruptPriority(IRQn eIRQn,uint8_t* pPriority, uint8_t* pSubPriority);
    
    static ISR GetISR(IRQn eIRQn){return Vectors_RAM[ eIRQn % TOTAL_INTERRUPT_VECTORS ]; }
    
private: 
    static ISR Vectors_RAM[TOTAL_INTERRUPT_VECTORS];

};






























}
#endif /* APP_INC_INTERRUPTMANAGER_HPP_ */
