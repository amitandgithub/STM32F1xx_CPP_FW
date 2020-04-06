// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
int SlaveAddress = 0x08;
void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  //  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);
  Wire.setClock(400000);
}

char count;
char TxArray[20] = "  Amit Chaudhary";
#define XFER_SIZE sizeof("  Amit Chaudhary")

void loop()
{
  Wire.beginTransmission(SlaveAddress); // transmit to device #8
  Wire.write(TxArray, XFER_SIZE);
  Wire.endTransmission();    // stop transmitting

  //delay(1);

  Wire.requestFrom(SlaveAddress, XFER_SIZE);    // request 6 bytes from slave device #8
#if 1
  for (uint8_t i = 0; i < XFER_SIZE; i++)
  {
    TxArray[i] = Wire.read();
    Serial.print(TxArray[i]);
  }
  Serial.println(' ');
#endif

  //delay(1);

}

char c;
void receiveEvent(int howMany)
{
  while (1 < Wire.available())
  {
    c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}
