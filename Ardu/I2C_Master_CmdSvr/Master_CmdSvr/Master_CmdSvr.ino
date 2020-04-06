// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

#define DEBUG 1
int SlaveAddress = 0x08;
uint8_t requestCount = 1;

typedef enum
{
  MODULE_ID_I2C,


  MODULE_ID_MAX,
} ModuleID_t;


typedef enum
{
  CMD_STATUS_OK,
  CMD_STATUS_INVALID_PARAMS,
  CMD_STATUS_LEN_ERROR,
  CMD_STATUS_CRC_ERROR,
  CMD_STATUS_INVALID_MODULEID,
  CMD_STATUS_INVALID_CMDID,
  CMD_STATUS_ACK,
  CMD_STATUS_PROCESSING,
  CMD_STATUS_PROCESSING_DONE,
  CMD_STATUS_READY,
} CmdStatus_t;

typedef enum
{
  I2C_CMDID_ADD_REQUEST,
  I2C_CMDID_ADD_RESPONSE,
  I2C_CMDID_SUB_REQUEST,
  I2C_CMDID_SUB_RESPONSE,
  I2C_CMDID_MUL_REQUEST,
  I2C_CMDID_MUL_RESPONSE,
  I2C_CMDID_DIV_REQUEST,
  I2C_CMDID_DIV_RESPONSE,
} I2C_Commands_t;

#define CMD_SIZE 6
void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  // Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);
  Wire.setClock(400000);
  Serial.println("Start");
}

char count;
char TxIndex;
char RxIndex;
char TxBuf[15];
char RxBuf[15];
#define XFER_SIZE 1
static uint16_t PrintCounter;
CmdStatus_t CmdStatus;
uint8_t status_i2C;
void loop()
{
  Wire.requestFrom(SlaveAddress, 1);
  if (Wire.available() > 0)
  {
    RxBuf[0] = Wire.read();

    if (RxBuf[0] == CMD_STATUS_READY)
    {
#if DEBUG
     // Serial.println(' ');
     // PrintState(CMD_STATUS_READY);
#endif
      TxBuf[0] = MODULE_ID_I2C;
      TxBuf[1] = count;// I2C_CMDID_ADD_REQUEST;
      TxBuf[2] = 2;
      TxBuf[3] = 50;
      TxBuf[4] = 2;
      TxBuf[5] = I2C_CMDID_ADD_REQUEST;
      Wire.beginTransmission(SlaveAddress); // transmit to device #8
      Wire.write(TxBuf, CMD_SIZE);
      Wire.endTransmission();    // stop transmitting
      //delay(1000);

      while (1)
      {
        Wire.requestFrom(SlaveAddress, 1);
        if (Wire.available() > 0)
        {
          status_i2C = Wire.read();
          if (status_i2C == CMD_STATUS_PROCESSING_DONE)
          {
            Wire.requestFrom(SlaveAddress, CMD_SIZE);
#if DEBUG
            Serial.print("S-> ");
            for (uint8_t i = 0; i < CMD_SIZE; i++)
            {
              RxBuf[RxIndex] = Wire.read();
              Serial.print(RxBuf[RxIndex++], DEC);
              Serial.print(' ');
            }
            Serial.println(' ');
#endif
            break;
          }
         else if (status_i2C == CMD_STATUS_READY)
         {
          //Wire.requestFrom(SlaveAddress, 1);
          break;
         }
         else
         {
          Serial.print(status_i2C,DEC);
         }
      }
#if DEBUG
      Serial.print("*");
        if (PrintCounter++ == 30)
        {
          Serial.println("-");
          PrintCounter = 0;
        }
#endif
      }
      //Serial.println("Get Response");
      //Wire.requestFrom(SlaveAddress, 7);

      RxIndex = 0;
      count = count + 2;

      if (count >= 8)
        count = 0;
    }
    else
    {
      Serial.print(".");

      if (PrintCounter++ == 30)
      {
        Serial.println(".");
        PrintCounter = 0;
      }
    }
    //delay(1);
  }
}



void PrintState(CmdStatus_t CmdStatus)
{
  switch (CmdStatus)
  {
    case CMD_STATUS_READY: Serial.println("READY"); break;
    case CMD_STATUS_INVALID_PARAMS: Serial.println("INVALID_PARAMS"); break;
    case CMD_STATUS_PROCESSING: Serial.println("PROCESSING"); break;
    case CMD_STATUS_PROCESSING_DONE: Serial.println("PROCESSING_DONE"); break;
    default : Serial.println(CmdStatus, DEC); break;
  }
}
