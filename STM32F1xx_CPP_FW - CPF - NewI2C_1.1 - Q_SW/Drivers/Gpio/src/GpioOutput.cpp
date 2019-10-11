/*
 * GpioOutput.cpp
 *
 *  Created on: 07-Oct-2017
 *      Author: Amit Chaudhary
 */


#include "GpioOutput.hpp"

namespace Peripherals
{
GpioOutput::GpioOutput(PORT_t Port, PIN_t Pin)
{
	m_Port = Port;
	m_Pin  = Pin;
	m_Mode = OUTPUT_PP;
    m_Speed = HIGH;
    m_PULL =  GPIO_NOPULL;
}
GpioOutput::GpioOutput(PORT_t Port, PIN_t Pin, MODE_t aMODE)
{
	m_Port = Port;
	m_Pin  = Pin;
	m_Mode = aMODE;
    m_Speed = HIGH;
    m_PULL =  GPIO_NOPULL;
}
GpioOutput::GpioOutput(PORT_t Port, PIN_t Pin, MODE_t aMODE, SPEED_t SPEED)
{
	m_Port = Port;
	m_Pin  = Pin;
	m_Mode = aMODE;
    m_Speed = SPEED;
    m_PULL =  GPIO_NOPULL;
}

GpioOutput::GpioOutput(PORT_t Port, PIN_t Pin, MODE_t aMODE, SPEED_t SPEED, PULL_t PULL)
{
	m_Port = Port;
	m_Pin  = Pin;
	m_Mode = aMODE;
    m_Speed = SPEED;
    m_PULL = PULL;
}

Status_t GpioOutput::HwInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	const Peripheral_t  PeripheralName =
                                         (m_Port==GPIOA)? APB2Periph_GPIOA :
                                         (m_Port==GPIOB)? APB2Periph_GPIOB :
                                         (m_Port==GPIOC)? APB2Periph_GPIOC :
                                         (m_Port==GPIOD)? APB2Periph_GPIOD :
                                         (m_Port==GPIOE)? APB2Periph_GPIOE : APB2Periph_GPIOA;

     // Enable the GPIO hardware Clocks
     __HAL_RCC_AFIO_CLK_ENABLE();
     
     if(PeripheralName == APB2Periph_GPIOA )
         __HAL_RCC_GPIOA_CLK_ENABLE();
     else if(PeripheralName == APB2Periph_GPIOB )
         __HAL_RCC_GPIOB_CLK_ENABLE();
     else if(PeripheralName == APB2Periph_GPIOC )
         __HAL_RCC_GPIOC_CLK_ENABLE();
     else if(PeripheralName == APB2Periph_GPIOD )
         __HAL_RCC_GPIOD_CLK_ENABLE();
     else if(PeripheralName == APB2Periph_GPIOE )
         __HAL_RCC_GPIOE_CLK_ENABLE();
     else {;}       // Error          
     
     GPIO_InitStructure.Pin   = m_Pin;
     GPIO_InitStructure.Speed = m_Speed;
     GPIO_InitStructure.Mode  = m_Mode;
     GPIO_InitStructure.Pull  = m_PULL;
     
     HAL_GPIO_Init( m_Port, &GPIO_InitStructure );
     
     return true;
}






}

