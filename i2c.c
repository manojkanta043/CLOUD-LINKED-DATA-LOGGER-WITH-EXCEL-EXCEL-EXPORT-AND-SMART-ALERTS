//i2c.c
#include "types.h"
#include <LPC21xx.h>
#include "pin_connect_block.h"
#include "i2c.h"
#include "i2c_defines.h"

void init_i2c(void)
{
	//Cfg p0.2 for SCL functions 
  CfgPortPinFunc(0,2,SCL_PIN_0_2);
  //Cfg p0.3 for SDA functions  
  CfgPortPinFunc(0,3,SDA_PIN_0_3);
  //Cfg Speed for I2C Serial Communication 
  I2SCLL=BITRATE; 
  I2SCLH=BITRATE; 
  //I2C Peripheral Enable for Communication 
  I2CONSET=1<<I2EN_BIT;  
}

void i2c_start(void)
{
  //initiate start event
  I2CONSET=1<<STA_BIT;
  //wait for start event completion/status	
	while(((I2CONSET>>SI_BIT)&1)==0);
	//clear start event bit
	I2CONCLR=1<<STAC_BIT;
}

void i2c_restart(void)
{
	//initiate restart event
  I2CONSET=1<<STA_BIT;
	//clr prev SIC_BIT to trigger restart 
  I2CONCLR=1<<SIC_BIT; 
  //wait for start event completion/status	
	while(((I2CONSET>>SI_BIT)&1)==0);
	//clear restart event bit
	I2CONCLR=1<<STAC_BIT;
}

void i2c_stop(void)
{
  //initiate stop event
  I2CONSET=1<<STO_BIT;
  //clear prev SIC_BIT to trigger stop
  I2CONCLR=1<<SIC_BIT;	
}

/*void i2c_write(u8 sDat)
{
	//write to tx-buffer
	I2DAT=sDat;
	//clear prev SIC_BIT to trigger
	//serialization out msb to lsb order
	I2CONCLR=1<<SIC_BIT;
	//wait until serialization completion
	while(((I2CONSET>>SI_BIT)&1)==0);
	//& assuming positive ack from slave
}*/

u8 i2c_nack(void)
{
	//clear sic_bit to trigger
	//serialization in msb to lsb order
	I2CONCLR=1<<SIC_BIT; 
  //wait until serialization completion
  while(((I2CONSET>>SI_BIT)&1)==0); 
	//& issue nack to slave
	
	//read recvd byte
  return I2DAT; 
}

u8 i2c_mack(void)
{
	//set to issue mack to slave
	I2CONSET=1<<AA_BIT;
	//clear sic_bit to trigger
	//serialization in msb to lsb order
	I2CONCLR=1<<SIC_BIT; 
  //wait until serialization completion
  while(((I2CONSET>>SI_BIT)&1)==0); 
	//& issue mack to slave
	//& clear AA_BIT
	I2CONCLR=1<<AAC_BIT;
	
	//read recvd byte
  return I2DAT; 

}
u8 i2c_write(u8 sDat)
{
    I2DAT = sDat;

    // Clear SI to start transmission
    I2CONCLR = (1 << SIC_BIT);

    // Wait for transmission complete
    while(((I2CONSET >> SI_BIT) & 1) == 0);

    // Return I2C Status
    return I2STAT;
}
