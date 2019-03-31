/*
* GpioInput.cpp
*
*  Created on: 08-Oct-2017
*  - Added the CPF related changes - 31-March-2019
*      Author: Amit Chaudhary
*/


#include "GpioInput.h"

namespace HAL
{
    
    GpioInput::Callback_t  GpioInput::_GpioCallbacks[16];
    
    GpioInput::GpioInput( Pin_t  Pin,
                          GpioInput_Params_t Params,
                          Intr_Event_Edge_t Edge,
                          Callback* const pCallback )
                        : Gpio(Pin), _params(Params), _Edge(Edge),_Callback(pCallback)
        {
            _GpioCallbacks[POSITION_VAL(_pin)] = pCallback;
        }
        
        //0x88  Code, when compiled with High Optimization 
        //constexpr Interrupt::IRQn GpioInput::MapPin2ExtLine(uint32_t  Pin)
        //{
        //    
        //	const Interrupt::IRQn ExtLine =
        //        (Pin == 0) ? Interrupt::EXTI0_IRQHandler :
        //        (Pin == 1) ? Interrupt::EXTI1_IRQHandler :
        //        (Pin == 2) ? Interrupt::EXTI2_IRQHandler :
        //        (Pin == 3) ? Interrupt::EXTI3_IRQHandler :
        //        (Pin == 4) ? Interrupt::EXTI4_IRQHandler :
        //        (Pin == 5) ? Interrupt::EXTI9_5_IRQHandler :
        //        (Pin == 6) ? Interrupt::EXTI9_5_IRQHandler :
        //        (Pin == 7) ? Interrupt::EXTI9_5_IRQHandler :
        //        (Pin == 8) ? Interrupt::EXTI9_5_IRQHandler :
        //        (Pin == 9) ? Interrupt::EXTI9_5_IRQHandler :
        //        (Pin == 10)? Interrupt::EXTI15_10_IRQHandler :
        //        (Pin == 11)? Interrupt::EXTI15_10_IRQHandler :
        //        (Pin == 12)? Interrupt::EXTI15_10_IRQHandler :
        //        (Pin == 13)? Interrupt::EXTI15_10_IRQHandler :
        //        (Pin == 14)? Interrupt::EXTI15_10_IRQHandler :
        //        (Pin == 15)? Interrupt::EXTI15_10_IRQHandler : Interrupt::EXTI0_IRQHandler;
        //
        //	return ExtLine;
        //}
        
        
        // 0x20  Code
        constexpr GpioInput::IRQn_t GpioInput::MapPin2ExtLine(uint32_t  Pin)
        {
            const IRQn_t IRQns[16] = 
            {   EXTI0_IRQn,
            EXTI1_IRQn,
            EXTI2_IRQn,
            EXTI3_IRQn,
            EXTI4_IRQn,
            EXTI9_5_IRQn,
            EXTI9_5_IRQn,
            EXTI9_5_IRQn,
            EXTI9_5_IRQn,
            EXTI9_5_IRQn,
            EXTI15_10_IRQn,
            EXTI15_10_IRQn,
            EXTI15_10_IRQn,
            EXTI15_10_IRQn,
            EXTI15_10_IRQn,
            EXTI15_10_IRQn
            };            
            return IRQns[Pin % PINS_PER_PORT];
        }
        
