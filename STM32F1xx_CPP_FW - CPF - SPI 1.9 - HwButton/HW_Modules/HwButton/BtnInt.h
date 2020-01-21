/******************
** CLASS: BtnInt
**
** DESCRIPTION:
**  Implements the basic button functionality with Interrupt method
**
** CREATED: 22/2/2019, by Amit Chaudhary
**
** FILE: BtnInt.h
**
******************/
#ifndef BtnInt_h
#define BtnInt_h

#include"GpioInput.h"
#include"Callback.h"
#define BTN_DEBOUNCE_REQUIRED
namespace BSP
{
  
  using namespace HAL;   
  
  class BtnInt : public Callback
  {
  public:
    
    typedef Callback* Callback_t;
    
    typedef enum
    {
      BTN_LOW,
      BTN_HIGH,
      BTN_IDLE,
      BTN_DEBOUNCE       
    }BtnState_t;
    
    using Status_t = uint32_t;
    
    typedef enum
    {
      BTN_PULL_UP,
      BTN_PULL_DOWN
    }Pull_t;
    
    BtnInt(Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t OnRise = nullptr, Callback_t OnFall = nullptr);
    
    ~BtnInt(){};
    
    Status_t HwInit();
    
    bool Read(){return m_BtnPin.Read();}
    
    void RegisterHandler(Callback_t OnRise, Callback_t OnFall){m_OnRiseCallback = OnRise; m_OnFallCallback = OnFall;}
    
    virtual void CallbackFunction();
    
  private:
    HAL::GpioInput      m_BtnPin;
    Callback_t          m_OnRiseCallback;
    Callback_t          m_OnFallCallback;
  };  
}

#endif //BtnInt_h
