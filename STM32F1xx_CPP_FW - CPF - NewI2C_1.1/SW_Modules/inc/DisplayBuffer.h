/******************
** CLASS: DisplayBuffer 
**
** DESCRIPTION:
**  Display Buffer class
**
** CREATED: 10/15/2018, by Amit Chaudhary
**
** FILE: DisplayBuffer.h
**
******************/
#ifndef DisplayBuffer_h
#define DisplayBuffer_h

#include"stdint.h"
#include<cstring>

namespace Peripherals
{
    
#define SPACE_BEFORE   
    
class DisplayBuffer
{
public:
    typedef enum : uint8_t
    {
        SPEED_SUPER_SLOW = 0,
        SPEED_SLOW,
        SPEED_NORMAL,
        SPEED_FAST,
        SPEED_FASTER,
        SPEED_FASTEST        
    }Speed_t;
    
    typedef enum : uint8_t
    {
        UP = 0,
        DOWN,
        LEFT,
        RIGHT       
    }Dir_t;
    
    static const        uint8_t SIZE_OF_1_CHAR      = 6;
    static const        uint8_t NO_OF_CHAR_IN_LINE  = 14;
    static const        uint8_t NO_OF_LINES_IN_SCREEN          = 6;
    
    static constexpr    uint32_t BUFFER_SIZE        = (SIZE_OF_1_CHAR*NO_OF_CHAR_IN_LINE*NO_OF_LINES_IN_SCREEN);
    
    typedef struct 
    { 
        char Chars[NO_OF_CHAR_IN_LINE][SIZE_OF_1_CHAR];        
    }LineText_t;
    
    typedef struct 
    { 
        LineText_t Line[NO_OF_LINES_IN_SCREEN];        
    }DisplayBufferText_t;
    
    typedef union
    {
        char Array[BUFFER_SIZE];
        DisplayBufferText_t Struct;
        
    }DisplayBufferMem_t;
        
    static const uint8_t NORMAL  = 0x0C;
	static const uint8_t INVERSE = 0x0D;
    
    DisplayBuffer   (){Clear();}
    DisplayBuffer   (char* pText){DrawBuffer(pText);}
    
    ~DisplayBuffer  (){};
    
    void        DrawChar        (unsigned char Row, unsigned char Col, const char aChar, uint8_t format = NORMAL);
    //void        DrawStr         (unsigned char Row, unsigned char Col, const char* Str, uint8_t format = NORMAL);
    void        DrawStr         (unsigned char Row, unsigned char Col, const char* Str, uint8_t Len = NO_OF_CHAR_IN_LINE, uint8_t format = NORMAL);
    void        DrawBuffer      (const char* Str, uint8_t format = NORMAL);
    void        DrawBitmap      (const char* Str);
    void        InvertRow       (unsigned char Row);
    void        ScroolText      (unsigned char Row, Speed_t Speed, Dir_t Direction);
    char*       GetBuffer       ()              { return DisplayBufferMem.Array;}
    void        Clear           ()              { memset(&DisplayBufferMem.Array[0],0,BUFFER_SIZE);};
    uint32_t    GetSize         ()              { return BUFFER_SIZE; }
    

private:
    DisplayBufferMem_t DisplayBufferMem;    
};


}

#endif //DisplayBuffer_h
