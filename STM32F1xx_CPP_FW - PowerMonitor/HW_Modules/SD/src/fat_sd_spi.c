

#include "fat_sd_spi.h"
#include "RTC.h"

extern HAL::Spi spi1;
extern HAL::Spi spi2;
extern HAL::Rtc rtc;

#define SD_SPI spi2

#define PRINTF_SD 1

#define FAT_SD_CS_PORT GPIOB
#define FAT_SD_CS_PIN  GPIO_PIN_12

HAL::GpioOutput     SD_CS(B12);

void spi_cs_low() {SD_CS.Low();}

void spi_cs_high() {SD_CS.High();}

void spi_init(void)
{
  SD_SPI.HwInit();
  SD_CS.HwInit();
}


void spi_set_speed(uint32_t speed)
{
  SD_SPI.SetBaudrate( speed == SD_SPEED_400KHZ ? HAL::Spi::SPI_BAUDRATE_DIV256 : HAL::Spi::SPI_BAUDRATE_DIV2 );
}

inline uint8_t spi_txrx(uint8_t data)
{
    return  SD_SPI.TxRxPoll8Bit(data);
}



#if 0
DWORD get_fattime()
{  
  rtc.Get(&DateAndTime);
  
  return (DateAndTime.Year-1980)<<25 | DateAndTime.Month<<21 | DateAndTime.Date<<16 |
    (DateAndTime.Hours)<<11 | (DateAndTime.Minutes)<<5 | DateAndTime.Seconds;
}
#else
DWORD get_fattime()
{
  HAL::Rtc::RtcDate_t date;
  HAL::Rtc::RtcTime_t time;
  rtc.GetTime(&time);
  rtc.GetDate(&date);
//  int time = rtc.RTC_ReadTimeCounter();
//   int y = 2020, m = 5, d = 18;
//  time %= 86400;
  return ((date.Year+2000)-1980)<<25 | date.Month<<21 | date.Day<<16 |
  //return (y-1980)<<25 | m<<21 | d<<16 |
    time.Hours<<11 | time.Minutes<<5 | time.Seconds;
}
#endif
/* crc helpers */
static uint8_t crc7_one(uint8_t t, uint8_t data)
{
	int i;
	const uint8_t g = 0x89;
	t ^= data;
	for (i=0; i<8; i++) {
		if (t & 0x80)
			t ^= g;
		t <<= 1;
	}
	return t;
}
uint8_t crc7(const uint8_t *p, int len)
{
	int j;
	uint8_t crc = 0;
	for (j=0; j<len; j++)
		crc = crc7_one(crc, p[j]);
	return crc>>1;
}
static uint16_t crc16_ccitt(uint16_t crc, uint8_t ser_data)
{
	crc  = (uint8_t)(crc >> 8) | (crc << 8);
	crc ^= ser_data;
	crc ^= (uint8_t)(crc & 0xff) >> 4;
	crc ^= (crc << 8) << 4;
	crc ^= ((crc & 0xff) << 4) << 1;
	return crc;
}
uint16_t crc16(const uint8_t *p, int len)
{
	int i;
	uint16_t crc = 0;
	for (i=0; i<len; i++)
		crc = crc16_ccitt(crc, p[i]);
	return crc;
}


/*** sd functions - on top of spi code ***/

static void sd_cmd(uint8_t cmd, uint32_t arg)
{
	uint8_t crc = 0;
	spi_txrx(0x40 | cmd);
	crc = crc7_one(crc, 0x40 | cmd);
	spi_txrx(arg >> 24);
	crc = crc7_one(crc, arg >> 24);
	spi_txrx(arg >> 16);
	crc = crc7_one(crc, arg >> 16);
	spi_txrx(arg >> 8);
	crc = crc7_one(crc, arg >> 8);
	spi_txrx(arg);
	crc = crc7_one(crc, arg);
	//spi_txrx(0x95);
	spi_txrx(crc | 0x1);	/* crc7, for cmd0 */
}

static uint8_t sd_get_r1()
{
	int tries = 1000;
	uint8_t r;

	while (tries--) {
		r = spi_txrx(0xff);
		if ((r & 0x80) == 0)
			return r;
	}
	return 0xff;
}

static uint16_t sd_get_r2()
{
	int tries = 1000;
	uint16_t r;
	while (tries--) {
		r = spi_txrx(0xff);
		if ((r & 0x80) == 0)
			break;
	}
	if (tries < 0)
		return 0xff;
	r = r<<8 | spi_txrx(0xff);
	return r;
}

