/******************
** FILE: Gpio.cpp
**
** DESCRIPTION:
**  Gpio class implementation
**
** CREATED: 1/16/2019, by Amit Chaudhary
******************/
#include "Gpio.h"

namespace HAL
{


    
Gpio::GpioStatus_t Gpio::ClockEnable()
{
   
    if(_GPIOx == GPIOA )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    }
    else  if(_GPIOx == GPIOB )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }
    else  if(_GPIOx == GPIOC )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    }
    else  if(_GPIOx == GPIOD )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    }
#if defined(GPIOE)
    else  if(_GPIOx == GPIOE )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOE);
    }
#endif
#if defined(GPIOF)
    else  if(_GPIOx == GPIOF )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOG);
    }
#endif
    else 
    {
        return GPIO_ERROR_INVALID_PARAMS;
    }
    
     return GPIO_SUCCESS;
}

Gpio::GpioStatus_t Gpio::ClockDisable()
{
   
    if(_GPIOx == GPIOA )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    }
    else  if(_GPIOx == GPIOB )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }
    else  if(_GPIOx == GPIOB )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    }
    else  if(_GPIOx == GPIOD )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    }
#if defined(GPIOE)
    else  if(_GPIOx == GPIOE )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOE);
    }
#endif
#if defined(GPIOF)
    else  if(_GPIOx == GPIOF )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOG);
    }
#endif
    else 
    {
        return GPIO_ERROR_INVALID_PARAMS;
    }
    
     return GPIO_SUCCESS;
}

        

    
}