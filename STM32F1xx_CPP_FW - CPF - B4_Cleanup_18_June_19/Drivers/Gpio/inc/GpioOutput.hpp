/*
* GpioOutput.hpp
*
*  Created on: 07-Oct-2017
*      Author: Amit Chaudhary
*/

#ifndef APP_INC_GPIOOUTPUT_HPP_
#define APP_INC_GPIOOUTPUT_HPP_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "Peripheral.hpp"

namespace Peripherals
{
    
    class GpioOutput : public Peripheral
    {
    public:
        
        typedef enum 
        {
            INPUT                =	0x00000000U,   				/*!< Input Floating Mode                   */
            OUTPUT_PP            =	0x00000001U,   				/*!< Output Push Pull Mode                 */
            OUTPUT_OD            =	0x00000011U,   				/*!< Output Open Drain Mode                */
            AF_PP                =	0x00000002U,   				/*!< Alternate Function Push Pull Mode     */
            AF_OD                =	0x00000012U,   				/*!< Alternate Function Open Drain Mode    */
            AF_INPUT             =	0x00000000U,                	        /*!< Alternate Function Input Mode         */
            ANALOG               =	0x00000003U   				/*!< Analog Mode  */
        }MODE_t;
        
        typedef enum
        {
            IT_RISING            =	0x10110000U,   				/*!< External Interrupt Mode with Rising edge trigger detection          */
            IT_FALLING           =	0x10210000U,   				/*!< External Interrupt Mode with Falling edge trigger detection         */
            IT_RISING_FALLING    =	0x10310000U,   				/*!< External Interrupt Mode with Rising/Falling edge trigger detection  */
            EVT_RISING           =	0x10120000U,   				/*!< External Event Mode with Rising edge trigger detection               */
            EVT_FALLING          =	0x10220000U,   				/*!< External Event Mode with Falling edge trigger detection              */
            EVT_RISING_FALLING   =	0x10320000U,   				/*!< External Event Mode with Rising/Falling edge trigger detection       */
        }INTR_ON_EDGE_t;
        
        typedef GPIO_TypeDef*     PORT_t;
        typedef uint16_t          PIN_t;
        
        typedef enum
        {
            EXTI_Trigger_Rising         = 0x08,
            EXTI_Trigger_Falling        = 0x0C,
            EXTI_Trigger_Rising_Falling = 0x10
        }IntOnWhichEdge;

        typedef enum
        {
            EXTI_Mode_Interrupt = 0x00,
            EXTI_Mode_Event     = 0x04
        }EXTIMode;
        
        enum
        {
            NO_PULL      = GPIO_NOPULL,
            PULL_UP      = GPIO_PULLUP,
            PULL_DOWN    = GPIO_PULLDOWN
        };
        
        typedef uint32_t PULL_t;
        
        typedef enum
        {
            LOW        = GPIO_SPEED_FREQ_LOW,
            MEDIUM     = GPIO_SPEED_FREQ_MEDIUM,
            HIGH       = GPIO_SPEED_FREQ_HIGH
        }SPEED_t;
        

        
        GpioOutput(PORT_t Port, PIN_t Pin);
        
        GpioOutput(PORT_t Port, PIN_t Pin, MODE_t aMODE);
        
        GpioOutput(PORT_t Port, PIN_t Pin, MODE_t aMODE, SPEED_t SPEED);
        
        GpioOutput(PORT_t Port, PIN_t Pin, MODE_t aMODE, SPEED_t SPEED, PULL_t PULL);
        
        virtual ~GpioOutput(void){};
        
        virtual Status_t HwInit();
        
        inline void On(void){m_Port->BSRR = m_Pin;}//(m_Port, m_Pin,(GPIO_PinState)1);}
        
        inline void Off(void){m_Port->BSRR = (uint32_t)m_Pin << 16U;}//{HAL_GPIO_WritePin(m_Port, m_Pin, (GPIO_PinState)0);}
        
        //inline void Write(bool Value){ Value ? HAL_GPIO_WritePin(m_Port, m_Pin,(GPIO_PinState)1) : HAL_GPIO_WritePin(m_Port, m_Pin,(GPIO_PinState)0);}
        
        inline void Write(bool Value){ Value ? (m_Port->BSRR = m_Pin) : (m_Port->BSRR = (uint32_t)m_Port << 16U) ;}
        
        inline void ToggleOutput(void){ m_Port->ODR ^= m_Pin;}//{HAL_GPIO_TogglePin(m_Port,m_Pin);}
        
        inline GPIO_PinState ReadOutputValue(void){ return (m_Port->ODR & m_Pin) ? GPIO_PIN_SET : GPIO_PIN_RESET ;}
        
    private:
        PORT_t       m_Port;
        PIN_t        m_Pin;
        MODE_t       m_Mode;
        SPEED_t      m_Speed;
        PULL_t       m_PULL;
        
    };    
    
    
}



#endif /* APP_INC_GPIOOUTPUT_HPP_ */
