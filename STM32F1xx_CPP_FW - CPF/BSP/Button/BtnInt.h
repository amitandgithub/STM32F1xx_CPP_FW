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
    
    using Pin_t = HAL::Gpio::Pin_t;
    using Status_t = uint32_t;
    
    typedef enum
    {
        BTN_PULL_UP,
        BTN_PULL_DOWN
    }Pull_t;
    
    BtnInt(Pin_t Pin, Pull_t Pull = BTN_PULL_DOWN, Callback_t L2HCallback = nullptr, Callback_t H2LCallback = nullptr );
    ~BtnInt(){};
    Status_t HwInit(void *pInitStruct = nullptr);
	bool Read();
    bool getState();
    uint32_t _getMillis();
    void RegisterHandler(Callback_t L2HCallback, Callback_t H2LCallback);
    virtual void CallbackFunction();
    
private:
     HAL::GpioInput      _BtnPin;
     Callback_t          _L2HCallback;
     Callback_t          _H2LCallback;
};

inline void BtnInt::RegisterHandler(Callback_t L2HCallback, Callback_t H2LCallback)
{
    _L2HCallback = L2HCallback;
    _H2LCallback = H2LCallback;    
}

inline bool BtnInt::Read()
{
    return  _BtnPin.Read();
}

inline uint32_t BtnInt::_getMillis()
{
    return HAL_GetTick();
}


}



#endif //BtnInt_h
