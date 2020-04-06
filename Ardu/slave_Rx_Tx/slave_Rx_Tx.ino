// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
char c, count;
uint8_t TxArray[10] = "Avni 0";
void setup() {
  Wire.begin(0x08);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  Serial.begin(9600);           // start serial for output
  Serial.println("Started");
}

void loop() {
  delay(100);
}

void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    c = Wire.read(); // receive byte as a character
        Serial.print(c);
  }
 // int x = Wire.read();    // receive byte as an integer
//  Serial.println(x);         // print the integer
c = Wire.read();
if(c == '\n')
Serial.print('\n');
else
Serial.println(c);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  TxArray[5] = count++;
  Wire.write(TxArray,6);
  // as expected by master
}
