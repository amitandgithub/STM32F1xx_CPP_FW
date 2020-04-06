/******************
** CLASS: RotaryEncoderPoll
**
** DESCRIPTION:
**  Implements the Rotary Encoder class using polling
**
** CREATED: 31/10/2019, by Amit Chaudhary
**
** FILE: RotaryEncoderPoll.h
**
******************/
#ifndef RotaryEncoderPoll_h
#define RotaryEncoderPoll_h

#include "BtnPolling_t.h"

#define CW_PARAMS   CwPort,CwPin,INPUT_PULLUP,1
#define CCW_PARAMS  CCwPort,CCwPin,INPUT_PULLUP,0
#define SW_PARAMS   SwPort, SwPin,INPUT_PULLUP,0
namespace BSP
{
  template<Port_t CwPort, PIN_t CwPin, Port_t CCwPort, PIN_t CCwPin, Port_t SwPort, PIN_t SwPin, Callback_t OnCw = nullptr, Callback_t OnCCw = nullptr, Callback_t SwOnRise = nullptr, Callback_t SwOnFall = nullptr>
    class RotaryEncoderPoll //: public Callback
    {
    public:
      
      using SwBtnState_t = typename BSP::BtnPolling_t<SwPort,SwPin,INPUT_PULLUP,0>::BtnState_t;
      typedef enum
      {
        NOT_ROTATING,
        ROTATING_CLOCKWISE,
        ROTATING_ANTI_CLOCKWISE,
        BTN_IDLE,
        BTN_RISE,
        BTN_FALL
      }RotaryState_t;
      
      void    HwInit();
      
      void    Run();
      
      RotaryState_t   getRotaryState();
      
      RotaryState_t    getButtonState();
      
      //virtual void CallbackFunction();
      
    private:
      RotaryState_t RotaryState;
      RotaryState_t SwBtnState;
      BtnPolling_t<CW_PARAMS>        m_Cw;
      BtnPolling_t<CCW_PARAMS>       m_CCw;
      BtnPolling_t<SW_PARAMS>        m_Sw;        
    };
  
  template<Port_t CwPort, PIN_t CwPin, Port_t CCwPort, PIN_t CCwPin, Port_t SwPort, PIN_t SwPin, Callback_t OnCw , Callback_t OnCCw , Callback_t SwOnRise , Callback_t SwOnFall >
    void RotaryEncoderPoll <CwPort,CwPin,CCwPort,CCwPin,SwPort,SwPin,OnCw,OnCCw,SwOnRise,SwOnFall>::
  HwInit()
  {
    m_Cw.HwInit();
    m_CCw.HwInit();
    m_Sw.HwInit();   
    RotaryState = NOT_ROTATING;
    SwBtnState = BTN_IDLE;
  }
  
  template<Port_t CwPort, PIN_t CwPin, Port_t CCwPort, PIN_t CCwPin, Port_t SwPort, PIN_t SwPin, Callback_t OnCw , Callback_t OnCCw , Callback_t SwOnRise , Callback_t SwOnFall >
    void RotaryEncoderPoll <CwPort,CwPin,CCwPort,CCwPin,SwPort,SwPin,OnCw,OnCCw,SwOnRise,SwOnFall>::
  Run()
  {
    typename BSP::BtnPolling_t<CW_PARAMS>::BtnState_t CwState       =  m_Cw.Run();
    //typename BSP::BtnPolling_t<CCwPort,CCwPin,INPUT_PULLUP,0>::BtnState_t CCwState    =  m_CCw.Run();
    typename BSP::BtnPolling_t<SW_PARAMS>::BtnState_t SwState       =  m_Sw.Run(); 
    
    if( (CwState == BtnPolling_t<CW_PARAMS>::BTN_FALL) )
    {
      if(m_CCw.Read() == BSP::BtnPolling_t<CCW_PARAMS>::BTN_HIGH)
      {
        if(OnCCw) OnCCw->CallbackFunction(); 
        RotaryState = ROTATING_ANTI_CLOCKWISE;
      }
      else
      {
        if(OnCw) OnCw->CallbackFunction();        
        RotaryState = ROTATING_CLOCKWISE; 
      }
    }  
    
    if(SwState == BtnPolling_t<SW_PARAMS>::BTN_FALL)
    {
      if(SwOnFall) SwOnFall->CallbackFunction();
      SwBtnState = BTN_FALL;
    }
    else if(SwState == BtnPolling_t<SW_PARAMS>::BTN_RISE)
    {
      if(SwOnRise) SwOnRise->CallbackFunction();
      SwBtnState = BTN_RISE;
    }      
  }  
  
  template<Port_t CwPort, PIN_t CwPin, Port_t CCwPort, PIN_t CCwPin, Port_t SwPort, PIN_t SwPin, Callback_t OnCw , Callback_t OnCCw , Callback_t SwOnRise , Callback_t SwOnFall >
    RotaryEncoderPoll <CwPort,CwPin,CCwPort,CCwPin,SwPort,SwPin,OnCw,OnCCw,SwOnRise,SwOnFall>::
  RotaryState_t RotaryEncoderPoll <CwPort,CwPin,CCwPort,CCwPin,SwPort,SwPin,OnCw,OnCCw,SwOnRise,SwOnFall>::
  getRotaryState()
  {
    RotaryEncoderPoll <CwPort,CwPin,CCwPort,CCwPin,SwPort,SwPin,OnCw,OnCCw,SwOnRise,SwOnFall>::RotaryState_t _RotaryState;
    _RotaryState = RotaryState;
    RotaryState = NOT_ROTATING; // Reset-on-read
    return _RotaryState;
  }
  
  template<Port_t CwPort, PIN_t CwPin, Port_t CCwPort, PIN_t CCwPin, Port_t SwPort, PIN_t SwPin, Callback_t OnCw , Callback_t OnCCw , Callback_t SwOnRise , Callback_t SwOnFall >
    RotaryEncoderPoll <CwPort,CwPin,CCwPort,CCwPin,SwPort,SwPin,OnCw,OnCCw,SwOnRise,SwOnFall>::
  RotaryState_t RotaryEncoderPoll <CwPort,CwPin,CCwPort,CCwPin,SwPort,SwPin,OnCw,OnCCw,SwOnRise,SwOnFall>::
  getButtonState()
  {
    RotaryEncoderPoll <CwPort,CwPin,CCwPort,CCwPin,SwPort,SwPin,OnCw,OnCCw,SwOnRise,SwOnFall>::RotaryState_t _SwBtnState;
    _SwBtnState = SwBtnState; // Reset-on-read
    SwBtnState = BTN_IDLE;
    return _SwBtnState;
  }
}
#endif //RotaryEncoderPoll_h