/*
 * r1, then 32-bit reply... same format as r3
 */
static uint8_t sd_get_r7(uint32_t *r7)
{
	uint32_t r;
	r = sd_get_r1();
	if (r != 0x01)
		return r;
	r = spi_txrx(0xff) << 24;
	r |= spi_txrx(0xff) << 16;
	r |= spi_txrx(0xff) << 8;
	r |= spi_txrx(0xff);
	*r7 = r;
	return 0x01;
}

#if PRINTF_SD
static void print_r1(uint8_t r)
{
   
	int i;
	printf("R1: %02x\n", r);
	for (i=0; i<7; i++)
		if (r & (1<<i))
			printf("  %s\n", r1_strings[i]);
}
static void print_r2(uint16_t r)
{
	int i;
	printf("R2: %04x\n", r);
	for (i=0; i<15; i++)
		if (r & (1<<i))
			printf("  %s\n", r2_strings[i]);
}
#endif // PRINTF_SD

/* Nec (=Ncr? which is limited to [0,8]) dummy bytes before lowering CS,
 * as described in sandisk doc, 5.4. */
static void sd_nec()
{
	int i;
	for (i=0; i<8; i++)
		spi_txrx(0xff);
}

static int sd_init(hwif *hw)
{
	int i;
	int r;
	uint32_t r7;
	uint32_t r3;
	int tries;
    uint32_t hcs = 0;

	hw->capabilities = 0;

	/* start with 100-400 kHz clock */
	spi_set_speed(SD_SPEED_400KHZ);
    
#if PRINTF_SD
	printf("cmd0 - reset.. ");
#endif // PRINTF_SD
	spi_cs_high();
	/* 74+ clocks with CS high */
	for (i=0; i<10; i++)
		spi_txrx(0xff);

	/* reset */
	spi_cs_low();
	sd_cmd(0, 0);
	r = sd_get_r1();
	sd_nec();
	spi_cs_high();
	if (r == 0xff)
		goto err_spi;
	if (r != 0x01) {
#if PRINTF_SD
		printf("fail\n");
		print_r1(r);
#endif // PRINTF_SD
		goto err;
	}
#if PRINTF_SD
	printf("success\n");

	printf("cmd8 - voltage.. ");
#endif // PRINTF_SD    
	/* ask about voltage supply */
	spi_cs_low();
	sd_cmd(8, 0x1aa /* VHS = 1 */);
	r = sd_get_r7(&r7);
	sd_nec();
	spi_cs_high();
	hw->capabilities |= CAP_VER2_00;
	if (r == 0xff)
		goto err_spi;
	if (r == 0x01)
    {
#if PRINTF_SD
		printf("success, SD v2.x\n");
#endif // PRINTF_SD		
    }
	else if (r & 0x4) {
		hw->capabilities &= ~CAP_VER2_00;
#if PRINTF_SD
		printf("not implemented, SD v1.x\n");
#endif // PRINTF_SD		
	} else {
#if PRINTF_SD
		printf("fail\n");
		print_r1(r);
#endif // PRINTF_SD		
		return -2;
	}

#if PRINTF_SD
	printf("cmd58 - ocr.. ");
#endif // PRINTF_SD	
	/* ask about voltage supply */
	spi_cs_low();
	sd_cmd(58, 0);
	r = sd_get_r3(&r3);
	sd_nec();
	spi_cs_high();
	if (r == 0xff)
		goto err_spi;
	if (r != 0x01 && !(r & 0x4)) { /* allow it to not be implemented - old cards */
#if PRINTF_SD
		printf("fail\n");
		print_r1(r);
#endif // PRINTF_SD			
		return -2;
	}
	else {
		int i;
		for (i=4; i<=23; i++)
			if (r3 & 1<<i)
				break;
#if PRINTF_SD
		printf("Vdd voltage window: %i.%i-", (12+i)/10, (12+i)%10);
#endif // PRINTF_SD			
		for (i=23; i>=4; i--)
			if (r3 & 1<<i)
				break;
#if PRINTF_SD
		/* CCS shouldn't be valid here yet */
		printf("%i.%iV, CCS:%li, power up status:%li\n",
				(13+i)/10, (13+i)%10,
				r3>>30 & 1, r3>>31);
		printf("success\n");
#endif // PRINTF_SD			
        
	}

#if PRINTF_SD
	printf("acmd41 - hcs.. ");
#endif // PRINTF_SD	    
	tries = 1000;
	
	/* say we support SDHC */
	if (hw->capabilities & CAP_VER2_00)
		hcs = 1<<30;

	/* needs to be polled until in_idle_state becomes 0 */
	do {
		/* send we don't support SDHC */
		spi_cs_low();
		/* next cmd is ACMD */
		sd_cmd(55, 0);
		r = sd_get_r1();
		sd_nec();
		spi_cs_high();
		if (r == 0xff)
			goto err_spi;
		/* well... it's probably not idle here, but specs aren't clear */
		if (r & 0xfe) {
#if PRINTF_SD
			printf("fail\n");
			print_r1(r);
#endif // PRINTF_SD				
			goto err;
		}

		spi_cs_low();
		sd_cmd(41, hcs);
		r = sd_get_r1();
		sd_nec();
		spi_cs_high();
		if (r == 0xff)
			goto err_spi;
		if (r & 0xfe) {
#if PRINTF_SD
			printf("fail\n");
			print_r1(r);
#endif // PRINTF_SD				
			goto err;
		}
	} while (r != 0 && tries--);
	if (tries == -1) {
#if PRINTF_SD
		printf("timeouted\n");
#endif // PRINTF_SD			
		goto err;
	}
    #if PRINTF_SD
	printf("success\n");
#endif // PRINTF_SD		

	/* Seems after this card is initialized which means bit 0 of R1
	 * will be cleared. Not too sure. */


	if (hw->capabilities & CAP_VER2_00) {
#if PRINTF_SD
		printf("cmd58 - ocr, 2nd time.. ");
#endif // PRINTF_SD			
		/* ask about voltage supply */
		spi_cs_low();
		sd_cmd(58, 0);
		r = sd_get_r3(&r3);
		sd_nec();
		spi_cs_high();
		if (r == 0xff)
			goto err_spi;
		if (r & 0xfe) {
#if PRINTF_SD            
			printf("fail\n");
			print_r1(r);
#endif // PRINTF_SD	            
			return -2;
		}
		else {
//#if 1
			int i;
			for (i=4; i<=23; i++)
				if (r3 & 1<<i)
					break;
#if PRINTF_SD           
			printf("Vdd voltage window: %i.%i-", (12+i)/10, (12+i)%10);
#endif // PRINTF_SD				
			for (i=23; i>=4; i--)
				if (r3 & 1<<i)
					break;
#if PRINTF_SD           
			/* CCS shouldn't be valid here yet */
			printf("%i.%iV, CCS:%li, power up status:%li\n",
					(13+i)/10, (13+i)%10,
					r3>>30 & 1, r3>>31);
			// XXX power up status should be 1 here, since we're finished initializing, but it's not. WHY?
			// that means CCS is invalid, so we'll set CAP_SDHC later
#endif // PRINTF_SD	
			if (r3>>30 & 1) {
				hw->capabilities |= CAP_SDHC;
			}
#if PRINTF_SD
			printf("success\n");
#endif // PRINTF_SD				
		}
	}


	/* with SDHC block length is fixed to 1024 */
	if ((hw->capabilities & CAP_SDHC) == 0) {
#if PRINTF_SD        
		printf("cmd16 - block length.. ");
#endif // PRINTF_SD			
		spi_cs_low();
		sd_cmd(16, 512);
		r = sd_get_r1();
		sd_nec();
		spi_cs_high();
		if (r == 0xff)
			goto err_spi;
		if (r & 0xfe) {
#if PRINTF_SD            
			printf("fail\n");
			print_r1(r);
#endif // PRINTF_SD				
			goto err;
		}
#if PRINTF_SD  
		printf("success\n");
#endif // PRINTF_SD	        
	}

#if PRINTF_SD
	printf("cmd59 - enable crc.. ");
#endif // PRINTF_SD		
	/* crc on */
	spi_cs_low();
	sd_cmd(59, 0);
	r = sd_get_r1();
	sd_nec();
	spi_cs_high();
	if (r == 0xff)
		goto err_spi;
	if (r & 0xfe) {
#if PRINTF_SD        
		printf("fail\n");
		print_r1(r);
#endif // PRINTF_SD			
		goto err;
	}
#if PRINTF_SD   
	printf("success\n");
#endif // PRINTF_SD	

	/* now we can up the clock to <= 25 MHz */
	spi_set_speed(SD_SPEED_25MHZ);

	return 0;

 err_spi:
#if PRINTF_SD     
	printf("fail spi\n");
#endif // PRINTF_SD		
	return -1;
 err:
	return -2;
}

