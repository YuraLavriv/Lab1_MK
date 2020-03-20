
/** Put this in the src folder **/

#include "stm32f4xx_i2c.h"

#include "i2c-lcd.h"

#include "delay.h"

#define SLAVE_ADDRESS_LCD 0x4E // change this according to ur setup



void Start_I2C_Transmitting(){

	//For transmiting to the slave, start the data transmitting
	I2C_GenerateSTART(I2C1, ENABLE);
	 //Waiting for flag
	 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	 //Send address to slave
	 I2C_Send7bitAddress(I2C1, SLAVE_ADDRESS_LCD, I2C_Direction_Transmitter);
	//And check the transmitting
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

}

void Start_I2C_Receiving(){

	//For transmiting to the slave, start the data transmitting
	I2C_GenerateSTART(I2C1, ENABLE);
	 //Waiting for flag
	 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	 //Send address to slave
	 I2C_Send7bitAddress(I2C1, SLAVE_ADDRESS_LCD, I2C_Direction_Transmitter);
	//And check the transmitting
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

}

void Send_I2C_Data(uint8_t data)
{
	I2C_SendData(I2C1, data);
	// wait for the data trasnmitted flag
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

uint8_t Read_I2C_Data(){
	uint8_t data = I2C_ReceiveData(I2C1);
	return data;
}

void I2C_Master_Transmit( uint8_t * data, int  size){
	int i;
	Start_I2C_Transmitting();
	
	for(i=0; i<size; i++){
		Send_I2C_Data(data[i]);
	}
	I2C_GenerateSTOP(I2C1, ENABLE);
}	

void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	I2C_Master_Transmit ((uint8_t *) data_t, 4);
}

void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	I2C_Master_Transmit ((uint8_t *) data_t, 4);
}

void lcd_clear (void)
{
	int i;
	lcd_send_cmd (0x80);
	for (i=0; i<70; i++)
	{
		lcd_send_data (' ');
	}
}

void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcd_send_cmd (col);
}


void lcd_init (void)
{
	I2C_InitTypeDef I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	//Config the I2C
	//Set the clock speed of I2C. It has to be equal with the external device
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	//I2C mode
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	//I2C device adress
	I2C_InitStructure.I2C_OwnAddress1 = 0x33; 
	//I2C Acknowladge configuration
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	//Enable the I2C with the prepared configuration
	I2C_Init(I2C1, &I2C_InitStructure);

	//And start the I2C 
	I2C_Cmd(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	// 4 bit initialisation
	DelayMs(50);  // wait for >40ms
	lcd_send_cmd (0x30);
	DelayMs(5);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	DelayMs(1);  // wait for >100us
	lcd_send_cmd (0x30);
	DelayMs(10);
	lcd_send_cmd (0x20);  // 4bit mode
	DelayMs(10);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	DelayMs(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	DelayMs(1);
	lcd_send_cmd (0x01);  // clear display
	DelayMs(1);
	DelayMs(1);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	DelayMs(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}
