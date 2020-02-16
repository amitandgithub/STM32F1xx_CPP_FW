

#include "KeypadPoll.h"

class Key0RiseCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t Key0Rise;
        Key0Rise++;
    }
};

class Key0FallCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t Key0Fall;
        Key0Fall++;
    }
};


class Key1RiseCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t Key1Rise;
        Key1Rise++;
    }
};

class Key1FallCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t Key1Fall;
        Key1Fall++;
    }
};

class Key2RiseCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t Key2Rise;
        Key2Rise++;
    }
};

class Key2FallCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t Key2Fall;
        Key2Fall++;
    }
};


class Key3RiseCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t Key3Rise;
        Key3Rise++;
    }
};

class Key3FallCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t Key3Fall;
        Key3Fall++;
    }
};



Key0RiseCallback Key0RiseCB;
Key0FallCallback Key0FallCB;

Key1RiseCallback Key1RiseCB;
Key1FallCallback Key1FallCB;

Key2RiseCallback Key2RiseCB;
Key2FallCallback Key2FallCB;

Key3RiseCallback Key3RiseCB;
Key3FallCallback Key3FallCB;


BtnPoll Key0(Gpio::A5, BSP::BtnPoll::BTN_PULL_DOWN,3, &Key0RiseCB,&Key0FallCB );
BtnPoll Key1(Gpio::A7, BSP::BtnPoll::BTN_PULL_DOWN,3, &Key1RiseCB,&Key1FallCB );
BtnPoll Key2(Gpio::A1, BSP::BtnPoll::BTN_PULL_DOWN,3, &Key2RiseCB,&Key2FallCB );
BtnPoll Key3(Gpio::A8, BSP::BtnPoll::BTN_PULL_DOWN,3, &Key3RiseCB,&Key3FallCB );

void KeypadPoll_Test()
{
    KeypadPoll<4> MyKeypad;
    MyKeypad.AddKey(0,&Key0);
    MyKeypad.AddKey(1,&Key1);
    MyKeypad.AddKey(2,&Key2);
    MyKeypad.AddKey(3,&Key3);

    while(1)
    {
        MyKeypad.Run();
    }
}