static int sd_read_status(hwif *hw)
{
	uint16_t r2;

	spi_cs_low();
	sd_cmd(13, 0);
	r2 = sd_get_r2();
	sd_nec();
	spi_cs_high();
	if (r2 & 0x8000)
		return -1;
#if PRINTF_SD  
	if (r2)
		print_r2(r2);
#endif // PRINTF_SD	

	return 0;
}

/* 0xfe marks data start, then len bytes of data and crc16 */
static int sd_get_data(hwif *hw, uint8_t *buf, int len)
{
	int tries = 20000;
	uint8_t r;
	uint16_t _crc16;
	uint16_t calc_crc;
	int i;

	while (tries--) {
		r = spi_txrx(0xff);
		if (r == 0xfe)
			break;
	}
	if (tries < 0)
		return -1;

	for (i=0; i<len; i++)
		buf[i] = spi_txrx(0xff);

	_crc16 = spi_txrx(0xff) << 8;
	_crc16 |= spi_txrx(0xff);

	calc_crc = crc16(buf, len);
	if (_crc16 != calc_crc) {
#if PRINTF_SD        
		printf("%s, crcs differ: %04x vs. %04x, len:%i\n", __func__, _crc16, calc_crc, len);
#endif // PRINTF_SD			
		return -1;
	}

	return 0;
}

