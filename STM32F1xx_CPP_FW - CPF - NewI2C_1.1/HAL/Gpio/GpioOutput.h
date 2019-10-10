/******************
** CLASS: GpioOutput
**
** DESCRIPTION:
**  GpioOutput class functionality
**
** CREATED: 1/17/2019, by Amit Chaudhary
**
** FILE: GpioOutput.h
**
******************/



#ifndef GpioOutput_h
#define GpioOutput_h

#include "Gpio.h"

namespace HAL
{
  
class GpioOutput : public Gpio
{
public:

typedef enum : uint8_t
{
    PUSH_PULL,
    OPEN_DRAIN,
    AF_PUSH_PULL,
    AF_OPEN_DRAIN  
}GpioOutput_Params_t;

typedef enum
{
    GPIO_OUTPUT_TYPE_PUSHPULL = LL_GPIO_OUTPUT_PUSHPULL, 
    GPIO_OUTPUT_TYPE_OPENDRAIN = LL_GPIO_OUTPUT_OPENDRAIN,        
}GpioOutputType_t;

typedef enum
{
    GPIO_OUTPUT_SPEED_LOW = LL_GPIO_SPEED_FREQ_LOW, 
    GPIO_OUTPUT_SPEED_MID = LL_GPIO_SPEED_FREQ_MEDIUM, 
    GPIO_OUTPUT_SPEED_HIGH = LL_GPIO_SPEED_FREQ_HIGH, 
}GpioOutputSpeed_t;

typedef enum
{
    GPIO_OUTPUT_TYPE_PULL_DOWN = LL_GPIO_PULL_DOWN, 
    GPIO_OUTPUT_TYPE_PULL_UP = LL_GPIO_PULL_UP,        
}GpioOutputPull_t;

~GpioOutput(){};

constexpr GpioOutput(Pin_t  Pin, GpioOutput_Params_t Params = PUSH_PULL);

GpioStatus_t HwInit(void *pInitStruct = nullptr);

GpioStatus_t HwDeinit();

void High();

void Low();

void Toggle();

bool ReadOutput();

void SetOutputType(GpioOutputType_t OutputType);

void SetPinSpeed(GpioOutputSpeed_t OutputSpeed);

void SetPull(GpioOutputPull_t Pull);

GpioOutputType_t    GetOutputType();

GpioOutputSpeed_t   GetPinSpeed();

GpioOutputPull_t    GetOutputPull();  

private: 
GpioStatus_t get_GPIO_InitTypeDef(LL_GPIO_InitTypeDef* pLL_GPIO_Init);

private:
    GpioOutput_Params_t _params;
    
};

constexpr GpioOutput::GpioOutput(Pin_t  Pin, GpioOutput_Params_t Params) : Gpio(Pin),_params(Params)
{

}
inline void GpioOutput::High()
{
    _GPIOx->BSRR |= _pin;
}

inline void GpioOutput::Low()
{
    _GPIOx->BRR |= _pin;
}

inline void GpioOutput::Toggle()
{
    _GPIOx->ODR ^= _pin;
}

inline bool GpioOutput::ReadOutput()
{
    return _GPIOx->ODR & _pin ? true : false;
}

inline void GpioOutput::SetOutputType(GpioOutputType_t OutputType)
{
    LL_GPIO_SetPinOutputType(_GPIOx, get_LL_pin(_pin), OutputType);   
}

inline void GpioOutput::SetPinSpeed(GpioOutputSpeed_t OutputSpeed)
{
    LL_GPIO_SetPinSpeed(_GPIOx, get_LL_pin(_pin), OutputSpeed);   
}

inline void GpioOutput::SetPull(GpioOutputPull_t Pull)
{
    LL_GPIO_SetPinPull(_GPIOx, get_LL_pin(_pin), Pull);  
}


inline GpioOutput::GpioOutputType_t GpioOutput::GetOutputType()
{
    
    return LL_GPIO_GetPinOutputType(_GPIOx, get_LL_pin(_pin)) == GPIO_OUTPUT_TYPE_PUSHPULL ? 
                                                        GPIO_OUTPUT_TYPE_PUSHPULL : GPIO_OUTPUT_TYPE_OPENDRAIN;    
}

inline GpioOutput::GpioOutputSpeed_t GpioOutput::GetPinSpeed()
{    
    return (GpioOutputSpeed_t)LL_GPIO_GetPinSpeed(_GPIOx, get_LL_pin(_pin));
}

inline GpioOutput::GpioOutputPull_t GpioOutput::GetOutputPull()
{
    
    return (GpioOutputPull_t)LL_GPIO_GetPinPull(_GPIOx, get_LL_pin(_pin));
}

}

#endif //GpioOutput_h
