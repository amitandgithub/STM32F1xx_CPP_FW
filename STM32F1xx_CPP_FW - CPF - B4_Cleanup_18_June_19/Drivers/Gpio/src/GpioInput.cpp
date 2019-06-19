/*
 * GpioInput.cpp
 *
 *  Created on: 08-Oct-2017
 *      Author: Amit Chaudhary
 */


#include "GpioInput.hpp"

namespace Peripherals
{

Interrupt::ISR GpioInput::GPIO_ISRs[16];

GpioInput::GpioInput(PORT_t Port, PIN_t Pin, ISR_t aISR, Intr_Event_Edge_t Intr_Event_Edge, PULL_t PULL)
{
	m_Port              = Port;
	m_Pin               = Pin;
    m_Pull              = PULL;
	m_pISR              = aISR;
    m_Intr_Event_Edge   = Intr_Event_Edge; 
}



Interrupt::IRQn GpioInput::MapPin2ExtLine()
{
	const Interrupt::IRQn ExtLine =
        (GpioInput::m_Pin == GPIO_PIN_0) ? Interrupt::EXTI0_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_1) ? Interrupt::EXTI1_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_2) ? Interrupt::EXTI2_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_3) ? Interrupt::EXTI3_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_4) ? Interrupt::EXTI4_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_5) ? Interrupt::EXTI9_5_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_6) ? Interrupt::EXTI9_5_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_7) ? Interrupt::EXTI9_5_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_8) ? Interrupt::EXTI9_5_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_9) ? Interrupt::EXTI9_5_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_10)? Interrupt::EXTI15_10_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_11)? Interrupt::EXTI15_10_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_12)? Interrupt::EXTI15_10_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_13)? Interrupt::EXTI15_10_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_14)? Interrupt::EXTI15_10_IRQHandler :
        (GpioInput::m_Pin == GPIO_PIN_15)? Interrupt::EXTI15_10_IRQHandler : Interrupt::EXTI0_IRQHandler;

	return ExtLine;
}

uint8_t GpioInput::MapPin2PinSource()
{
	const uint8_t PinSource =
         (GpioInput::m_Pin == GPIO_PIN_0) ? 0  :
         (GpioInput::m_Pin == GPIO_PIN_1) ? 1  :
         (GpioInput::m_Pin == GPIO_PIN_2) ? 2  :
         (GpioInput::m_Pin == GPIO_PIN_3) ? 3  :
         (GpioInput::m_Pin == GPIO_PIN_4) ? 4  :
         (GpioInput::m_Pin == GPIO_PIN_5) ? 5  :
         (GpioInput::m_Pin == GPIO_PIN_6) ? 6  :
         (GpioInput::m_Pin == GPIO_PIN_7) ? 7  :
         (GpioInput::m_Pin == GPIO_PIN_8) ? 8  :
         (GpioInput::m_Pin == GPIO_PIN_9) ? 9  :
         (GpioInput::m_Pin == GPIO_PIN_10)? 10 :
         (GpioInput::m_Pin == GPIO_PIN_11)? 11 :
         (GpioInput::m_Pin == GPIO_PIN_12)? 12 :
         (GpioInput::m_Pin == GPIO_PIN_13)? 13 :
         (GpioInput::m_Pin == GPIO_PIN_14)? 14 :
         (GpioInput::m_Pin == GPIO_PIN_15)? 15 : 0;

	return PinSource;
}

Status_t GpioInput::HwInit()
{
        GPIO_InitTypeDef GPIO_InitStructure;
        const Peripheral_t  APB_Peripheral =
                                           (m_Port==GPIOA)? APB2Periph_GPIOA :
                                           (m_Port==GPIOB)? APB2Periph_GPIOB :
                                           (m_Port==GPIOC)? APB2Periph_GPIOC :
                                           (m_Port==GPIOD)?APB2Periph_GPIOD :
                                           (m_Port==GPIOE)? APB2Periph_GPIOE : APB2Periph_GPIOA;

     // Enable the GPIO hardware Clocks
     //__HAL_RCC_AFIO_CLK_ENABLE();
     
     if(APB_Peripheral == APB2Periph_GPIOA )
         __HAL_RCC_GPIOA_CLK_ENABLE();
     else if(APB_Peripheral == APB2Periph_GPIOB )
         __HAL_RCC_GPIOB_CLK_ENABLE();
     else if(APB_Peripheral == APB2Periph_GPIOC )
         __HAL_RCC_GPIOC_CLK_ENABLE();
     else if(APB_Peripheral == APB2Periph_GPIOD )
         __HAL_RCC_GPIOD_CLK_ENABLE();
     else if(APB_Peripheral == APB2Periph_GPIOE )
         __HAL_RCC_GPIOE_CLK_ENABLE();
     else {;}       // Error    


      
    
       if(m_pISR == nullptr)
       {
           GPIO_InitStructure.Mode   = INPUT;
       }
       else
       {
          GPIO_InitStructure.Mode   = m_Intr_Event_Edge;
       }
       GPIO_InitStructure.Pin    = m_Pin;
       GPIO_InitStructure.Speed  = MEDIUM;       
       GPIO_InitStructure.Pull   = m_Pull;
       HAL_GPIO_Init( m_Port, &GPIO_InitStructure );

      
       
       if(m_pISR != nullptr)
       {
           //ExtLineInit();

           // Configure the Interrupt if ISR is supplied with the constructor
           ConfigureInterrupt();
       }
       return true;
}