static int sd_put_data(hwif *hw, const uint8_t *buf, int len)
{
	uint8_t r;
	int tries = 10;
	//uint8_t b[16];
	//int bi = 0;
	uint16_t crc;

	spi_txrx(0xfe); /* data start */

	while (len--)
		spi_txrx(*buf++);

	crc = crc16(buf, len);
	/* crc16 */
	spi_txrx(crc>>8);
	spi_txrx(crc);

	/* normally just one dummy read in between... specs don't say how many */
	while (tries--) {
		//b[bi++] = r = spi_txrx(0xff);
        r = spi_txrx(0xff);
		if (r != 0xff)
			break;
	}
	if (tries < 0)
		return -1;

	/* poll busy, about 300 reads for 256 MB card */
	tries = 100000;
	while (tries--) {
		if (spi_txrx(0xff) == 0xff)
			break;
	}
	if (tries < 0)
		return -2;

	/* data accepted, WIN */
	if ((r & 0x1f) == 0x05)
		return 0;

	return r;
}

static int sd_read_csd(hwif *hw)
{
	uint8_t buf[16];
	int r;
	int capacity;

	spi_cs_low();
	sd_cmd(9, 0);
	r = sd_get_r1();
	if (r == 0xff) {
		spi_cs_high();
		return -1;
	}
	if (r & 0xfe) {
		spi_cs_high();
#if PRINTF_SD        
		printf("%s ", __func__);
		print_r1(r);
#endif // PRINTF_SD			
		return -2;
	}

	r = sd_get_data(hw, buf, 16);
	sd_nec();
	spi_cs_high();
	if (r == -1) {
#if PRINTF_SD        
		printf("failed to get csd\n");
#endif // PRINTF_SD			
		return -3;
	}

	if ((buf[0] >> 6) + 1 == 1) {
#if PRINTF_SD
	/* CSD v1 */
	printf("CSD: CSD v%i taac:%02x, nsac:%i, tran:%02x, classes:%02x%x, read_bl_len:%i, "
		"read_bl_part:%i, write_blk_misalign:%i, read_blk_misalign:%i, dsr_imp:%i, "
		"c_size:%i, vdd_rmin:%i, vdd_rmax:%i, vdd_wmin:%i, vdd_wmax:%i, "
		"c_size_mult:%i, erase_blk_en:%i, erase_s_size:%i, "
		"wp_grp_size:%i, wp_grp_enable:%i, r2w_factor:%i, write_bl_len:%i, write_bl_part:%i, "
		"filef_gpr:%i, copy:%i, perm_wr_prot:%i, tmp_wr_prot:%i, filef:%i\n",
			(buf[0] >> 6) + 1,
			buf[1], buf[2], buf[3],
			buf[4], buf[5] >> 4, 1<<(buf[5] & 0xf), /* classes, read_bl_len */
			buf[6]>>7, (buf[6]>>6)&1, (buf[6]>>5)&1, (buf[6]>>4)&1,
			(buf[6]&0x3)<<10 | buf[7]<<2 | buf[8]>>6, /* c_size */
			(buf[8]&0x38)>>3, buf[8]&0x07, buf[9]>>5, (buf[9]>>2)&0x7,
			1<<(2+(((buf[9]&3) << 1) | buf[10]>>7)), /* c_size_mult */
			(buf[10]>>6)&1,
			((buf[10]&0x3f)<<1 | buf[11]>>7) + 1, /* erase sector size */
			(buf[11]&0x7f) + 1, /* write protect group size */
			buf[12]>>7, 1<<((buf[12]>>2)&7),
			1<<((buf[12]&3)<<2 | buf[13]>>6), /* write_bl_len */
			(buf[13]>>5)&1,
			buf[14]>>7, (buf[14]>>6)&1, (buf[14]>>5)&1, (buf[14]>>4)&1,
			(buf[14]>>2)&3 /* file format */);
#endif // PRINTF_SD	

	capacity = (((buf[6]&0x3)<<10 | buf[7]<<2 | buf[8]>>6)+1) << (2+(((buf[9]&3) << 1) | buf[10]>>7)) << ((buf[5] & 0xf) - 9);
	/* ^ = (c_size+1) * 2**(c_size_mult+2) * 2**(read_bl_len-9) */

	} else {
	/* CSD v2 */
		/* this means the card is HC */
		hw->capabilities |= CAP_SDHC;
#if PRINTF_SD
	printf("CSD: CSD v%i taac:%02x, nsac:%i, tran:%02x, classes:%02x%x, read_bl_len:%i, "
		"read_bl_part:%i, write_blk_misalign:%i, read_blk_misalign:%i, dsr_imp:%i, "
		"c_size:%i, erase_blk_en:%i, erase_s_size:%i, "
		"wp_grp_size:%i, wp_grp_enable:%i, r2w_factor:%i, write_bl_len:%i, write_bl_part:%i, "
		"filef_gpr:%i, copy:%i, perm_wr_prot:%i, tmp_wr_prot:%i, filef:%i\n",
			(buf[0] >> 6) + 1,
			buf[1], buf[2], buf[3],
			buf[4], buf[5] >> 4, 1<<(buf[5] & 0xf), /* classes, read_bl_len */
			buf[6]>>7, (buf[6]>>6)&1, (buf[6]>>5)&1, (buf[6]>>4)&1,
			buf[7]<<16 | buf[8]<<8 | buf[9], /* c_size */
			(buf[10]>>6)&1,
			((buf[10]&0x3f)<<1 | buf[11]>>7) + 1, /* erase sector size */
			(buf[11]&0x7f) + 1, /* write protect group size */
			buf[12]>>7, 1<<((buf[12]>>2)&7),
			1<<((buf[12]&3)<<2 | buf[13]>>6), /* write_bl_len */
			(buf[13]>>5)&1,
			buf[14]>>7, (buf[14]>>6)&1, (buf[14]>>5)&1, (buf[14]>>4)&1,
			(buf[14]>>2)&3 /* file format */);
#endif // PRINTF_SD	
	capacity = buf[7]<<16 | buf[8]<<8 | buf[9]; /* in 512 kB */
	capacity *= 1024; /* in 512 B sectors */

	}
#if PRINTF_SD
	printf("capacity = %i kB\n", capacity/2);
#endif // PRINTF_SD		
	hw->sectors = capacity;

	/* if erase_blk_en = 0, then only this many sectors can be erased at once
	 * this is NOT yet tested */
	hw->erase_sectors = 1;
	if (((buf[10]>>6)&1) == 0)
		hw->erase_sectors = ((buf[10]&0x3f)<<1 | buf[11]>>7) + 1;

	return 0;
}

