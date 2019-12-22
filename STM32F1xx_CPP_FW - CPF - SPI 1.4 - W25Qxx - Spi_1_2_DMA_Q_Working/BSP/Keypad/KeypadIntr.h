/******************
** CLASS: KeypadIntr
**
** DESCRIPTION:
**  KeypadIntr related functionality
**
** CREATED: 4/7/2019, by Amit Chaudhary
**
** FILE: KeypadIntr.h
**
******************/
#ifndef KeypadIntr_h
#define KeypadIntr_h

#include "BtnInt.h"

namespace BSP
{
    
    template<uint32_t keys = 4>
        class KeypadIntr
        {
        public:
            KeypadIntr();
            ~KeypadIntr();
            typedef BtnInt Btn_t ;      
            void AddKey(uint8_t keynumber,Btn_t* pBtn);
            void Run();
            
        private:
            static const uint32_t Total_keys = keys;
            Btn_t* _Keyes[Total_keys];
            
        };    
    
    template<uint32_t keys>
        KeypadIntr<keys>::KeypadIntr()
        {
            for(uint8_t i= 0; i<Total_keys; i++)
                _Keyes[i] = nullptr;
        }
    
    template<uint32_t keys>
        KeypadIntr<keys>::~KeypadIntr()
        {
            
        }
    
    template<uint32_t keys>
        void KeypadIntr<keys>::AddKey(uint8_t keynumber,Btn_t* pBtn)
        {
            if(pBtn && (keynumber < Total_keys))
            {
                pBtn->HwInit();
                _Keyes[keynumber] = pBtn;        
            }    
        }
    
    template<uint32_t keys>
        void KeypadIntr<keys>::Run()
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
