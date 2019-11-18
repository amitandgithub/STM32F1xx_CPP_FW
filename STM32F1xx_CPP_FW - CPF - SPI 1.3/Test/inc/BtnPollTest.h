

#include"BtnPoll.h"

using namespace BSP;

void BtnPoll_Test()
{
    uint32_t size;
    size = sizeof(BtnPoll); /* 44 butrdssss*/ size++;
    
    BtnPoll BtnPoll_A8(Gpio::A8, BSP::BtnPoll::BTN_PULL_DOWN,3, &Rising,&Falling );
    BtnPoll_A8.HwInit();      
    
    while(1)
    {
      BtnPoll_A8.Run();  
    }
}