static int sd_read_cid(hwif *hw)
{
	uint8_t buf[16];
	int r;

	spi_cs_low();
	sd_cmd(10, 0);
	r = sd_get_r1();
	if (r == 0xff) {
		spi_cs_high();
		return -1;
	}
	if (r & 0xfe) {
		spi_cs_high();
#if PRINTF_SD        
		printf("%s ", __func__);
		print_r1(r);
#endif // PRINTF_SD			
		return -2;
	}

	r = sd_get_data(hw, buf, 16);
	sd_nec();
	spi_cs_high();
	if (r == -1) {
#if PRINTF_SD        
		printf("failed to get cid\n");
#endif // PRINTF_SD			
		return -3;
	}
#if PRINTF_SD
	printf("CID: mid:%x, oid:%c%c, pnm:%c%c%c%c%c, prv:%i.%i, psn:%02x%02x%02x%02x, mdt:%i/%i\n",
			buf[0], buf[1], buf[2],			/* mid, oid */
			buf[3], buf[4], buf[5], buf[6], buf[7],	/* pnm */
			buf[8] >> 4, buf[8] & 0xf,		/* prv */
			buf[9], buf[10], buf[11], buf[12],	/* psn */
			2000 + (buf[13]<<4 | buf[14]>>4), 1 + (buf[14] & 0xf));
#endif // PRINTF_SD				

	return 0;
}


