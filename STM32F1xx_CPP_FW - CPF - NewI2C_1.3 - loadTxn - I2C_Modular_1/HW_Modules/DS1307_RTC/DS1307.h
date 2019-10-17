/******************
** CLASS: DS1307
**
** DESCRIPTION:
**  DS1307 RTC functionality implementation
**
** CREATED: 7/1/2019, by Amit Chaudhary
**
** FILE: $FILE_FNAME$
**
******************/
#ifndef DS1307_H
#define DS1307_H

#define DS1307_ADDR_R	0xD1
#define DS1307_ADDR_W	0xD0
#define DS1307_ADDR		0x68

#define SQW_RATE_1		0
#define SQW_RATE_4K		1
#define SQW_RATE_8K		2
#define SQW_RATE_32K	3


class Time
{
public:
	uint8_t		hour;
	uint8_t		min;
	uint8_t		sec;
	uint8_t		date;
	uint8_t		mon;
	uint16_t	year;
	uint8_t		dow;

	Time(){year = 2010,mon  = 1,date = 1,hour = 0,min  = 0,sec  = 0,dow  = 0};
};

class DS1307
{
public:
    
  DS1307();
  
  ~DS1307(){};
  
  typedef enum : uint8_t {Mon=1, Tue, Wed, Thur, Fri, Sat, Sun } DOW_t;
  
  void	setTime(Time aTime);
  void	setTime(uint8_t hour, uint8_t min, uint8_t sec);
  void	setDate(uint8_t date, uint8_t mon, uint16_t year);
  void	setDOW(DOW_t DOW);
  uint8_t dec2bcd(uint8_t num);
  uint8_t bcd2dec(uint8_t num);  
  
  
};
#endif //DS1307_H
