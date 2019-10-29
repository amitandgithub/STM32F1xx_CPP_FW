

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


typedef enum
{
  PORTA = GPIOA_BASE,
  PORTB = GPIOB_BASE,
  PORTC = GPIOC_BASE,
  PORTD = GPIOD_BASE,
}Port_t;

typedef enum
{
  PIN_0 = LL_GPIO_PIN_0,
  PIN_1 = LL_GPIO_PIN_1,
  PIN_2 = LL_GPIO_PIN_2,
  PIN_3 = LL_GPIO_PIN_3,
  PIN_4 = LL_GPIO_PIN_4,
  PIN_5 = LL_GPIO_PIN_5,
  PIN_6 = LL_GPIO_PIN_6,
  PIN_7 = LL_GPIO_PIN_7,
  PIN_8 = LL_GPIO_PIN_8,
  PIN_9 = LL_GPIO_PIN_9,
  PIN_10 = LL_GPIO_PIN_10,
  PIN_11 = LL_GPIO_PIN_11,
  PIN_12 = LL_GPIO_PIN_12,
  PIN_13 = LL_GPIO_PIN_13,
  PIN_14 = LL_GPIO_PIN_14,
  PIN_15 = LL_GPIO_PIN_15,
}PIN_t;

typedef enum
{
  INPUT_FLOATING,
  INPUT_PULLUP,
  INPUT_PULLDOWN,
  INPUT_FLOATING_INTERRUPT,
  INPUT_PULLUP_INTERRUPT,
  OPEN_DRAIN,
  OPEN_DRAIN_PUSH_PULL,
  OPEN_DRAIN_FAST,
  OPEN_DRAIN_PUSH_PULL_FAST,
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

typedef Callback* Callback_t;

template<Port_t _GPIOx>
void Gpio_ClockEnable()
{
   
    if((GPIO_TypeDef*)_GPIOx == GPIOA )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    }
    else  if((GPIO_TypeDef*)_GPIOx == GPIOB )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }
    else  if((GPIO_TypeDef*)_GPIOx == GPIOC )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    }
    else  if((GPIO_TypeDef*)_GPIOx == GPIOD )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    }
#if defined(GPIOE)
    else  if((GPIO_TypeDef*)_GPIOx == GPIOE )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOE);
    }
#endif
#if defined(GPIOF)
    else  if((GPIO_TypeDef*)_GPIOx == GPIOF )
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOG);
    }
#endif
}
template<Port_t _GPIOx>
void Gpio_ClockDisable()
{
   
    if((GPIO_TypeDef*)_GPIOx == GPIOA )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    }
    else  if((GPIO_TypeDef*)_GPIOx == GPIOB )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }
    else  if((GPIO_TypeDef*)_GPIOx == GPIOB )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    }
    else  if((GPIO_TypeDef*)_GPIOx == GPIOD )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    }
#if defined(GPIOE)
    else  if((GPIO_TypeDef*)_GPIOx == GPIOE )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOE);
    }
#endif
#if defined(GPIOF)
    else  if((GPIO_TypeDef*)_GPIOx == GPIOF )
    {
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOG);
    }
