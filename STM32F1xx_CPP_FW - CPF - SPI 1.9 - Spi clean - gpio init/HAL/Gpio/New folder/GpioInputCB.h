/*
 * GpioInputCB.hpp
 *
 *  Created on: 08-Oct-2017
 *      Author: Amit Chaudhary
 */

#ifndef APP_INC_GPIOINPUT_HPP_
#define APP_INC_GPIOINPUT_HPP_

#include"Interrupt.h"
#include"Gpio.h"
#include"callback.h"
     
namespace HAL
{

class GpioInputCB : public Gpio, public Interrupt 
{
public:
    
    typedef enum : uint8_t
    {
        INPUT_FLOATING,
        INPUT_PULL_UP,
        INPUT_PULL_DOWN,
        ANALOG
    }GpioInput_Params_t;

        
    typedef enum
    {
        IT_RISING            =	0x10110000U,   				/*!< External Interrupt Mode with Rising edge trigger detection          */
        IT_FALLING           =	0x10210000U,   				/*!< External Interrupt Mode with Falling edge trigger detection         */
        IT_RISING_FALLING    =	0x10310000U,   				/*!< External Interrupt Mode with Rising/Falling edge trigger detection  */
        EVT_RISING           =	0x10120000U,   				/*!< External Event Mode with Rising edge trigger detection               */
        EVT_FALLING          =	0x10220000U,   				/*!< External Event Mode with Falling edge trigger detection              */
        EVT_RISING_FALLING   =	0x10320000U,   				/*!< External Event Mode with Rising/Falling edge trigger detection       */
        NO_INT_NO_EVT        =  0x00000000U
    }Interrupt_Edge_t;
        
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
        
    GpioInputCB(Pin_t  Pin, GpioInput_Params_t Params = INPUT_PULL_UP , Intr_Event_Edge_t Edge = IT_ON_FALLING, ISR_t aISR = nullptr);
    GpioInputCB(Pin_t  Pin, GpioInput_Params_t Params = INPUT_PULL_UP , Intr_Event_Edge_t Edge = IT_ON_FALLING, const CBFunctor0 &uponClickDoThis = nullptr);

	~GpioInputCB(){};

	GpioStatus_t HwInit(void *pInitStruct = nullptr);

	inline bool Read() const;
    bool Read(GpioInputCB* thisPtr);



private:
    
    GpioStatus_t get_GPIO_InitTypeDef(LL_GPIO_InitTypeDef* pLL_GPIO_Init);
    
    GpioStatus_t  ConfigureInterrupt(const uint32_t Pin);
    
	constexpr Interrupt::IRQn MapPin2ExtLine(uint32_t  Pin);
    
	GpioStatus_t ExtLineInit();
    
    static void EXTI0_IRQHandler();
    
    static void EXTI1_IRQHandler();
    
    static void EXTI2_IRQHandler();
    
    static void EXTI3_IRQHandler();
    
    static void EXTI4_IRQHandler();
    
    static void EXTI5_9_IRQHandler();
    
    static void EXTI15_10_IRQHandler();

private:
    const GpioInput_Params_t        _params;
    const Intr_Event_Edge_t         _Intr_Event_Edge;
    ISR_t                           _pISR;    
    static Interrupt::ISR           _GPIO_ISRs[16];
    static CBFunctor0               notify;
};



inline bool GpioInputCB::Read() const
{
    return (_GPIOx->IDR) & _pin ? true : false;
}



}// namespace Bsp

#endif /* APP_INC_GPIOINPUT_HPP_ */
