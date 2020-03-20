#include "spi_flash.h"

#include "i2c-lcd.h"
#include "delay.h"


typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

#define  FLASH_WRITE_ADDRESS      0x0
#define  FLASH_READ_ADDRESS       FLASH_WRITE_ADDRESS
#define  FLASH_SECTOR_TO_ERASE    FLASH_WRITE_ADDRESS

#define  BufferSize (countof(Tx_Buffer)-1)

#define countof(a) (sizeof(a) / sizeof(*(a)))

uint8_t Tx_Buffer[] = "SPI_LAB2";
uint8_t  Rx_Buffer[BufferSize];
__IO uint8_t Index = 0x0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = PASSED;
__IO uint32_t FlashID = 0;

TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);


int main(void)
{
	 // GPIOD Configuration
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitTypeDef GPIO_but;
	
	
	 if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
		
		
 
		//Pin 0
		GPIO_but.GPIO_Pin = GPIO_Pin_0;
		//Mode output
		GPIO_but.GPIO_Mode = GPIO_Mode_IN;
		//Output type push-pull
		GPIO_but.GPIO_OType = GPIO_OType_PP;
		//With pull down resistor
		GPIO_but.GPIO_PuPd = GPIO_PuPd_DOWN;
		//50MHz pin speed
		GPIO_but.GPIO_Speed = GPIO_Speed_50MHz;
		

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		//Initialize pin on GPIOA port
		GPIO_Init(GPIOA, &GPIO_but);
	
		
	
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		//GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		 
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	
		GPIO_Init(GPIOD, &GPIO_InitStruct);
		//GPIO_SetBits(GPIOD, GPIO_Pin_All);
		
	// GPIOD-PIN-15 ON

	//while(1){
		//int i;
		
		GPIO_SetBits(GPIOD, GPIO_Pin_All);
		Delay_MS(500);
		//delay_ms(500);
		//for(i=0;i<10000000;i++);
		GPIO_ResetBits(GPIOD, GPIO_Pin_All);
		Delay_MS(500);
		//for(i=0;i<10000000;i++);
	//}
		
	
  lcd_init ();
	
  sFLASH_Init();
	sFLASH_ReadBuffer(Rx_Buffer, FLASH_READ_ADDRESS, BufferSize);


  lcd_send_string ((char*)Rx_Buffer);

  DelayMs(1000);

  lcd_put_cur(1, 0);

  lcd_send_string("status register");
	
	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)!=1);
	
	Delay_MS(100);

  /* Get SPI Flash ID */
  FlashID = sFLASH_ReadID();

    /* OK: Turn on LD1 */
    //STM_LEDOn(LED1);

    sFLASH_EraseSector(FLASH_SECTOR_TO_ERASE);

    sFLASH_WriteBuffer(Tx_Buffer, FLASH_WRITE_ADDRESS, BufferSize);

    sFLASH_ReadBuffer(Rx_Buffer, FLASH_READ_ADDRESS, BufferSize);

    TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);

  GPIO_SetBits(GPIOD, GPIO_Pin_All);
		//Delay_MS(500);
  while (1)
  {}
}


TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

