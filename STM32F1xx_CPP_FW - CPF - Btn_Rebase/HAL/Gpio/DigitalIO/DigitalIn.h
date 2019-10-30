/******************
** CLASS: DigitalIn
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 8/9/2019, by Amit Chaudhary
**
** FILE: DigitalIn.h
**
******************/
#ifndef DigitalIn_h
#define DigitalIn_h


#include"DigitalInOut.h"


namespace HAL
{  
  
  template<Port_t Port, PIN_t Pin, Mode_t Mode = INPUT_PULLUP, IntEdge_t Edge = NO_INTERRUPT, Callback_t Callback = nullptr>
    class DigitalIn : public InterruptSource 
    {
    public:      
      void HwInit();
      
      void SetInputMode();
      
      void SetInterrupt();
      
      void SetEXTI_Intr();
      
      void RegisterCallback();
        
      bool Read(){ return (bool)(  (((GPIO_TypeDef*)Port)->IDR) & Gpio_get_pin_pos<Pin>() ); }
      
      virtual void ISR();
      
    private:
      static Callback_t m_GpioCallbacks[16];
    };
    
    template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t Callback>
      Callback_t  DigitalIn<Port,Pin,Mode,Edge,Callback>::m_GpioCallbacks[16];
    
    template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t Callback>
      void DigitalIn<Port,Pin,Mode,Edge,Callback>::HwInit()
      {
        SetInputMode();  
        
        if(Callback)
        {
          RegisterCallback();
        }      
      }  
    
    template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t Callback>
      void DigitalIn<Port,Pin,Mode,Edge,Callback>::SetEXTI_Intr()
      {      
        LL_EXTI_DisableEvent_0_31(Gpio_get_pin_pos<Pin>());
        LL_EXTI_EnableIT_0_31(Gpio_get_pin_pos<Pin>());
        
        switch(Edge)
        {
        case INTERRUPT_ON_RISING:
          LL_EXTI_DisableFallingTrig_0_31(Gpio_get_pin_pos<Pin>());
          LL_EXTI_EnableRisingTrig_0_31(Gpio_get_pin_pos<Pin>());
          break;
        case INTERRUPT_ON_FALLING:
          LL_EXTI_DisableRisingTrig_0_31(Gpio_get_pin_pos<Pin>());
          LL_EXTI_EnableFallingTrig_0_31(Gpio_get_pin_pos<Pin>());
          break;
        case INTERRUPT_ON_RISING_FALLING:
          LL_EXTI_EnableFallingTrig_0_31(Gpio_get_pin_pos<Pin>());
          LL_EXTI_EnableRisingTrig_0_31(Gpio_get_pin_pos<Pin>());
          break;
        default: break;  
        }  
      }
    
    template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t Callback>
      void DigitalIn<Port,Pin,Mode,Edge,Callback>::SetInputMode()
      {
        Gpio_ClockEnable<Port>();
        
        switch(Mode)
        {
        case INPUT_FLOATING:
          LL_GPIO_SetPinMode((GPIO_TypeDef*)Port,Pin,LL_GPIO_MODE_FLOATING);
          break;
        case ANALOG:
          LL_GPIO_SetPinMode((GPIO_TypeDef*)Port,Pin,LL_GPIO_MODE_ANALOG);
          break;
        case INPUT_PULLUP:
          LL_GPIO_SetPinMode((GPIO_TypeDef*)Port,Pin,LL_GPIO_MODE_INPUT);
          LL_GPIO_SetPinPull((GPIO_TypeDef*)Port,Pin,LL_GPIO_PULL_UP);
          break;
        case INPUT_PULLDOWN:
          LL_GPIO_SetPinMode((GPIO_TypeDef*)Port,Pin,LL_GPIO_MODE_INPUT);
          LL_GPIO_SetPinPull((GPIO_TypeDef*)Port,Pin,LL_GPIO_PULL_DOWN);
          break;
        default: break;  
        }  
      }
    template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t Callback>
      void DigitalIn<Port,Pin,Mode,Edge,Callback>::ISR()
      {
        // if we don't check EXTI->PR for 0,
        // in that case POSITION_VAL(EXTI->PR) will return 32, which will
        // try to access with 32 index _GpioCallbacks[ExternalInterrupt]
        // and will crash because array is only 16 elements
        if(EXTI->PR)
        {
          uint32_t ExternalInterrupt = (uint32_t)POSITION_VAL(EXTI->PR);               
          m_GpioCallbacks[ExternalInterrupt]->CallbackFunction();                
          LL_EXTI_ClearFlag_0_31( (uint32_t)((uint32_t)1U)<<ExternalInterrupt);
        }
      }
    
    template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t Callback>
      void DigitalIn<Port,Pin,Mode,Edge,Callback>::RegisterCallback()
      {
        m_GpioCallbacks[POSITION_VAL(Gpio_get_pin_pos<Pin>())] = Callback;
        
        SetEXTI_Intr();
        
        InterruptManagerInstance.RegisterDeviceInterrupt(Gpio_get_pin_IRQn<Pin>(),0,this);
      }
    
    
    
} 
#endif //DigitalIn_h