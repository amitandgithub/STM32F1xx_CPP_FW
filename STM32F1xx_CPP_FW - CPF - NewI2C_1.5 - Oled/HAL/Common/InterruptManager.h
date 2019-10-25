/*
 * InterruptManager.h
 *
 *  Created on: 3-Mar-2019
 *      Author: Amit Chaudhary
 */

#ifndef InterruptManager_h
#define InterruptManager_h

#include "CPP_HAL.h"
#include "InterruptSource.h"

namespace HAL
{


class InterruptManager
{

public:
    static const uint32_t DEVICE_FLASH_START = 0x08000000;
    static const uint32_t TOTAL_INTERRUPT_VECTORS = 64;
    typedef HAL::HAL_Status_t Status_t;
	typedef void(*ISR)(void);
    
	typedef enum {DISABLE = 0, ENABLE = !DISABLE}InterruptState;
	
    typedef IRQn_Type IRQn_t;
    
    InterruptManager(){};
    
    ~InterruptManager(){};
    
    bool RegisterDeviceInterrupt(IRQn_t IRQNumber, uint32_t priority, InterruptSource* const pInterruptSource);
    
    bool UnRegisterDeviceInterrupt(IRQn_t IRQNumber);
    
    Status_t EnableInterrupt(IRQn_t IRQNumber);
    
    inline void ClearInterrupt(IRQn_t IRQNumber);
    
    //static InterruptManager* GetInstance(void);    
    
    static InterruptSource*    _InterruptSourceArray[TOTAL_INTERRUPT_VECTORS]; // USBWakeUp_IRQn+16
    
private: 


};

void InterruptManager::ClearInterrupt(IRQn_t IRQNumber)
{
   NVIC_ClearPendingIRQ(IRQNumber); 
}



}
#endif /* InterruptManager_h */
