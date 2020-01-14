/******************
** FILE: DS1307.cpp
**
** DESCRIPTION:
**  DS1307 RTC functionality Implementation
**
** CREATED: 7/1/2019, by Amit Chaudhary
******************/
#include "DS1307.h"

namespace HWModules
{
    
    
    DS1307::DS1307()
    {
        
    }
    
    void DS1307::setTime(Time aTime)
    {
        
    }
    
    void DS1307::setTime(uint8_t hour, uint8_t min, uint8_t sec)
    {
        
    }
    // Convert Decimal to Binary Coded Decimal (BCD)
    uint8_t DS1307::dec2bcd(uint8_t num)
    {
        return ((num/10 * 16) + (num % 10));
    }
    
    // Convert Binary Coded Decimal (BCD) to Decimal
    uint8_t DS1307::bcd2dec(uint8_t num)
    {
        return ((num/16 * 10) + (num % 16));
    }

    
}