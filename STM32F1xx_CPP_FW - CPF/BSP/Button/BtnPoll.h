/******************
** CLASS: BtnPoll
**
** DESCRIPTION:
**  Implements the basic button functionality with polling method
**
** CREATED: 1/31/2019, by Amit Chaudhary
**
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
    
    BtnPoll(Pin_t Pin, Pull_t Pull = BTN_PULL_DOWN, uint8_t Debounce = 3U, HALCallback_t L2HCallback = nullptr, HALCallback_t H2LCallback = nullptr );
    ~BtnPoll(){};
    Status_t HwInit(void *pInitStruct = nullptr);
	void RunStateMachine();
	BtnState_t Read();
    bool getState();
    uint32_t _getMillis();
    void RegisterHandler(HALCallback_t L2HCallback, HALCallback_t H2LCallback);
    
private:
     HAL::GpioInput         _BtnPin;
     BtnState_t             _previousState;
     BtnState_t             _currentState;
     uint8_t                _debounce;
     uint16_t               _startMillis;
     HALCallback_t          _L2HCallback;
     HALCallback_t          _H2LCallback;
};

inline void BtnPoll::RegisterHandler(HALCallback_t L2HCallback, HALCallback_t H2LCallback)
{
    _L2HCallback = L2HCallback;
    _H2LCallback = H2LCallback;    
}

inline BtnPoll::BtnState_t BtnPoll::Read()
{
    return ( _BtnPin.Read()? BTN_HIGH: BTN_LOW);
}

inline uint32_t BtnPoll::_getMillis()
{
    return HAL_GetTick();
}

inline bool BtnPoll::getState()
{
    return _previousState? true:false;
}

}



#endif //BtnPoll_h
