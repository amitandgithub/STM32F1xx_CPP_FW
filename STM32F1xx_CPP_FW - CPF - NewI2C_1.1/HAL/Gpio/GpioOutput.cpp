/******************
** FILE: GpioOutput.cpp
**
** DESCRIPTION:
**  GpioOutput class Implementation
**
** CREATED: 1/17/2019, by Amit Chaudhary
******************/

#include "GpioOutput.h"

namespace HAL
{
 
GpioOutput::GpioStatus_t GpioOutput::get_GPIO_InitTypeDef(LL_GPIO_InitTypeDef* pLL_GPIO_Init)
{
    if(pLL_GPIO_Init == nullptr)
        return GPIO_ERROR_INVALID_PARAMS; 
    
    pLL_GPIO_Init->Pin   = get_LL_pin(_pin); // Init Done
    pLL_GPIO_Init->Pull  = 0;    
    pLL_GPIO_Init->Speed = LL_GPIO_MODE_OUTPUT_2MHz;    

    switch(_params)
    {
        case PUSH_PULL:     pLL_GPIO_Init->Mode         = LL_GPIO_MODE_OUTPUT;  
                            pLL_GPIO_Init->OutputType   = LL_GPIO_OUTPUT_PUSHPULL;
                            break;
        case OPEN_DRAIN:    pLL_GPIO_Init->Mode         = LL_GPIO_MODE_OUTPUT; 
                            pLL_GPIO_Init->OutputType   = LL_GPIO_OUTPUT_OPENDRAIN;
                            break;
        case AF_PUSH_PULL:  pLL_GPIO_Init->Mode         = LL_GPIO_MODE_ALTERNATE;
                            pLL_GPIO_Init->OutputType   = LL_GPIO_OUTPUT_PUSHPULL;
                            break;                                  
        case AF_OPEN_DRAIN: pLL_GPIO_Init->Mode         = LL_GPIO_MODE_ALTERNATE;
                            pLL_GPIO_Init->OutputType   = LL_GPIO_OUTPUT_OPENDRAIN;
                            break;
        default:            return GPIO_ERROR;  
                            break;
    }
    return GPIO_SUCCESS;     
                     
}

GpioOutput::GpioStatus_t GpioOutput::HwInit(void *pInitStruct)
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
    
    status = LL_GPIO_Init(_GPIOx, (LL_GPIO_InitTypeDef*)pInitStruct);
    
    return (status == SUCCESS) && (Status == GPIO_SUCCESS) ? GPIO_SUCCESS: GPIO_ERROR;
}

}