/******************
** CLASS: KeypadPoll
**
** DESCRIPTION:
**  KeypadPoll related functionality
**
** CREATED: 4/6/2019, by Amit Chaudhary
**
** FILE: KeypadPoll.h
**
******************/
#ifndef KeypadPoll_h
#define KeypadPoll_h

#include "BtnPoll.h"

namespace BSP
{
    
    template<uint32_t keys = 4>
        class KeypadPoll
        {
        public:
            KeypadPoll();
            ~KeypadPoll();
            typedef BtnPoll Btn_t ;      
            void AddKey(uint8_t keynumber,Btn_t* pBtn);
            void Run();
            
        private:
            static const uint32_t Total_keys = keys;
            Btn_t* _Keyes[Total_keys];
            
        };    
    
    template<uint32_t keys>
        KeypadPoll<keys>::KeypadPoll()
        {
            for(uint8_t i= 0; i<Total_keys; i++)
                _Keyes[i] = nullptr;
        }
    
    template<uint32_t keys>
        KeypadPoll<keys>::~KeypadPoll()
        {
            
        }
    
    template<uint32_t keys>
        void KeypadPoll<keys>::AddKey(uint8_t keynumber,Btn_t* pBtn)
        {
            if(pBtn && (keynumber < Total_keys))
            {
                pBtn->HwInit();
                _Keyes[keynumber] = pBtn;        
            }    
        }
    
    template<uint32_t keys>
        void KeypadPoll<keys>::Run()
        {
            for(uint8_t i= 0; i<Total_keys; i++)
            {
                if(_Keyes[i])
                {
                    _Keyes[i]->Run(); 
                }             
            }   
        }    
    
}
#endif //KeypadPoll_h
