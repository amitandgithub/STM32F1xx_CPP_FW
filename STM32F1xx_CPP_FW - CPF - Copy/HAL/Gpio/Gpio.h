/******************
** CLASS: GpioOutput
**
** DESCRIPTION:
**  Implements the GpioOutput functionality
**
** CREATED: 1/13/2019 = 16, by Amit Chaudhary - "Happy Lohri 2019"
**
** FILE: GpioOutput.h
**
******************/



#ifndef Gpio_h
#define Gpio_h

#include "CPP_HAL.h"

namespace HAL
{
  

  
class Gpio
{
public:
   
typedef GPIO_TypeDef*const Port_t;
typedef void(*Callback_t)(); 
static const uint8_t PINS_PER_PORT = 16U;
  
typedef enum
{    
    GPIO_SUCCESS,
    GPIO_ERROR,
    GPIO_ERROR_INVALID_PARAMS,   
    
}GpioStatus_t;


typedef enum : uint8_t
{
    FLOATING    = 1U<<0U,
    PULLUP      = 1U<<1U,
    PULLDOWN    = 1U<<2U,
    ANALOG      = 1U<<3U
    
}GpioInput_Params_t;


typedef enum : uint8_t
{
    GPIO_DIR_INPUT,
    GPIO_DIR_OUTPUT    
}GpioDir_t;

typedef enum : uint8_t
{
    GPIO_MODE_ANALOG1,
    GPIO_MODE_FLOAT,
    GPIO_MODE_OPEN_DRAIN,
    GPIO_MODE_PUSH_PULL
}GpioMode_t;

typedef enum : uint8_t
{
    GPIO_IT_RISING_EDGE,
    GPIO_IT_FALLING_EDGE,
    GPIO_IT_RISING_FALLING_EDGE,
    GPIO_IT_RISING
}GpioExtIntr_t;

typedef enum : uint8_t
{
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
}GpioPull_t;

typedef enum : uint8_t
{
    GPIO_SPEED_LOW,
    GPIO_SPEED_MEDIUM,
    GPIO_SPEED_HIGH
}GpioSpeed_t;

typedef enum : uint8_t
{
    PARAM_ID_DIRECTION,
    PARAM_ID_MODE,
    PARAM_ID_PULL,
    PARAM_ID_EXTINTR
}GpioParamID_t;

typedef enum
{
	A0  =  0,
	A1  =  1,
	A2  =  2,
    A3  =  3,
	A4  =  4,
	A5  =  5,
	A6  =  6,
	A7  =  7,
	A8  =  8,
	A9  =  9,
	A10 = 10,
	A11 = 11,
	A12 = 12,
	A13 = 13,
	A14 = 14,
	A15 = 15,
	B0  = 16,
	B1  = 17,
	B2  = 18,
    B3  = 19,
	B4  = 20,
	B5  = 21,
	B6  = 22,
	B7  = 23,
	B8  = 24,
	B9  = 25,
	B10 = 26,
	B11 = 27,
	B12 = 28,
	B13 = 29,
	B14 = 30,
	B15 = 31,
	C0  = 32,
	C1  = 33,
	C2  = 34,
    C3  = 35,
	C4  = 36,
	C5  = 37,
	C6  = 38,
	C7  = 39,
	C8  = 40,
	C9  = 41,
	C10 = 42,
	C11 = 43,
	C12 = 44,
	C13 = 45,
	C14 = 46,
	C15 = 47,
	D0  = 48,
	D1  = 49,
	D2  = 50,
    D3  = 51,
	D4  = 52,
	D5  = 53,
	D6  = 54,
	D7  = 55,
	D8  = 56,
	D9  = 57,
	D10 = 58,
	D11 = 59,
	D12 = 60,
	D13 = 61,
	D14 = 62,
	D15 = 63
#ifdef GPIOE
	,E0  = 64,
	E1  = 65,
	E2  = 66,
    E3  = 67,
	E4  = 68,
	E5  = 69,
	E6  = 70,
	E7  = 71,
	E8  = 72,
	E9  = 73,
	E10 = 74,
	E11 = 75,
	E12 = 76,
	E13 = 77,
	E14 = 78,
	E15 = 79
#endif
#ifdef GPIOF
	,F0  = 80,
	F1  = 81,
	F2  = 82,
    F3  = 83,
	F4  = 84,
	F5  = 85,
	F6  = 86,
	F7  = 87,
	F8  = 88,
	F9  = 89,
	F10 = 90,
	F11 = 91,
	F12 = 92,
	F13 = 93,
	F14 = 94,
	F15 = 95
#endif
#ifdef GPIOG
	,G0   = 96,
	G1   = 97,
	G2   = 98,
	G3   = 99,
	G4   = 100,
	G5   = 101,
	G6   = 102,
	G7   = 103,
	G8   = 104,
	G9   = 105,
	G10	 = 106,
	G11	 = 107,
	G12	 = 108,
	G13	 = 109,
	G14	 = 110,
	G15	 = 111
#endif
#ifdef GPIOH
	,H0	 = 112,
	H1	 = 113,
	H2	 = 114,
	H3	 = 115,
	H4	 = 116,
	H5	 = 117,
	H6	 = 118,
	H7	 = 119,
	H8	 = 120,
	H9	 = 121,
	H10	 = 122,
	H11	 = 123,
	H12	 = 124,
	H13	 = 125,
	H14	 = 126,
	H15	 = 127
#endif
	,MAX  = 128
}Pin_t;

constexpr Gpio(Pin_t  Pin);

~Gpio(){};

GpioStatus_t ClockEnable();

GpioStatus_t ClockDisable();

constexpr Port_t  getPort(Pin_t  pin);

constexpr uint8_t getPin(Pin_t  pin);

constexpr uint32_t get_LL_pin(const uint16_t _pin)const;  
 
typedef enum : uint8_t
{
    PIN,
    MODE,
    PORT,
    CALLBACK    
}ParamID_t; 

protected:
    const Port_t  _GPIOx;
    const uint16_t _pin;
    
};

constexpr Gpio::Gpio(Pin_t  Pin ) : _GPIOx(getPort(Pin)), _pin((uint16_t)(1U<<getPin(Pin)))
{

}


constexpr Gpio::Port_t Gpio::getPort(Pin_t  pin)
{
    constexpr GPIO_TypeDef * GPIOArray[] = {GPIOA,GPIOB,GPIOC,GPIOD
        
#ifdef GPIOE
       ,GPIOE
#endif           
#ifdef GPIOF
       ,GPIOF
#endif 
#ifdef GPIOG
       ,GPIOG
#endif 
#ifdef GPIOH
       ,GPIOH
#endif 
#ifdef GPIOI
       ,GPIOI
#endif 
    };

    return GPIOArray[(pin/PINS_PER_PORT)];
}
constexpr uint8_t Gpio::getPin(Pin_t  pin)
{
    return (uint8_t)pin%PINS_PER_PORT;
}


constexpr uint32_t Gpio::get_LL_pin(const uint16_t _pin) const
{
    constexpr uint32_t LL_PINS[16] = 
    {
        LL_GPIO_PIN_0,
        LL_GPIO_PIN_1,
        LL_GPIO_PIN_2,
        LL_GPIO_PIN_3,
        LL_GPIO_PIN_4,
        LL_GPIO_PIN_5,
        LL_GPIO_PIN_6,
        LL_GPIO_PIN_7,
        LL_GPIO_PIN_8,
        LL_GPIO_PIN_9,
        LL_GPIO_PIN_10,
        LL_GPIO_PIN_11,
        LL_GPIO_PIN_12,
        LL_GPIO_PIN_13,
        LL_GPIO_PIN_14,
        LL_GPIO_PIN_15        
    };
    
    return LL_PINS[POSITION_VAL(_pin) % PINS_PER_PORT];                                
}


//constexpr uint32_t Gpio::get_LL_pin()
//{
//    return  (   _pin == (1U<<0) ? LL_GPIO_PIN_0 :
//                                _pin == (1U<<1) ? LL_GPIO_PIN_1 :  
//                                _pin == (1U<<2) ? LL_GPIO_PIN_2 :
//                                _pin == (1U<<3) ? LL_GPIO_PIN_3 :
//                                _pin == (1U<<4) ? LL_GPIO_PIN_4 :
//                                _pin == (1U<<5) ? LL_GPIO_PIN_5 :
//                                _pin == (1U<<6) ? LL_GPIO_PIN_6 :
//                                _pin == (1U<<7) ? LL_GPIO_PIN_7 :
//                                _pin == (1U<<8) ? LL_GPIO_PIN_8 :
//                                _pin == (1U<<9) ? LL_GPIO_PIN_9 :
//                                _pin == (1U<<10) ? LL_GPIO_PIN_10 :
//                                _pin == (1U<<11) ? LL_GPIO_PIN_11 :
//                                _pin == (1U<<12) ? LL_GPIO_PIN_12 :
//                                _pin == (1U<<13) ? LL_GPIO_PIN_13 :
//                                _pin == (1U<<14) ? LL_GPIO_PIN_14 :
//                                _pin == (1U<<15) ? LL_GPIO_PIN_15 :                                     
//                                 LL_GPIO_PIN_0);
//    
//   // return LL_pin;                                
//}



//constexpr void Gpio::get_GPIO_InitTypeDef(LL_GPIO_InitTypeDef* pLL_GPIO_Init)
//{
//    pLL_GPIO_Init->Pin = get_LL_pin(); // Init Done
//    pLL_GPIO_Init->Mode = 0;
//    pLL_GPIO_Init->Speed = LL_GPIO_MODE_OUTPUT_2MHz; 
//    pLL_GPIO_Init->OutputType = 0;
//    pLL_GPIO_Init->Pull = 0;
//    
//    if(pLL_GPIO_Init)
//    {
//        switch(_mode)
//        {
//            case INPUT_FLOATING : pLL_GPIO_Init->Mode  = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;                                  
//                                  break;
//            case INPUT_PULLUP   : pLL_GPIO_Init->Mode  = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;
//                                  pLL_GPIO_Init->Pull  = LL_GPIO_PULL_UP;
//                                  break;
//            case INPUT_PULLDOWN : pLL_GPIO_Init->Mode  = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;
//                                  pLL_GPIO_Init->Pull  = LL_GPIO_PULL_DOWN;
//                                  break;                                  
//            case ANALOG         : pLL_GPIO_Init->Mode  = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;
//                                  break;
//
//            case OUTPUT_OPENDRAIN_SLOW : pLL_GPIO_Init->Mode = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;
//                                  pLL_GPIO_Init->OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
//                                  pLL_GPIO_Init->Speed = LL_GPIO_MODE_OUTPUT_2MHz; 
//                                  break;   
//            case OUTPUT_PUSH_PULL_SLOW : pLL_GPIO_Init->Mode = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;
//                                  pLL_GPIO_Init->OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//                                  pLL_GPIO_Init->Speed = LL_GPIO_MODE_OUTPUT_2MHz; 
//                                  break; 
//            case OUTPUT_OPENDRAIN_MEDIUM : pLL_GPIO_Init->Mode = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;
//                                  pLL_GPIO_Init->OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
//                                  pLL_GPIO_Init->Speed = LL_GPIO_MODE_OUTPUT_10MHz; 
//                                  break;   
//            case OUTPUT_PUSH_PULL_MEDIUM : pLL_GPIO_Init->Mode = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;
//                                  pLL_GPIO_Init->OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//                                  pLL_GPIO_Init->Speed = LL_GPIO_MODE_OUTPUT_10MHz; 
//                                  break;
//            case OUTPUT_OPENDRAIN_FAST : pLL_GPIO_Init->Mode = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;
//                                  pLL_GPIO_Init->OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
//                                  pLL_GPIO_Init->Speed = LL_GPIO_MODE_OUTPUT_50MHz; 
//                                  break;   
//            case OUTPUT_PUSH_PULL_FAST : pLL_GPIO_Init->Mode = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;
//                                  pLL_GPIO_Init->OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//                                  pLL_GPIO_Init->Speed = LL_GPIO_MODE_OUTPUT_50MHz; 
//                                  break;
//            case AF_OPENDRAIN   : pLL_GPIO_Init->Mode = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;
//                                  break;   
//            case AF_PUSH_PULL   : pLL_GPIO_Init->Mode = LL_GPIO_MODE_INPUT;
//                                  pLL_GPIO_Init->Mode |= LL_GPIO_MODE_FLOATING;
//                                  break;                                  
//            default             : break;
//        }
//        
//    }
//                     
//}
//
//constexpr uint32_t Gpio::get_LL_mode()
//{
//    uint32_t mode=0;
//    
//    switch(_mode)
//    {
//        case INPUT_FLOATING :           mode  = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;                                  
//                                        break;
//        case INPUT_PULLUP   :           mode  = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;
//        case INPUT_PULLDOWN :           mode  = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;                                  
//        case ANALOG :                   mode  = LL_GPIO_MODE_INPUT;
//                                        break;    
//        case OUTPUT_OPENDRAIN_SLOW :    mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;   
//        case OUTPUT_PUSH_PULL_SLOW :    mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break; 
//        case OUTPUT_OPENDRAIN_MEDIUM :  mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;   
//        case OUTPUT_PUSH_PULL_MEDIUM :  mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;
//        case OUTPUT_OPENDRAIN_FAST :    mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;   
//        case OUTPUT_PUSH_PULL_FAST :    mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;
//        case AF_OPENDRAIN :             mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;   
//        case AF_PUSH_PULL :             mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;                                  
//        default :                       break;
//    }  
//    
//    return mode;                                
//}
//
//constexpr uint32_t Gpio::get_LL_speed()
//{
//    uint32_t mode=0;
//    
//    switch(_mode)
//    {
//        case INPUT_FLOATING :           mode  = LL_GPIO_MODE_FLOATING;
//        break;
//        case INPUT_PULLUP   :           mode  = LL_GPIO_MODE_INPUT;
//        break;
//        case INPUT_PULLDOWN :           mode  = LL_GPIO_MODE_INPUT;
//        break;                                  
//        case ANALOG :                   mode  = LL_GPIO_MODE_INPUT;
//                                        break;    
//        case OUTPUT_OPENDRAIN_SLOW :    mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;   
//        case OUTPUT_PUSH_PULL_SLOW :    mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break; 
//        case OUTPUT_OPENDRAIN_MEDIUM :  mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;   
//        case OUTPUT_PUSH_PULL_MEDIUM :  mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;
//        case OUTPUT_OPENDRAIN_FAST :    mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;   
//        case OUTPUT_PUSH_PULL_FAST :    mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;
//        case AF_OPENDRAIN :             mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;   
//        case AF_PUSH_PULL :             mode = LL_GPIO_MODE_INPUT;
//                                        mode |= LL_GPIO_MODE_FLOATING;
//                                        break;                                  
//        default :                       break;
//    }  
//    
//    return mode;                                
//}
//


                         


}


#endif //Gpio_h



