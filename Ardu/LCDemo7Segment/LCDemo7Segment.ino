//We always have to include the library
#include "LedControl.h"
#include <SimpleDHT.h>

/*
  Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
  pin 12 is connected to the DataIn
  pin 11 is connected to the CLK
  pin 10 is connected to LOAD
  We have only a single MAX72XX.
*/


#include <Arduino.h>
#include <Wire.h>

#include <BMx280I2C.h>

#define MY_DEBUG 1
#define I2C_ADDRESS 0x76

int battVolts,batt;   // made global for wider avaliblity throughout a sketch if needed, example a low voltage alarm, etc

//create a BMx280I2C object using the I2C interface with I2C Address 0x76
BMx280I2C bmx280(I2C_ADDRESS);


int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);

LedControl lc = LedControl(12, 11, 10, 1);

/* we always wait a bit between updates of the display */
unsigned long delaytime = 250;

void setup()
{
  Wire.begin();

  Serial.begin(115200);
  Serial.print("Start OK: ");

  //begin() checks the Interface, reads the sensor ID (to differentiate between BMP280 and BME280)
  //and reads compensation parameters.
  if (!bmx280.begin())
  {
    Serial.println("begin() failed. check your BMx280 Interface and I2C Address.");
    while (1);
  }

  if (bmx280.isBME280())
    Serial.println("sensor is a BME280");
  else
    Serial.println("sensor is a BMP280");

  //reset sensor to default parameters.
  bmx280.resetToDefaults();

  //by default sensing is disabled and must be enabled by setting a non-zero
  //oversampling setting.
  //set an oversampling setting for pressure and temperature measurements.
  bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
  bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);

  //if sensor is a BME280, set an oversampling setting for humidity measurements.
  if (bmx280.isBME280())
    bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);

  /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
  */

  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);

  //lc.setScanLimit();
  //
  //  lc.setChar(0,7,'1',false);
  //
  //  lc.setChar(0,6,'2',false);
  //
  //  lc.setChar(0,5,'3',false);
  //
  //  lc.setChar(0,4,'4',false);
  //
  //  lc.setChar(0, 3, '5', false);
  //
  //  lc.setChar(0, 2, '6', false);
  //
  //  lc.setChar(0, 1, '7', false);
  //
  //  lc.setChar(0, 0, '8', false);
}


/*
  This method will display the characters for the
  word "Arduino" one after the other on digit 0.
*/
void writeArduinoOn7Segment() {
  lc.setChar(0, 0, 'a', false);
  delay(delaytime);
  lc.setRow(0, 0, 0x05);
  delay(delaytime);
  lc.setChar(0, 0, 'd', false);
  delay(delaytime);
  lc.setRow(0, 0, 0x1c);
  delay(delaytime);
  lc.setRow(0, 0, B00010000);
  delay(delaytime);
  lc.setRow(0, 0, 0x15);
  delay(delaytime);
  lc.setRow(0, 0, 0x1D);
  delay(delaytime);
  lc.clearDisplay(0);
  delay(delaytime);
}

int getBandgap(void) // Returns actual value of Vcc (x 100)
{

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  // For mega boards
  const long InternalReferenceVoltage = 1115L;  // Adjust this value to your boards specific internal BG voltage x1000
  // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc reference
  // MUX4 MUX3 MUX2 MUX1 MUX0  --> 11110 1.1V (VBG)         -Selects channel 30, bandgap voltage, to measure
  ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (0 << MUX5) | (1 << MUX4) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0);

#else
  // For 168/328 boards
  const long InternalReferenceVoltage = 1056L;  // Adjust this value to your boards specific internal BG voltage x1000
  // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc external reference
  // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)         -Selects channel 14, bandgap voltage, to measure
  ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0);

#endif
  delay(50);  // Let mux settle a little to get a more stable A/D conversion
  // Start a conversion
  ADCSRA |= _BV( ADSC );
  // Wait for it to complete
  while ( ( (ADCSRA & (1 << ADSC)) != 0 ) );
  // Scale the value
  int results = (((InternalReferenceVoltage * 1024L) / ADC) + 5L) / 10L; // calculates for straight line value
  return results;

}

void loop()
{
  static int LoopCounter;
  static byte counter;
  float temp, temp1;
  uint32_t pressure;
  int temp2;
  //start a measurement
  if (!bmx280.measure())
  {
    Serial.println("could not start measurement, is a measurement already running?");
    return;
  }

  //wait for the measurement to finish
  do
  {
    delay(100);
  } while (!bmx280.hasValue());

  if (bmx280.isBME280())
  {
    Serial.print("Humidity: ");
    Serial.println(bmx280.getHumidity());
  }

  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(1000);
    return;
  }
#if MY_DEBUG
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, ");
  Serial.print((int)humidity); Serial.print(" H");
#endif
  temp = bmx280.getTemperature();
  pressure = bmx280.getPressure();
  temp1 = temp * 100;
  temp2 = temp1;

  // Temperature
  lc.setChar(0, 7, ((byte)temp / 10) + '0', false);
  lc.setChar(0, 6, ((byte)temp % 10) + '0', false);

  //  lc.setChar(0, 5, (temp2%100)/10 +'0', false);
  //  lc.setChar(0, 4, (temp2%10)+'0', false);

  // Humidity
  lc.setChar(0, 5, (humidity / 10) + '0', false);
  lc.setChar(0, 4, (humidity % 10) + '0', false);


#if MY_DEBUG
  Serial.print("\t Pressure: "); Serial.print(bmx280.getPressure());
  Serial.print("\t Temperature: "); Serial.println(bmx280.getTemperature());
#endif


if(LoopCounter%6 == 0)
{
  battVolts = getBandgap();  //Determins what actual Vcc is, (X 100), based on known bandgap voltage
  batt = map(analogRead(3), 0, 1023, 0, battVolts);
  // Battery voltage
  lc.setChar(0, 3, 'b', false);
  lc.setChar(0, 2, (batt % 1000) / 100 + '0', false);
  lc.setChar(0, 1, (batt % 100) / 10 + '0', false);
  lc.setChar(0, 0, (batt % 10) + '0', false);  
}
else
{
  // Pressure
  if((pressure % 10000) / 1000 )
  {
    lc.setChar(0, 3, (pressure % 10000) / 1000 + '0', false);
  }
  else
  {
    lc.setChar(0, 3, ' ', false);
  }
  
  lc.setChar(0, 2, (pressure % 1000) / 100 + '0', false);
  lc.setChar(0, 1, (pressure % 100) / 10 + '0', false);
  lc.setChar(0, 0, (pressure % 10) + '0', false);
}
  LoopCounter++;
  delay(1500);

}
