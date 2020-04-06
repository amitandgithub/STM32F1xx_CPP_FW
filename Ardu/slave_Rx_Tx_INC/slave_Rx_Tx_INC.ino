// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
volatile char c, count;
char TxArray[50];
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

void receiveEvent(int howMany)
{
  while (0 < Wire.available())
  {
    c = Wire.read();
    Serial.print(c);
    TxArray[count++] = c+1;
  }

//  c = Wire.read();
//  TxArray[count++] = c+1;
//  
//  if (c == '\n')
//    Serial.print('\n');
//  else
//    Serial.println(c);
 Serial.print(" \t Received = ");
 Serial.println(count,DEC);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Wire.write(TxArray, count);
  Serial.print("Sent - ");
  for(uint8_t i = 0; i < count; i++)
  {
    Serial.print(TxArray[i]);  
  }
  Serial.print('\n');
  //Serial.print("Sent ");
  //Serial.println(count,DEC);
   count = 0;

}
