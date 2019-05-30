/*
 * GpioInput.hpp
 *
 *  Created on: 08-Oct-2017
 *      Author: Amit Chaudhary
 */

#ifndef APP_INC_GPIOINPUT_HPP_
#define APP_INC_GPIOINPUT_HPP_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_ll_exti.h"
#include "Peripheral.hpp"
#include "Interrupt.hpp"

namespace Peripherals
{

class GpioInput : public Peripheral, public Interrupt 
{
public:
    
     typedef enum 
        {
            INPUT                =	0x00000000U,   				/*!< Input Floating Mode                   */
            OUTPUT_PP            =	0x00000001U,   				/*!< Output Push Pull Mode                 */
            OUTPUT_OD            =	0x00000011U,   				/*!< Output Open Drain Mode                */
            AF_PP                =	0x00000002U,   				/*!< Alternate Function Push Pull Mode     */
            AF_OD                =	0x00000012U,   				/*!< Alternate Function Open Drain Mode    */
            AF_INPUT             =	0x00000000U,                /*!< Alternate Function Input Mode         */
            ANALOG               =	0x00000003U   				/*!< Analog Mode  */
        }IOMODE_t;
        
        typedef enum
        {
            IT_RISING            =	0x10110000U,   				/*!< External Interrupt Mode with Rising edge trigger detection          */
            IT_FALLING           =	0x10210000U,   				/*!< External Interrupt Mode with Falling edge trigger detection         */
            IT_RISING_FALLING    =	0x10310000U,   				/*!< External Interrupt Mode with Rising/Falling edge trigger detection  */
            EVT_RISING           =	0x10120000U,   				/*!< External Event Mode with Rising edge trigger detection               */
            EVT_FALLING          =	0x10220000U,   				/*!< External Event Mode with Falling edge trigger detection              */
            EVT_RISING_FALLING   =	0x10320000U,   				/*!< External Event Mode with Rising/Falling edge trigger detection       */
            NO_INT_NO_EVT        =  0x00000000U
        }INTR_EVT_ON_EDGE_t;
        
        typedef GPIO_TypeDef*     PORT_t;
        typedef uint16_t          PIN_t;
        typedef Interrupt::ISR    ISR_t;
        
        typedef enum
        {
            EXTI_NONE                        = LL_EXTI_TRIGGER_NONE,
            EXTI_RISING                      = LL_EXTI_TRIGGER_RISING,
            EXTI_FALLING                     = LL_EXTI_TRIGGER_FALLING,
            EXTI_RISING_FALLING              = LL_EXTI_TRIGGER_RISING_FALLING
        }EXTIIntrOnEdge_t;

        typedef enum
        {
            EXTI_MODE_GPIO                = 0,
            EXTI_MODE_EXTI                = 1,
        }EXTIMode_t;
        
        typedef enum
        {
            NO_PULL      = GPIO_NOPULL,
            PULL_UP      = GPIO_PULLUP,
            PULL_DOWN    = GPIO_PULLDOWN
        }PULL_t;
        
        typedef enum
        {
            LOW        = GPIO_SPEED_FREQ_LOW,
            MEDIUM     = GPIO_SPEED_FREQ_MEDIUM,
            HIGH       = GPIO_SPEED_FREQ_HIGH
        }SPEED_t;
        
        typedef enum 
        {
            GPIO             = 0x00000003U,
            EXT_I            = 0x10000000U,
            GPIO_IT          = 0x00010000U,
            GPIO_EVT         = 0x00020000U,
        }INTR_OR_EVENT_t;
        
        typedef enum
        {
            IT_ON_RISING                    = 0x10110000U,
            IT_ON_FALLING                   = 0x10210000U,
            IT_ON_RISING_FALLING            = 0x10310000U,
            EVT_ON_RISING                   = 0x10120000U,
            EVT_ON_FALLING                  = 0x10220000U,
            EVT_ON_RISING_FALLING           = 0x10320000U,
        }Intr_Event_Edge_t;
        
        typedef struct
        {
            uint16_t Port : 3;
            uint16_t Pin  : 4;
            uint16_t Mode : 3;
            uint16_t Pull : 1;
            uint16_t Rsvd : 5;

        }Gpio_Info_t;
        
    GpioInput(PORT_t Port, PIN_t Pin, ISR_t aISR = nullptr, Intr_Event_Edge_t Intr_Event_Edge = IT_ON_FALLING, PULL_t PULL = PULL_UP  );

	virtual ~GpioInput(){};

	virtual Status_t HwInit();

	inline uint8_t ReadInput() const { return HAL_GPIO_ReadPin(m_Port,m_Pin); }




private:
    Status_t ConfigureInterrupt();
    
	Interrupt::IRQn MapPin2ExtLine();
    
	uint8_t MapPin2PinSource();
    
	Status_t ExtLineInit();
    
    static void EXTI0_IRQHandler();
    
    static void EXTI1_IRQHandler();
    
    static void EXTI2_IRQHandler();
    
    static void EXTI3_IRQHandler();
    
    static void EXTI4_IRQHandler();
    
    static void EXTI5_9_IRQHandler();
    
    static void EXTI15_10_IRQHandler();

private:
	PIN_t                   m_Pin;
	PORT_t                  m_Port;
    PULL_t                  m_Pull;
    ISR_t                   m_pISR;
    Intr_Event_Edge_t       m_Intr_Event_Edge;
    static Interrupt::ISR   GPIO_ISRs[16];
};

}// namespace Bsp




#endif /* APP_INC_GPIOINPUT_HPP_ */
