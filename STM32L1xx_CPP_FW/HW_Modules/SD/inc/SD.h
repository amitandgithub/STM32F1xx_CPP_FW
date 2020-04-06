/******************
** FILE: SD.h
**
** DESCRIPTION:
**  SD Card class 
**
** CREATED: 10/31/2018, by Amit Chaudhary
******************/
#ifndef SD_h
#define SD_h

#include<cstdint>

#include "ff.h"
#include "diskio.h"

#include "Spi.h"
#include "GpioOutput.h"

namespace BSP
{
#define SPI_HAL 0
#define SPI_BM 1
    
class SD
{
public:   
   
    typedef HAL::Spi SPIDrv_t;
    
    typedef uint32_t Status_t;
    
    SD(SPIDrv_t*   pSpiDriverSD , Port_t CSPort, PIN_t CSPin);
    
    ~SD(){};
    
    void HwInit();
    
    void spi_init() {HwInit();}
    
    uint8_t spi_txrx(uint8_t data);
    
    void spi_cs_high(){m_pSD_CS.High();}
    
    void spi_cs_low(){m_pSD_CS.Low();}
    
    void spi_set_speed(uint32_t speed){};
    
    FRESULT  open(FIL* fp, const TCHAR* path, BYTE mode){ return f_open(fp,path,mode); }
    
    FRESULT mount(FATFS* fs,const TCHAR* path,BYTE opt){ return f_mount(fs,path,opt); }
    
    FRESULT  close(FIL* fp){ return f_close(fp); }
    
    FRESULT  read(FIL* fp, void* buff, UINT btr, UINT* br){ return f_read(fp,buff,btr,br); }
    
    FRESULT  write(FIL* fp, const void* buff, UINT btw, UINT* bw){ return f_write(fp,buff,btw,bw); }
    
    FRESULT  lseek(FIL* fp, DWORD ofs){ return f_lseek(fp,ofs); }
    
    int  print(FIL* fp, const TCHAR* str, ...){ return f_printf(fp,str); }  

private:
        
    SPIDrv_t*   m_pSpiDriverSD;
    
    HAL::GpioOutput     m_pSD_CS;
    
};


}








#endif //SD_h