#endif
    
}
    template<PIN_t _pin>
    uint32_t Gpio_get_pin_pos()
    {
        return  (   _pin == LL_GPIO_PIN_0 ? (1U<<0)  :
                    _pin == LL_GPIO_PIN_1  ? (1U<<1) :  
                    _pin == LL_GPIO_PIN_2  ? (1U<<2) :
                    _pin == LL_GPIO_PIN_3  ? (1U<<3) :
                    _pin == LL_GPIO_PIN_4  ? (1U<<4) :
                    _pin == LL_GPIO_PIN_5  ? (1U<<5) :
                    _pin == LL_GPIO_PIN_6  ? (1U<<6) :
                    _pin == LL_GPIO_PIN_7  ? (1U<<7) :
                    _pin == LL_GPIO_PIN_8  ? (1U<<8) :
                    _pin == LL_GPIO_PIN_9  ? (1U<<9) :
                    _pin == LL_GPIO_PIN_10 ? (1U<<10) :
                    _pin == LL_GPIO_PIN_11 ? (1U<<11) :
                    _pin == LL_GPIO_PIN_12 ? (1U<<12) :
                    _pin == LL_GPIO_PIN_13 ? (1U<<13) :
                    _pin == LL_GPIO_PIN_14 ? (1U<<14) :
                    _pin == LL_GPIO_PIN_15 ? (1U<<15) :                                     
                    1);                            
    }

    template<PIN_t _pin>
    IRQn_Type Gpio_get_pin_IRQn()
    {
        return  (   _pin == LL_GPIO_PIN_0  ? EXTI0_IRQn :
                    _pin == LL_GPIO_PIN_1  ? EXTI1_IRQn:  
                    _pin == LL_GPIO_PIN_2  ? EXTI2_IRQn:
                    _pin == LL_GPIO_PIN_3  ? EXTI3_IRQn:
                    _pin == LL_GPIO_PIN_4  ? EXTI4_IRQn:
                    _pin == LL_GPIO_PIN_5  ? EXTI9_5_IRQn:
                    _pin == LL_GPIO_PIN_6  ? EXTI9_5_IRQn:
                    _pin == LL_GPIO_PIN_7  ? EXTI9_5_IRQn:
                    _pin == LL_GPIO_PIN_8  ? EXTI9_5_IRQn:
                    _pin == LL_GPIO_PIN_9  ? EXTI9_5_IRQn:
                    _pin == LL_GPIO_PIN_10 ? EXTI15_10_IRQn :
                    _pin == LL_GPIO_PIN_11 ? EXTI15_10_IRQn :
                    _pin == LL_GPIO_PIN_12 ? EXTI15_10_IRQn :
                    _pin == LL_GPIO_PIN_13 ? EXTI15_10_IRQn :
                    _pin == LL_GPIO_PIN_14 ? EXTI15_10_IRQn :
                    _pin == LL_GPIO_PIN_15 ? EXTI15_10_IRQn :                                     
                    EXTI0_IRQn);                           
    }

//extern GpioFptr_t EXTI_ISRs[];

/* Port A Pin definitions */

#define A0	PORTA,PIN_0
#define A1	PORTA,PIN_1
#define A2	PORTA,PIN_2
#define A3	PORTA,PIN_3
#define A4	PORTA,PIN_4
#define A5	PORTA,PIN_5
#define A6	PORTA,PIN_6
#define A7	PORTA,PIN_7
#define A8	PORTA,PIN_8
#define A9	PORTA,PIN_9
#define A10 PORTA,PIN_10
#define A11 PORTA,PIN_11
#define A12 PORTA,PIN_12
#define A13 PORTA,PIN_13
#define A14 PORTA,PIN_14
#define A15 PORTA,PIN_15

/* Port B Pin definitions */

#define B0	PORTB,PIN_0
#define B1	PORTB,PIN_1
#define B2	PORTB,PIN_2
#define B3	PORTB,PIN_3
#define B4	PORTB,PIN_4
#define B5	PORTB,PIN_5
#define B6	PORTB,PIN_6
#define B7	PORTB,PIN_7
#define B8	PORTB,PIN_8
#define B9	PORTB,PIN_9
#define B10 PORTB,PIN_10
#define B11 PORTB,PIN_11
#define B12 PORTB,PIN_12
#define B13 PORTB,PIN_13
#define B14 PORTB,PIN_14
#define B15 PORTB,PIN_15

/* Port C Pin definitions */

#define C0	PORTC,PIN_0
#define C1	PORTC,PIN_1
#define C2	PORTC,PIN_2
#define C3	PORTC,PIN_3
#define C4	PORTC,PIN_4
#define C5	PORTC,PIN_5
#define C6	PORTC,PIN_6
#define C7	PORTC,PIN_7
#define C8	PORTC,PIN_8
#define C9	PORTC,PIN_9
#define C10 PORTC,PIN_10
#define C11 PORTC,PIN_11
#define C12 PORTC,PIN_12
#define C13 PORTC,PIN_13
#define C14 PORTC,PIN_14
#define C15 PORTC,PIN_15

/* Port D Pin definitions */

#define D0	PORTD,PIN_0
#define D1	PORTD,PIN_1
#define D2	PORTD,PIN_2
#define D3	PORTD,PIN_3
#define D4	PORTD,PIN_4
#define D5	PORTD,PIN_5
#define D6	PORTD,PIN_6
#define D7	PORTD,PIN_7
#define D8	PORTD,PIN_8
#define D9	PORTD,PIN_9
#define D10 PORTD,PIN_10
#define D11 PORTD,PIN_11
#define D12 PORTD,PIN_12
#define D13 PORTD,PIN_13
#define D14 PORTD,PIN_14
#define D15 PORTD,PIN_15

#endif // DigitalInOut_h