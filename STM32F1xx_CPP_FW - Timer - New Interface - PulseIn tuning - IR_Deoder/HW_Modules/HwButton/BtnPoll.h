/******************
** CLASS: BtnPoll
**
** DESCRIPTION:
**  Implements the basic button functionality with polling method
**
** CREATED: 1/31/2019, by Amit Chaudhary
** Modified for new Gpio class - 15 Jan 2020
** FILE: BtnPoll.h
**
******************/
#ifndef BtnPoll_h
#define BtnPoll_h

#include "GpioInput.h"
#define BTN_DEBOUNCE_REQUIRED
namespace BSP
{
  
  using namespace HAL;   
  
  class BtnPoll 
  {
  public:
    typedef Callback* Callback_t;
    
    typedef enum : uint8_t
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
    
    BtnPoll(Port_t Port, PIN_t Pin, uint8_t Debounce = 0);
    
    ~BtnPoll(){};
    
    Status_t HwInit(Mode_t Mode = INPUT_PULLUP,IntEdge_t Edge = NO_INTERRUPT, Callback_t OnRise = nullptr, Callback_t OnFall = nullptr);
    
    void Run();
    
    BtnState_t Read(){return ( m_BtnPin.Read() ? BTN_HIGH : BTN_LOW );}
      
    bool getState(){return m_previousState?true:false;}
    
    void RegisterHandler(Callback_t OnRise, Callback_t OnFall){m_OnRiseCallback = OnRise; m_OnFallCallback = OnFall;}
    
  private:
    HAL::GpioInput      m_BtnPin;
    Callback_t          m_OnRiseCallback;
    Callback_t          m_OnFallCallback;
    uint32_t            m_startMillis;    
    BtnState_t          m_previousState;
    BtnState_t          m_currentState;
    uint8_t             m_debounce;
    
  };
  
}



#endif //BtnPoll_h