        // 0x46  Code, but when compiled with High Optimization 
        // its eliminated by compiler completly 
        //constexpr IRQn_t GpioInput::MapPin2ExtLine(uint32_t  Pin)
        //{
        //    switch(Pin)
        //    {
        //    case 0: return Interrupt::EXTI0_IRQHandler;
        //    case 1: return Interrupt::EXTI1_IRQHandler;
        //    case 2: return Interrupt::EXTI2_IRQHandler;
        //    case 3: return Interrupt::EXTI3_IRQHandler;
        //    case 4: return Interrupt::EXTI4_IRQHandler;
        //    case 5:
        //    case 6: 
        //    case 7: 
        //    case 8: 
        //    case 9: return Interrupt::EXTI9_5_IRQHandler;
        //    case 10: 
        //    case 11: 
        //    case 12: 
        //    case 13: 
        //    case 14: 
        //    case 15: return Interrupt::EXTI15_10_IRQHandler;
        //    default: break;
        //    }
        //    return Interrupt::EXTI0_IRQHandler;
        //}
        
        
        GpioInput::GpioStatus_t GpioInput::HwInit(void *pInitStruct)
        {
            ErrorStatus status;
            GpioStatus_t Status;
            LL_GPIO_InitTypeDef GPIO_InitTypeDef; 
            
            if(pInitStruct == nullptr)
            {
                pInitStruct = &GPIO_InitTypeDef;
                Status = get_GPIO_InitTypeDef((LL_GPIO_InitTypeDef*)pInitStruct);       
            }
            
            ClockEnable();    
            
            if( _GpioCallbacks[POSITION_VAL(_pin)] != nullptr)
            {
                ExtLineInit();
                
                // Configure the Interrupt if ISR is supplied with the constructor
                ConfigureInterrupt((uint32_t)POSITION_VAL(_pin)); // if _pin = 0x80, then POSITION_VAL(_pin) is 7
                // RegisterInterrupt(_RisingCallback,_FallingCallback);
            }
            status = LL_GPIO_Init(_GPIOx, (LL_GPIO_InitTypeDef*)pInitStruct);
            
            return (status == SUCCESS) && (Status == GPIO_SUCCESS) ? GPIO_SUCCESS: GPIO_ERROR;
            
        }
        
        GpioInput::GpioStatus_t GpioInput::get_GPIO_InitTypeDef(LL_GPIO_InitTypeDef* pLL_GPIO_Init)
        {
            if(pLL_GPIO_Init == nullptr)
                return GPIO_ERROR_INVALID_PARAMS; 
            
            
            pLL_GPIO_Init->Pin              = get_LL_pin(_pin); // Init Done 
            pLL_GPIO_Init->Speed            = LL_GPIO_MODE_OUTPUT_2MHz;
            pLL_GPIO_Init->Mode             = LL_GPIO_MODE_INPUT; 
            
            switch(_params)
            {
            case INPUT_FLOATING:    pLL_GPIO_Init->Pull = 0;
            break;
            case INPUT_PULL_UP:     pLL_GPIO_Init->Pull = LL_GPIO_PULL_UP;
            break;
            case INPUT_PULL_DOWN:   pLL_GPIO_Init->Pull = LL_GPIO_PULL_DOWN;
            break;                                  
            case ANALOG:            pLL_GPIO_Init->Mode =  LL_GPIO_MODE_ANALOG;
            break;
            default:                return GPIO_ERROR;  
            break;
            }
            
            return GPIO_SUCCESS;             
        }
        
        GpioInput::GpioStatus_t GpioInput::ConfigureInterrupt(const uint32_t Pin)
        {
            IRQn_t L_IRQn;
            //constexpr uint32_t Pin = Pin;//POSITION_VAL(_pin);// MapPin2PinSource();
            
            L_IRQn = MapPin2ExtLine(Pin);
            InterruptManager::GetInstance()->RegisterDeviceInterrupt(L_IRQn,0,this);
            return GPIO_SUCCESS;
        }
        
        void GpioInput::RegisterInterrupt(Callback* const pRisingCallback, Callback* const pFallingCallback )
        {
            
        }
        
