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

#include "BtnPolling.h"

namespace BSP
{
    template<Port_t CwPort, PIN_t CwPin, Port_t CCwPort, PIN_t CCwPin, Port_t SwPort, PIN_t SwPin, Callback_t OnCw = nullptr, Callback_t OnCCw = nullptr, Callback_t SwOnRise = nullptr, Callback_t SwOnFall = nullptr>
    class RotaryEncoderPoll : public Callback
    {
    public:
        
        typedef enum
        {
            NOT_ROTATING,
            ROTATING_CLOCKWISE,
            ROTATING_ANTI_CLOCKWISE
        }RotaryState_t;
        
        void    HwInit();
        
        void    Run();
        
        RotaryState_t   getRotaryState();
        
        bool    getButtonState(){return true;}
        
        virtual void CallbackFunction();
        
    private:
        BtnPolling<CwPort,CwPin,INPUT_PULLUP,0,nullptr,nullptr>         m_Cw;
        BtnPolling<CCwPort,CCwPin,INPUT_PULLUP,0,nullptr,nullptr>       m_CCw;
        BtnPolling<SwPort,SwPin,INPUT_PULLUP,0,SwOnRise,SwOnFall>       m_Sw;        
    };
    
         template<Port_t CwPort, PIN_t CwPin, Port_t CCwPort, PIN_t CCwPin, Port_t SwPort, PIN_t SwPin, Callback_t OnCw , Callback_t OnCCw , Callback_t SwOnRise , Callback_t SwOnFall >
    void RotaryEncoderPoll <CwPort,CwPin,CCwPort,CCwPin,SwPort,SwPin,OnCw,OnCCw,SwOnRise,SwOnFall>::
    HwInit()
    {
      m_Cw.Run();
      m_CCw.Run();
      m_Sw.Run();
    }
    
     template<Port_t CwPort, PIN_t CwPin, Port_t CCwPort, PIN_t CCwPin, Port_t SwPort, PIN_t SwPin, Callback_t OnCw , Callback_t OnCCw , Callback_t SwOnRise , Callback_t SwOnFall >
    void RotaryEncoderPoll <CwPort,CwPin,CCwPort,CCwPin,SwPort,SwPin,OnCw,OnCCw,SwOnRise,SwOnFall>::
    Run()
    {
      m_Cw.HwInit();

      m_CCw.HwInit();
      m_Sw.HwInit();
    }
    
    
    template<Port_t CwPort, PIN_t CwPin, Port_t CCwPort, PIN_t CCwPin, Port_t SwPort, PIN_t SwPin, Callback_t OnCw , Callback_t OnCCw , Callback_t SwOnRise , Callback_t SwOnFall >
    void RotaryEncoderPoll <CwPort,CwPin,CCwPort,CCwPin,SwPort,SwPin,OnCw,OnCCw,SwOnRise,SwOnFall>::
    CallbackFunction()
    {
      if(m_CCw.Read() == false)
      {
        if(OnCw) 
          OnCw->CallbackFunction();        
      }
      else
      {
        if(OnCCw) 
          OnCCw->CallbackFunction();        
      }
    }
}
#endif //RotaryEncoderPoll_h

