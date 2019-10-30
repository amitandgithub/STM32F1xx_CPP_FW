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

#include"DigitalIn.h"
#include"Callback.h"
#define BTN_DEBOUNCE_REQUIRED
namespace BSP
{
    
using namespace HAL;   

template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t OnRise = nullptr, Callback_t OnFall = nullptr>
class BtnInt : public Callback
{
public:
    
  //typedef HAL::Callback* Callback_t;
    
    typedef enum
	{
        BTN_LOW,
        BTN_HIGH,
        BTN_IDLE,
        BTN_DEBOUNCE       
	}BtnState_t;
    
   // using Pin_t = HAL::Gpio::Pin_t;
    using Status_t = uint32_t;
    //using Btn_t = HAL::DigitalIn<A11,INPUT_PULLUP,INTERRUPT_ON_FALLING,this> ;
    typedef enum
    {
        BTN_PULL_UP,
        BTN_PULL_DOWN
    }Pull_t;
    
   // BtnInt(Pin_t Pin, Pull_t Pull = BTN_PULL_DOWN, Callback_t L2HCallback = nullptr, Callback_t H2LCallback = nullptr );
  //  ~BtnInt(){};
    Status_t HwInit(void *pInitStruct = nullptr);
    
    bool Read(){return m_BtnPin.Read();};
    
    bool getState();
    
    uint32_t _getMillis(){ return HAL_GetTick();}
    
    virtual void CallbackFunction();
    
private:
     HAL::DigitalIn<Port, Pin ,Mode ,Edge >     m_BtnPin;

};



}



#endif //BtnInt_h
