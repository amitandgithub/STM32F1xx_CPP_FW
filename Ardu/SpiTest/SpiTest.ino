/*
 SCP1000 Barometric Pressure Sensor Display

 Shows the output of a Barometric Pressure Sensor on a
 Uses the SPI library. For details on the sensor, see:
 http://www.sparkfun.com/commerce/product_info.php?products_id=8161
 http://www.vti.fi/en/support/obsolete_products/pressure_sensors/

 This sketch adapted from Nathan Seidle's SCP1000 example for PIC:
 http://www.sparkfun.com/datasheets/Sensors/SCP1000-Testing.zip

 Circuit:
 SCP1000 sensor attached to pins 6, 7, 10 - 13:
 DRDY: pin 6
 CSB: pin 7
 MOSI: pin 11
 MISO: pin 12
 SCK: pin 13

 created 31 July 2010
 modified 14 August 2010
 by Tom Igoe
 */

// the sensor communicates using SPI, so include the library:
#include <SPI.h>

// DS3234 SPI Settings:
#define DS3234_MAX_SCLK 4000000 // 4MHz max SPI clock rate
SPISettings DS3234SPISettings(DS3234_MAX_SCLK, MSBFIRST, SPI_MODE0);

const int chipSelectPin = 7;

void setup() {
  Serial.begin(9600);

  // start the SPI library:
  SPI.begin();
  
  pinMode(chipSelectPin, OUTPUT);
  delay(100);
  spiWriteByte('A');
}
uint8_t Data = 1;
uint8_t RxBuf[260];
void loop() 
{
    
//  SPI.beginTransaction(DS3234SPISettings);
//  digitalWrite(chipSelectPin, LOW);
//  for (int i=0; i<255; i++)
//  {
//    SPI.transfer(Data);
//    Serial.println(Data,DEC);
//  }
//  digitalWrite(chipSelectPin, HIGH);
//  SPI.endTransaction();
//  Data++;
//  
//    //Serial.println("\tPressure [Pa]=");
//    //Serial.println(Data,DEC);
//
//    delay(1000);

  SPI.beginTransaction(DS3234SPISettings);
  digitalWrite(chipSelectPin, LOW);
  delay(300);
  Serial.println(SPI.transfer(Data++),DEC);
  delay(300);
  digitalWrite(chipSelectPin, HIGH);
  SPI.endTransaction();
//while(1);
  delay(300);
}

// spiWriteBytes -- write a set number of bytes to an SPI device, incrementing from a register
void spiWriteBytes(uint8_t * values, uint8_t len)
{
  SPI.beginTransaction(DS3234SPISettings);
  digitalWrite(chipSelectPin, LOW);
  for (int i=0; i<len; i++)
  {
    SPI.transfer(values[i]);
  }
  digitalWrite(chipSelectPin, HIGH);
  SPI.endTransaction();
}

void spiWriteByte(uint8_t value)
{
  SPI.beginTransaction(DS3234SPISettings);
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(value);
  digitalWrite(chipSelectPin, HIGH);
  SPI.endTransaction();
}

// spiWriteBytes -- read a byte from an spi device's register
uint8_t spiReadByte()
{
  uint8_t retVal = 0;
  SPI.beginTransaction(DS3234SPISettings);
  digitalWrite(chipSelectPin, LOW);
  retVal = SPI.transfer(0x00);
  digitalWrite(chipSelectPin, HIGH);
  SPI.endTransaction();
  
  return retVal;
}

// spiWriteBytes -- read a set number of bytes from an spi device, incrementing from a register
void spiReadBytes(uint8_t * dest, uint8_t len)
{
  SPI.beginTransaction(DS3234SPISettings);
  digitalWrite(chipSelectPin, LOW);
  for (int i=0; i<len; i++)
  {
    dest[i] = SPI.transfer(0x00);
  }
  digitalWrite(chipSelectPin, HIGH);
  SPI.endTransaction();
}
