#include "Arduino.h"

#include "adxl345_const.h"
#include "adxl345.h"
#include <SPI.h>

void adxl345::close(void)
{
  SPI.endTransaction();
  SPI.end();
}

void adxl345::init(void)
{
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
  SPI.begin();

  // initalize the chip select pins:
  pinMode(chipSelectPin, OUTPUT);

  setPowerControl(0x00);     //standby mode
  setDataFormatControl(0x0B);//Full resolution, +/-16g, 4mg/LSB.
  setDataRate(ADXL345_400HZ);
  setPowerControl(0x08);     //measurement mode
}

void adxl345::setPowerControl(byte settings)
{
  oneByteWrite(ADXL345_POWER_CTL_REG, settings);
}

void adxl345::setDataFormatControl(byte settings)
{
  oneByteWrite(ADXL345_DATA_FORMAT_REG, settings);
}

void adxl345::setDataRate(byte rate)
{
  //Get the current register contents, so we don't clobber the power bit.
  byte registerContents = oneByteRead(ADXL345_BW_RATE_REG);

  registerContents &= 0x10;
  registerContents |= rate;

  oneByteWrite(ADXL345_BW_RATE_REG, registerContents);
}

void adxl345::getOutput(int* readings)
{
    byte buffer[6];
    
    multiByteRead(ADXL345_DATAX0_REG, buffer, 6);
    
    readings[0] = (int)buffer[1] << 8 | (int)buffer[0];
    readings[1] = (int)buffer[3] << 8 | (int)buffer[2];
    readings[2] = (int)buffer[5] << 8 | (int)buffer[4];
}

void adxl345::oneByteWrite(byte address, byte data)
{
  int tx = (ADXL345_SPI_WRITE | (address & 0x3F));
    
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);
  
  SPI.transfer(tx);                 // send address
  SPI.transfer(data);               // send data
    
  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
}

int adxl345::oneByteRead(byte address)
{
  int tx = (ADXL345_SPI_READ | (address & 0x3F));
  int rx = 0;
  
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);
  
  SPI.transfer(tx);                 // send address
  rx = SPI.transfer(0x00);               // send data
    
  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);

  return rx;
}

void adxl345::multiByteRead(byte startAddress, byte* buffer, int size) 
{
  int tx = (ADXL345_SPI_READ | ADXL345_MULTI_BYTE | (startAddress & 0x3F));

  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(tx);                 // send address
  for (int i=0; i<size; i++) {
    buffer[i] = SPI.transfer(0x00);
  }

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
} 

