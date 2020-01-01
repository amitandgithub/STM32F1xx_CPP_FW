#include "diskio.h"
#include "stm32f1xx_hal.h"

#define CPP_CLASS

#define CAP_VER2_00	(1<<0)
#define CAP_SDHC	(1<<1)

struct hwif {
	int initialized;
	int sectors;
	int erase_sectors;
	int capabilities;
};
typedef struct hwif hwif;



enum sd_speed 
{ 
    SD_SPEED_INVALID = 0xff,
    SD_SPEED_400KHZ = SPI_BAUDRATEPRESCALER_256,
    SD_SPEED_25MHZ  = SPI_BAUDRATEPRESCALER_4
};


void spi_set_speed(unsigned int speed);
uint8_t spi_txrx(uint8_t data);
void spi_init(void);
void spi_cs_low();
void spi_cs_high();


int hwif_init(hwif* hw);
int sd_read(hwif* hw, uint32_t address, uint8_t *buf);
int sd_write(hwif* hw, uint32_t address,const uint8_t *buf);


#define sd_get_r3 sd_get_r7

static const char *r1_strings[7] = {
	"in idle",
	"erase reset",
	"illegal command",
	"communication crc error",
	"erase sequence error",
	"address error",
	"parameter error"
};
static const char *r2_strings[15] = {
	"card is locked",
	"wp erase skip | lock/unlock cmd failed",
	"error",
	"CC error",
	"card ecc failed",
	"wp violation",
	"erase param",
	"out of range | csd overwrite",
	"in idle state",
	"erase reset",
	"illegal command",
	"com crc error",
	"erase sequence error",
	"address error",
	"parameter error",
};