static int sd_readsector(hwif *hw, uint32_t address, uint8_t *buf)
{
	int r;

	spi_cs_low();
	if (hw->capabilities & CAP_SDHC)
		sd_cmd(17, address); /* read single block */
	else
		sd_cmd(17, address*512); /* read single block */

	r = sd_get_r1();
	if (r == 0xff) {
		spi_cs_high();
		r = -1;
		goto fail;
	}
	if (r & 0xfe) {
		spi_cs_high();
#if PRINTF_SD        
		printf("%s\n", __func__);
		print_r1(r);
#endif // PRINTF_SD			
		r = -2;
		goto fail;
	}

	r = sd_get_data(hw, buf, 512);
	sd_nec();
	spi_cs_high();
	if (r == -1) {
		r = -3;
		goto fail;
	}

	return 0;
 fail:
#if PRINTF_SD     
	printf("failed to read sector %li, err:%i\n", address, r);
#endif // PRINTF_SD		
	return r;
}

static int sd_writesector(hwif *hw, uint32_t address, const uint8_t *buf)
{
	int r;

	spi_cs_low();
	if (hw->capabilities & CAP_SDHC)
		sd_cmd(24, address); /* write block */
	else
		sd_cmd(24, address*512); /* write block */

	r = sd_get_r1();
	if (r == 0xff) {
		spi_cs_high();
		r = -1;
		goto fail;
	}
	if (r & 0xfe) {
		spi_cs_high();
#if PRINTF_SD        
		printf("%s\n", __func__);
		print_r1(r);
#endif // PRINTF_SD			
		r = -2;
		goto fail;
	}

	spi_txrx(0xff); /* Nwr (>= 1) high bytes */
	r = sd_put_data(hw, buf, 512);
	sd_nec();
	spi_cs_high();
	if (r != 0) {
#if PRINTF_SD        
		printf("sd_put_data returned: %i\n", r);
#endif // PRINTF_SD			
		r = -3;
		goto fail;
	}

	/* efsl code is weird shit, 0 is error in there?
	 * not that it's properly handled or anything,
	 * and the return type is char, fucking efsl */
	return 0;
 fail:
#if PRINTF_SD     
	printf("failed to write sector %li, err:%i\n", address, r);
#endif // PRINTF_SD		
	return r;
}


/*** public API - on top of sd/spi code ***/

int hwif_init(hwif* hw)
{
	int tries = 10;

	if (hw->initialized)
		return 0;

	spi_init();

	while (tries--) {
		if (sd_init(hw) == 0)
			break;
	}
	if (tries == -1)
		return -1;

	/* read status register */
	sd_read_status(hw);

	sd_read_cid(hw);
	if (sd_read_csd(hw) != 0)
		return -1;

	hw->initialized = 1;
	return 0;
}

int sd_read(hwif* hw, uint32_t address, uint8_t *buf)
{
	int r;
	int tries = 10;

	r = sd_readsector(hw, address, buf);

	while (r < 0 && tries--) {
		if (sd_init(hw) != 0)
			continue;

		/* read status register */
		sd_read_status(hw);

		r = sd_readsector(hw, address, buf);
	}
	if (tries == -1)
    {
#if PRINTF_SD        
		printf("%s: couldn't read sector %li\n", __func__, address);
#endif // PRINTF_SD	
    }		

	return r;
}

int sd_write(hwif* hw, uint32_t address,const uint8_t *buf)
{
	int r;
	int tries = 10;

	r = sd_writesector(hw, address, buf);

	while (r < 0 && tries--) {
		if (sd_init(hw) != 0)
			continue;

		/* read status register */
		sd_read_status(hw);

		r = sd_writesector(hw, address, buf);
	}
	if (tries == -1)
    {
#if PRINTF_SD        
		printf("%s: couldn't write sector %li\n", __func__, address);
#endif // PRINTF_SD	
    }		

	return r;
}