Status_t GpioInput::ConfigureInterrupt()
{
    Interrupt::IRQn L_IRQn;
    uint8_t Pin = MapPin2PinSource();

    if(m_pISR != nullptr)
    {
        L_IRQn = MapPin2ExtLine();
        
        if( L_IRQn == Interrupt::EXTI9_5_IRQHandler)
        {
            if((Pin > 4) && (Pin < 10) )
            {
                GPIO_ISRs[Pin] = m_pISR;
                Interrupt::RegisterInterrupt_Vct_Table(GpioInput::EXTI5_9_IRQHandler, L_IRQn); 
            }
        }
        else if( L_IRQn == Interrupt::EXTI15_10_IRQHandler)
        {
             if((Pin > 9) && (Pin < 16) )
             {
                GPIO_ISRs[Pin] = m_pISR;
                Interrupt::RegisterInterrupt_Vct_Table(GpioInput::EXTI15_10_IRQHandler, L_IRQn); 
             }
        }
        else 
        {
             Interrupt::ISR L_ISR = nullptr;
             GPIO_ISRs[Pin%16] = m_pISR;
             
             switch(Pin)
             {
             case 0 : L_ISR = EXTI0_IRQHandler; break;
             case 1 : L_ISR = EXTI1_IRQHandler; break;
             case 2 : L_ISR = EXTI2_IRQHandler; break;
             case 3 : L_ISR = EXTI3_IRQHandler; break;
             case 4 : L_ISR = EXTI4_IRQHandler; break;
             default: ;
             }
             
             Interrupt::RegisterInterrupt_Vct_Table(L_ISR, L_IRQn); 
        }
        //Enable_Interrupt
        Interrupt::EnableInterrupt(L_IRQn);

        return true;
    }

    return false;
}

Status_t GpioInput::ExtLineInit()
{
	LL_EXTI_InitTypeDef EXTI_InitStruct;
    
    EXTI_InitStruct.Line_0_31 = m_Pin;
    EXTI_InitStruct.Mode = m_Intr_Event_Edge;
    EXTI_InitStruct.Trigger = EXTI_FALLING;
    EXTI_InitStruct.LineCommand = (FunctionalState)ENABLE;
    LL_EXTI_Init(&EXTI_InitStruct);
    return true;
}
void GpioInput::EXTI5_9_IRQHandler()
{
	if((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_5) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_5);
        if (GPIO_ISRs[5])  GPIO_ISRs[5]();
	}
	else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_6) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_6);
        if (GPIO_ISRs[6])  GPIO_ISRs[6]();
	}
	else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_7) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_7);
        if (GPIO_ISRs[7])  GPIO_ISRs[7]();

	}
	else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_8) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_8);
        if (GPIO_ISRs[8])  GPIO_ISRs[8]();

	}
	else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_9) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_9);
        if (GPIO_ISRs[9])  GPIO_ISRs[9]();
	}
}

void GpioInput::EXTI15_10_IRQHandler()
{

	if((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_10) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_10);
        if (GPIO_ISRs[10])  GPIO_ISRs[10]();

	}
	else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_11) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_11);
        if (GPIO_ISRs[11])  GPIO_ISRs[11]();

	}
	else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_12) == true)
	{
        SET_BIT(EXTI->PR,LL_EXTI_LINE_12);
        if (GPIO_ISRs[12])  GPIO_ISRs[12]();

	}
	else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_13) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_13);
        if (GPIO_ISRs[13])  GPIO_ISRs[13]();

	}
	else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_14) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_14);
        if (GPIO_ISRs[14])  GPIO_ISRs[14]();

	}
	else if ((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_15) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_15);
        if (GPIO_ISRs[15])  GPIO_ISRs[15]();

	}
}

void GpioInput::EXTI0_IRQHandler()
{
	if((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_0) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_0);
         if (GPIO_ISRs[0])  GPIO_ISRs[0]();
	}

}
void GpioInput::EXTI1_IRQHandler()
{
	if((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_1) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_1);
         if (GPIO_ISRs[1])  GPIO_ISRs[1]();
	}

}
void GpioInput::EXTI2_IRQHandler()
{
	if((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_2) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_2);
         if (GPIO_ISRs[2])  GPIO_ISRs[2]();
	}

}
void GpioInput::EXTI3_IRQHandler()
{
	if((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_3) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_3);
         if (GPIO_ISRs[3])  GPIO_ISRs[3]();

	}

}
void GpioInput::EXTI4_IRQHandler()
{
	if((bool)LL_EXTI_IsEnabledIT_0_31(LL_EXTI_LINE_4) == true)
	{
		SET_BIT(EXTI->PR,LL_EXTI_LINE_4);
         if (GPIO_ISRs[4])  GPIO_ISRs[4]();

	}

}

}