        GpioInput::GpioStatus_t GpioInput::ExtLineInit()
        {
            LL_EXTI_InitTypeDef EXTI_InitStruct;
            
            if((_Edge == IT_ON_RISING) || (_Edge == EVT_ON_RISING))
            {
                EXTI_InitStruct.Trigger = EXTI_RISING;
            }
            else if((_Edge == IT_ON_FALLING) || (_Edge == EVT_ON_FALLING))
            {
                EXTI_InitStruct.Trigger = EXTI_FALLING;
            }
            else
            {
                EXTI_InitStruct.Trigger = EXTI_RISING_FALLING;
            } 
            
            EXTI_InitStruct.Line_0_31 = _pin;
            EXTI_InitStruct.Mode = _Edge;    
            EXTI_InitStruct.LineCommand = (FunctionalState)ENABLE;
            LL_EXTI_Init(&EXTI_InitStruct);
            return GPIO_SUCCESS;
        }
        
        void GpioInput::ISR(IRQn_t IRQNumber)
        {
            if(IRQNumber == EXTI0_IRQn )
            {
                SET_BIT(EXTI->PR,LL_EXTI_LINE_0);
                _GpioCallbacks[0]->CallbackFunction();
            }
            else if(IRQNumber == EXTI1_IRQn )
            {
                SET_BIT(EXTI->PR,LL_EXTI_LINE_1);
                _GpioCallbacks[1]->CallbackFunction();
            }
            else if(IRQNumber == EXTI2_IRQn )
            {
                SET_BIT(EXTI->PR,LL_EXTI_LINE_2);
                _GpioCallbacks[2]->CallbackFunction();
            }
            else if(IRQNumber == EXTI3_IRQn )
            {
                SET_BIT(EXTI->PR,LL_EXTI_LINE_3);
                _GpioCallbacks[3]->CallbackFunction();
            }
            else if(IRQNumber == EXTI4_IRQn )
            {
                SET_BIT(EXTI->PR,LL_EXTI_LINE_4);
                _GpioCallbacks[4]->CallbackFunction();
            }
            else if(IRQNumber == EXTI9_5_IRQn )
            {
                if((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_5) == true)
                {
                    SET_BIT(EXTI->PR,LL_EXTI_LINE_5);
                    _GpioCallbacks[5]->CallbackFunction();
                }
                else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_6) == true)
                {
                    SET_BIT(EXTI->PR,LL_EXTI_LINE_6);
                    _GpioCallbacks[6]->CallbackFunction();
                }
                else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_7) == true)
                {
                    SET_BIT(EXTI->PR,LL_EXTI_LINE_7);
                    _GpioCallbacks[7]->CallbackFunction();
                    
                }
                else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_8) == true)
                {
                    SET_BIT(EXTI->PR,LL_EXTI_LINE_8);
                    _GpioCallbacks[8]->CallbackFunction();
                    
                }
                else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_9) == true)
                {
                    SET_BIT(EXTI->PR,LL_EXTI_LINE_9);
                    _GpioCallbacks[9]->CallbackFunction();
                }
                
            }
            else if(IRQNumber == EXTI15_10_IRQn )
            {
                if((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_10) == true)
                {
                    SET_BIT(EXTI->PR,LL_EXTI_LINE_10);
                    _GpioCallbacks[10]->CallbackFunction();
                    
                }
                else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_11) == true)
                {
                    SET_BIT(EXTI->PR,LL_EXTI_LINE_11);
                    _GpioCallbacks[11]->CallbackFunction();
                    
                }
                else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_12) == true)
                {
                    SET_BIT(EXTI->PR,LL_EXTI_LINE_12);
                    _GpioCallbacks[12]->CallbackFunction();
                    
                }
                else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_13) == true)
                {
                    SET_BIT(EXTI->PR,LL_EXTI_LINE_13);
                    _GpioCallbacks[13]->CallbackFunction();
                    
                }
                else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_14) == true)
                {
                    SET_BIT(EXTI->PR,LL_EXTI_LINE_14);
                    _GpioCallbacks[14]->CallbackFunction();
                    
                }
                else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_15) == true)
                {
                    SET_BIT(EXTI->PR,LL_EXTI_LINE_15);
                    _GpioCallbacks[15]->CallbackFunction();
                    
                }
            }
            else
            {
                while(1); // Fatal Error
            }
        }
        
}



