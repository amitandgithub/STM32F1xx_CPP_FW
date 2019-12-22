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

#include "SPI_Poll.h"
#include "SPI_IT.h"
#include "SPI_DMA.h"
#include "SPI_Base.h"
#include "GpioOutput.hpp"

namespace Peripherals
{
#define SPI_HAL 0
#define SPI_BM 1
    
class SD
{
public:   
   
    typedef Peripherals::SPI_Poll SPIDrv_t;
    
    typedef uint32_t Status_t;
    
    SD(SPIDrv_t*   pSpiDriverSD , GpioOutput* pCS);
    
    ~SD(){};
    
    static void HwInit();
    
    static void spi_init() {HwInit();}
    
    static uint8_t spi_txrx(uint8_t data);
    
    static inline void spi_cs_high(){m_pSD_CS->On();}
    
    static void inline spi_cs_low(){m_pSD_CS->Off();}
    
    static void spi_set_speed(uint32_t speed){};
    
    FRESULT  open(FIL* fp, const TCHAR* path, BYTE mode){ return f_open(fp,path,mode); }
    
    FRESULT mount(FATFS* fs,const TCHAR* path,BYTE opt){ return f_mount(fs,path,opt); }
    
    FRESULT  close(FIL* fp){ return f_close(fp); }
    
    FRESULT  read(FIL* fp, void* buff, UINT btr, UINT* br){ return f_read(fp,buff,btr,br); }
    
    FRESULT  write(FIL* fp, const void* buff, UINT btw, UINT* bw){ return f_write(fp,buff,btw,bw); }
    
    FRESULT  lseek(FIL* fp, DWORD ofs){ return f_lseek(fp,ofs); }
    
    int  print(FIL* fp, const TCHAR* str, ...){ return f_printf(fp,str); }  

private:
        
    static SPIDrv_t*   m_pSpiDriverSD;
    
	static GpioOutput* m_pSD_CS;
    
    static SPI_Base::SPIx_t m_SPIx;
    
    static SPI_TypeDef* m_SPI_SD;
    
};


}








#endif //SD_h
