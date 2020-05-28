
/******************
** FILE: DigitalOut.h
**
** DESCRIPTION:
**  Gpio related functionality
**
** CREATED: 28/7/2019, by Amit Chaudhary
******************/
#ifndef DigitalInOut_h
#define DigitalInOut_h


#include"chip_peripheral.h"
#include"InterruptManager.h"
#include"CPP_HAL.h"
#include"ClockManager.h"

extern HAL::InterruptManager InterruptManagerInstance;
extern HAL::Callback* g_GpioCallbacks[16];

typedef enum
{
  PORTA = GPIOA_BASE,
  PORTB = GPIOB_BASE,
  PORTC = GPIOC_BASE,
  PORTD = GPIOD_BASE,
}Port_t;

typedef uint16_t PIN_t;

typedef enum  : uint8_t
{
  INPUT_FLOATING,
  INPUT_PULLUP,
  INPUT_PULLDOWN,
  INPUT_FLOATING_INTERRUPT,
  INPUT_PULLUP_INTERRUPT,
  OUTPUT_OPEN_DRAIN,
  OUTPUT_OPEN_DRAIN_PUSH_PULL,
  OUTPUT_OPEN_DRAIN_FAST,
  OUTPUT_OPEN_DRAIN_PUSH_PULL_FAST,
  OUTPUT_AF_PUSH_PULL,
  OUTPUT_AF_OPEN_DRAIN,
  ANALOG,
}Mode_t;

typedef enum
{
  //INTERRUPT_ON_FALLING_LOW,
  INTERRUPT_ON_RISING,
  INTERRUPT_ON_FALLING,
  INTERRUPT_ON_RISING_FALLING,
  NO_INTERRUPT
}IntEdge_t;

typedef HAL::Callback* Callback_t;


template<Port_t _GPIOx>
void Gpio_ClockEnable()
{   
  if((GPIO_TypeDef*)_GPIOx == GPIOA )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOA);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOB )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOB);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOC )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOC);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOD )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOD);
  }
#if defined(GPIOE)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOE )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOE);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOE);
  }
#endif
#if defined(GPIOF)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOF )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOG);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOG);
  }
#endif
}
template<Port_t _GPIOx>
void Gpio_ClockDisable()
{   
  if((GPIO_TypeDef*)_GPIOx == GPIOA )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOA);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOB )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOB);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOC )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOC);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOD )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOD);
  }
#if defined(GPIOE)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOE )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOE);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOE);
  }
#endif
#if defined(GPIOF)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOF )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOG);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOG);
  }
#endif  
}

__STATIC_INLINE IRQn_Type Gpio_get_pin_IRQn(uint8_t pin)
{                          
  IRQn_Type LL_PINS[16] = 
  {
    EXTI0_IRQn ,
    EXTI1_IRQn,  
    EXTI2_IRQn,
    EXTI3_IRQn,
    EXTI4_IRQn,
    EXTI9_5_IRQn,
    EXTI9_5_IRQn,
    EXTI9_5_IRQn,
    EXTI9_5_IRQn,
    EXTI9_5_IRQn,
    EXTI15_10_IRQn ,
    EXTI15_10_IRQn ,
    EXTI15_10_IRQn ,
    EXTI15_10_IRQn ,
    EXTI15_10_IRQn ,
    EXTI15_10_IRQn ,       
  };
  
  return LL_PINS[pin];  
}

template<PIN_t _pin>
IRQn_Type Gpio_get_pin_IRQn()
{
  IRQn_Type LL_IRQns[16] = 
  {
    EXTI0_IRQn ,
    EXTI1_IRQn,  
    EXTI2_IRQn,
    EXTI3_IRQn,
    EXTI4_IRQn,
    EXTI9_5_IRQn,
    EXTI9_5_IRQn,
    EXTI9_5_IRQn,
    EXTI9_5_IRQn,
    EXTI9_5_IRQn,
    EXTI15_10_IRQn ,
    EXTI15_10_IRQn ,
    EXTI15_10_IRQn ,
    EXTI15_10_IRQn ,
    EXTI15_10_IRQn ,
    EXTI15_10_IRQn ,       
  };
  
  return LL_IRQns[_pin]; 
}

__STATIC_INLINE uint32_t get_LL_pin(uint8_t pin)
{
  uint32_t LL_PINS[16] = 
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
  
  return LL_PINS[pin];                                
}

__STATIC_INLINE void Gpio_ClockEnable(Port_t _GPIOx)
{   
  if((GPIO_TypeDef*)_GPIOx == GPIOA )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOA);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOB )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOB);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOC )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOC);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOD )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOD);
  }
#if defined(GPIOE)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOE )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOE);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOE);
  }
#endif
#if defined(GPIOF)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOF )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOG);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOG);
  }
#endif
}
__STATIC_INLINE void Gpio_ClockDisable(Port_t _GPIOx)
{   
  if((GPIO_TypeDef*)_GPIOx == GPIOA )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOA);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOB )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOB);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOC )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOC);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOD )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOD);
  }
#if defined(GPIOE)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOE )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOE);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOE);
  }
#endif
#if defined(GPIOF)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOF )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOG);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOG);
  }
#endif  
}


/* Port A Pin definitions */

#define A0	PORTA,0
#define A1	PORTA,1
#define A2	PORTA,2
#define A3	PORTA,3
#define A4	PORTA,4
#define A5	PORTA,5
#define A6	PORTA,6
#define A7	PORTA,7
#define A8	PORTA,8
#define A9	PORTA,9
#define A10 PORTA,10
#define A11 PORTA,11
#define A12 PORTA,12
#define A13 PORTA,13
#define A14 PORTA,14
#define A15 PORTA,15

/* Port B Pin definitions */

#define B0	PORTB,0
#define B1	PORTB,1
#define B2	PORTB,2
#define B3	PORTB,3
#define B4	PORTB,4
#define B5	PORTB,5
#define B6	PORTB,6
#define B7	PORTB,7
#define B8	PORTB,8
#define B9	PORTB,9
#define B10 PORTB,10
#define B11 PORTB,11
#define B12 PORTB,12
#define B13 PORTB,13
#define B14 PORTB,14
#define B15 PORTB,15

/* Port C Pin definitions */

#define C0	PORTC,0
#define C1	PORTC,1
#define C2	PORTC,2
#define C3	PORTC,3
#define C4	PORTC,4
#define C5	PORTC,5
#define C6	PORTC,6
#define C7	PORTC,7
#define C8	PORTC,8
#define C9	PORTC,9
#define C10 PORTC,10
#define C11 PORTC,11
#define C12 PORTC,12
#define C13 PORTC,13
#define C14 PORTC,14
#define C15 PORTC,15

/* Port D Pin definitions */

#define D0      PORTD,0
#define D1	PORTD,1
#define D2	PORTD,2
#define D3	PORTD,3
#define D4	PORTD,4
#define D5	PORTD,5
#define D6	PORTD,6
#define D7	PORTD,7
#define D8	PORTD,8
#define D9	PORTD,9
#define D10 PORTD,10
#define D11 PORTD,11
#define D12 PORTD,12
#define D13 PORTD,13
#define D14 PORTD,14
#define D15 PORTD,15

#endif // DigitalInOut_h
