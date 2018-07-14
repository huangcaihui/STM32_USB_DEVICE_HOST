/**
  ******************************************************************************
  * @file    app.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file provides all the Application firmware functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/

#include "usbd_hid_core.h"
//#include "usbd_msc_core.h"
#include "usbd_cdc_core.h"
//#include "usbd_cdc_core_loopback.h"
#include "usbd_usr.h"
//#include "usbd_msc_desc.h"
#include "usbd_vcp_desc.h"
#include "usbd_hid_desc.h"
#include "usb_conf.h"
#include "usbd_cdc_vcp.h"
#include "usb_bsp.h"
/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

#include <stdio.h>
/** @defgroup APP_DUAL_CORE 
  * @brief Mass storage application module
  * @{
  */ 

/** @defgroup APP_DUAL_CORE_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup APP_DUAL_CORE_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup APP_DUAL_CORE_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup APP_DUAL_CORE_Private_Variables
  * @{
  */ 
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

__ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_FS_dev __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_HS_dev __ALIGN_END;

uint8_t Rxbuffer[256]; 
volatile uint32_t receive_count =1;
int need_reboot = 0;
/**
  * @brief  Program entry point
  * @param  None
  * @retval None
  */
#define LED_R_ON() GPIO_ResetBits(GPIOC,GPIO_Pin_3);
#define LED_R_OFF() GPIO_SetBits(GPIOC,GPIO_Pin_3);
/*------------------------------------------------------------------------------
 *  SysTick_Handler
 *----------------------------------------------------------------------------*/
volatile uint32_t jiff = 0;

int jiff_flag = 0;
int led_on_ms = 200;
int led_off_ms = 800;

#if 0
#define ADDR_24LC02		0xA0
#define I2C_PAGESIZE	4		/* 24C01/01A页缓冲是4个 */

/* Define I2C Speed ----------------------------------------------------------*/
#ifdef FAST_I2C_MODE
 #define I2C_SPEED 340000
 #define I2C_DUTYCYCLE I2C_DutyCycle_16_9  

#else /* STANDARD_I2C_MODE*/
 #define I2C_SPEED 100000
 #define I2C_DUTYCYCLE  I2C_DutyCycle_2
#endif /* FAST_I2C_MODE*/
  
/* Define Slave Address  -----------------------------------------------------*/
#ifdef I2C_10BITS_ADDRESS
 #define SLAVE_ADDRESS (uint16_t)0x0330

#else /* I2C_7BITS_ADDRESS */
 #define SLAVE_ADDRESS 0x30
#endif /* I2C_10BITS_ADDRESS */
 
/* USER_TIMEOUT value for waiting loops. This timeout is just a guarantee that the
   application will not remain stuck if the I2C communication is corrupted. 
   You may modify this timeout value depending on CPU frequency and application
   conditions (interrupts routines, number of data to transfer, speed, CPU
   frequency...). */ 
#define USER_TIMEOUT                  ((uint32_t)0x64) /* Waiting 1s */  

/* I2Cx Communication boards Interface */
#define I2Cx_DMA                      DMA1
#define I2Cx_DMA_CHANNEL              DMA_Channel_1
#define I2Cx_DR_ADDRESS               ((uint32_t)0x40005410)
#define I2Cx_DMA_STREAM_TX            DMA1_Stream6
#define I2Cx_DMA_STREAM_RX            DMA1_Stream0
#define I2Cx_TX_DMA_TCFLAG            DMA_FLAG_TCIF6
#define I2Cx_TX_DMA_FEIFLAG           DMA_FLAG_FEIF6
#define I2Cx_TX_DMA_DMEIFLAG          DMA_FLAG_DMEIF6
#define I2Cx_TX_DMA_TEIFLAG           DMA_FLAG_TEIF6
#define I2Cx_TX_DMA_HTIFLAG           DMA_FLAG_HTIF6
#define I2Cx_RX_DMA_TCFLAG            DMA_FLAG_TCIF0
#define I2Cx_RX_DMA_FEIFLAG           DMA_FLAG_FEIF0
#define I2Cx_RX_DMA_DMEIFLAG          DMA_FLAG_DMEIF0
#define I2Cx_RX_DMA_TEIFLAG           DMA_FLAG_TEIF0
#define I2Cx_RX_DMA_HTIFLAG           DMA_FLAG_HTIF0
#define DMAx_CLK                      RCC_AHB1Periph_DMA1    

#define I2Cx                          I2C2
#define I2Cx_CLK                      RCC_APB1Periph_I2C2
#define I2Cx_SDA_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define I2Cx_SDA_PIN                  GPIO_Pin_11                
#define I2Cx_SDA_GPIO_PORT            GPIOB                       
#define I2Cx_SDA_SOURCE               GPIO_PinSource11
#define I2Cx_SDA_AF                   GPIO_AF_I2C2
  
#define I2Cx_SCL_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define I2Cx_SCL_PIN                  GPIO_Pin_10               
#define I2Cx_SCL_GPIO_PORT            GPIOB                    
#define I2Cx_SCL_SOURCE               GPIO_PinSource10
#define I2Cx_SCL_AF                   GPIO_AF_I2C2

#define countof(a)   (sizeof(a) / sizeof(*(a)))  

#define BUFFER_SIZE 64
uint32_t aTxBuffer[BUFFER_SIZE];
uint32_t aRxBuffer[BUFFER_SIZE];

#define TXBUFFERSIZE   (countof(aTxBuffer) - 1)
#define RXBUFFERSIZE   TXBUFFERSIZE

static void I2C_Config(void)
#if 1
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;  
  /* RCC Configuration */
  /*I2C Peripheral clock enable */
  RCC_APB1PeriphClockCmd(I2Cx_CLK, ENABLE);
  
  /*SDA GPIO clock enable */
  RCC_AHB1PeriphClockCmd(I2Cx_SDA_GPIO_CLK, ENABLE);
  
  /*SCL GPIO clock enable */
  RCC_AHB1PeriphClockCmd(I2Cx_SCL_GPIO_CLK, ENABLE);
  
  /* Reset I2Cx IP */
  RCC_APB1PeriphResetCmd(I2Cx_CLK, ENABLE);
  
  /* Release reset signal of I2Cx IP */
  RCC_APB1PeriphResetCmd(I2Cx_CLK, DISABLE);
  
  /* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(DMAx_CLK, ENABLE);
  
  /* GPIO Configuration */
  /*Configure I2C SCL pin */
  GPIO_InitStructure.GPIO_Pin = I2Cx_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStructure);
  
  /*Configure I2C SDA pin */
  GPIO_InitStructure.GPIO_Pin = I2Cx_SDA_PIN;
  GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStructure);
    
  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_SOURCE, I2Cx_SCL_AF);
  
  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_SOURCE, I2Cx_SDA_AF);

  /* DMA Configuration */
  /* Clear any pending flag on Tx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_TX, I2Cx_TX_DMA_TCFLAG | I2Cx_TX_DMA_FEIFLAG | I2Cx_TX_DMA_DMEIFLAG | \
                                       I2Cx_TX_DMA_TEIFLAG | I2Cx_TX_DMA_HTIFLAG);
                                       
  /* Clear any pending flag on Rx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_RX, I2Cx_RX_DMA_TCFLAG | I2Cx_RX_DMA_FEIFLAG | I2Cx_RX_DMA_DMEIFLAG | \
                                       I2Cx_RX_DMA_TEIFLAG | I2Cx_RX_DMA_HTIFLAG);
  
  /* Disable the I2C Tx DMA stream */
  DMA_Cmd(I2Cx_DMA_STREAM_TX, DISABLE);
  /* Configure the DMA stream for the I2C peripheral TX direction */
  DMA_DeInit(I2Cx_DMA_STREAM_TX);
  
  /* Disable the I2C Rx DMA stream */
  DMA_Cmd(I2Cx_DMA_STREAM_RX, DISABLE);
  /* Configure the DMA stream for the I2C peripheral RX direction */
  DMA_DeInit(I2Cx_DMA_STREAM_RX);
  
  /* Initialize the DMA_Channel member */
  DMA_InitStructure.DMA_Channel = I2Cx_DMA_CHANNEL;
  
  /* Initialize the DMA_PeripheralBaseAddr member */
  DMA_InitStructure.DMA_PeripheralBaseAddr = I2Cx_DR_ADDRESS;
  
  /* Initialize the DMA_PeripheralInc member */         
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  
  /* Initialize the DMA_MemoryInc member */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  
  /* Initialize the DMA_PeripheralDataSize member */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  
  /* Initialize the DMA_MemoryDataSize member */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  
  /* Initialize the DMA_Mode member */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  
  /* Initialize the DMA_Priority member */
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  
  /* Initialize the DMA_FIFOMode member */
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  
  /* Initialize the DMA_FIFOThreshold member */
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  
  /* Initialize the DMA_MemoryBurst member */
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  
  /* Initialize the DMA_PeripheralBurst member */
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  
  /* Init DMA for Reception */
   /* Initialize the DMA_DIR member */
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
   /* Initialize the DMA_Memory0BaseAddr member */
   DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)aRxBuffer;
   /* Initialize the DMA_BufferSize member */
   DMA_InitStructure.DMA_BufferSize = RXBUFFERSIZE;
   DMA_DeInit(I2Cx_DMA_STREAM_RX);
   DMA_Init(I2Cx_DMA_STREAM_RX, &DMA_InitStructure);
  
  /* Init DMA for Transmission */
   /* Initialize the DMA_DIR member */
   DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
   /* Initialize the DMA_Memory0BaseAddr member */
   DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)aTxBuffer;
   /* Initialize the DMA_BufferSize member */
   DMA_InitStructure.DMA_BufferSize = TXBUFFERSIZE;
   DMA_DeInit(I2Cx_DMA_STREAM_TX);
   DMA_Init(I2Cx_DMA_STREAM_TX, &DMA_InitStructure);
   
   /* Configure I2C Filters */
   I2C_AnalogFilterCmd(I2Cx,ENABLE);
   I2C_DigitalFilterConfig(I2Cx,0x0F);
   
  /* I2C ENABLE */
  //I2C_Cmd(I2Cx, ENABLE);
}

#else
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  I2C_InitTypeDef   I2C_InitStructure;
  RCC_ClocksTypeDef   rcc_clocks;
  /* GPIO Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
      /* Reset I2Cx IP */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);
    /* Release reset signal of I2Cx IP */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);
  /*I2C2 configuration*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2); //??,????????GPIO_PinSource6|GPIO_PinSource7
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);

    //PB10: I2C2_SCL  PB11: I2C2_SDA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* I2C Struct Initialize */
    I2C_DeInit(I2C2);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x0A;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &I2C_InitStructure);

    /* I2C Initialize */
    I2C_Cmd(I2C2, ENABLE);  
    
    /*????*/
    RCC_GetClocksFreq(&rcc_clocks);
    //ulTimeOut_Time = (rcc_clocks.SYSCLK_Frequency /10000); 
}
#endif
//#endif
//#if 1
static void SysTickConfig(void)
{
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
  
  /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}

static void I2C_delay(uint16_t cnt)
{
	while(cnt--);
}

static void I2C_AcknowledgePolling(I2C_TypeDef *I2Cx,uint8_t I2C_Addr)
{
  vu16 SR1_Tmp;
  do
  {   
    I2C_GenerateSTART(I2Cx, ENABLE); /*起始位*/
    /*读SR1*/
    SR1_Tmp = I2C_ReadRegister(I2Cx, I2C_Register_SR1);
    /*器件地址(写)*/
#ifdef AT24C01A

	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
#else

	I2C_Send7bitAddress(I2Cx, 0, I2C_Direction_Transmitter);
#endif

  }while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002));
  
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
    
  I2C_GenerateSTOP(I2Cx, ENABLE);  /*停止位*/  
}

uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t value)

{
    /* 起始位 */
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));  

#ifdef AT24C01A
    /* 发送器件地址(写)*/
    I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
    /*发送地址*/
    I2C_SendData(I2Cx, addr);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#else	
    I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
#endif

    /* 写一个字节*/
    I2C_SendData(I2Cx, value); 
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    /* 停止位*/
    I2C_GenerateSTOP(I2Cx, ENABLE);
  
    I2C_AcknowledgePolling(I2Cx,I2C_Addr);

    I2C_delay(1000);

    return 0;
}




uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
    if(num==0)
	return 1;
	
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
		
	/*允许1字节1应答模式*/
	I2C_AcknowledgeConfig(I2Cx, ENABLE);


	/* 发送起始位 */
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,主模式*/

#ifdef AT24C01A	
    /*发送器件地址(写)*/
    I2C_Send7bitAddress(I2Cx,  I2C_Addr, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/*发送地址*/
	I2C_SendData(I2Cx, addr);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));/*数据已发送*/
		
	/*起始位*/
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	
	/*器件读*/
	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		
#else	
	/*发送器件地址(读)24C01*/
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
#endif
	
    while (num)
    {
		if(num==1)
		{
     		I2C_AcknowledgeConfig(I2Cx, DISABLE);	/* 最后一位后要关闭应答的 */
    		I2C_GenerateSTOP(I2Cx, ENABLE);			/* 发送停止位 */
		}
	    
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));  /* EV7 */
	    *buf = I2C_ReceiveData(I2Cx);
	    buf++;
	    /* Decrement the read bytes counter */
	    num--;
    }
	/* 再次允许应答模式 */
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	return 0;
}

uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
	uint8_t err=0;
	
	while(num--)
	{
        printf(" I2C_Write num=%d 000 \n",num);
		if(I2C_WriteOneByte(I2Cx, I2C_Addr,addr++,*buf++))
		{
			err++;
		}
	}
	if(err)
	{
        printf(" I2C_Write err 111 \n");
		return 1;
	}
	else 
	{
        printf(" I2C_Write err 222 \n");
		return 0;	
	}
}

void RS232_Send_Data(unsigned char *send_buff,unsigned int length)
{
 	unsigned int i = 0;
	for(i = 0;i < length;i ++)
	{			
		UART4->DR = send_buff[i];
		while((UART4->SR&0X40)==0);	
	}	
}

static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLength)
{
  uint16_t index = 0;
  
  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++ )
  {
    pBuffer[index] = 0x00;
  }
}

#if 0
void I2C_Test(void)
{
	unsigned int i = 0;
	unsigned char WriteBuffer[64];
	unsigned char ReadBuffer[64];
	for(i = 0;i < 64;i ++)//初始化待写数据区
	{
		WriteBuffer[i] = i;
        printf("WriteBuffer[%d]=%x \n",i,WriteBuffer[i]);
	}
	I2C_Write(I2C2,ADDR_24LC02,0,WriteBuffer,sizeof(WriteBuffer));	//向EEPROM写数据
	I2C_Read(I2C2,ADDR_24LC02,0,ReadBuffer,sizeof(WriteBuffer));	//向EEPROM读数据
	RS232_Send_Data(ReadBuffer,64);								//通RS232将读到的数据发出去	
}
#else
void I2C_Test(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s)
       before to branch to application main. To reconfigure the default setting 
       of SystemInit() function, refer to system_stm32f4xx.c file
     */    

  /* I2C configuration ---------------------------------------------------------*/
  //I2C_Config();
  
  /* SysTick configuration -----------------------------------------------------*/
  SysTickConfig();
  
  /* Clear the RxBuffer */
  Fill_Buffer(aRxBuffer, RXBUFFERSIZE);
  
  /*************************************Master Code******************************/
#if defined (I2C_MASTER)
 
  /* I2C Struct Initialize */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DUTYCYCLE;
  I2C_InitStructure.I2C_OwnAddress1 = 0xA0;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  
#ifndef I2C_10BITS_ADDRESS
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
#else
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
#endif /* I2C_10BITS_ADDRESS */
  
  /* I2C Initialize */
  I2C_Init(I2Cx, &I2C_InitStructure);
  
  /* Master Transmitter --------------------------------------------------------*/   
  
  /* Generate the Start condition */
  I2C_GenerateSTART(I2Cx, ENABLE);
  
#ifdef I2C_10BITS_ADDRESS  
  /* Test on I2C1 EV5 and clear it */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send Header to I2Cx for write or time out */
  I2C_SendData(I2Cx, HEADER_ADDRESS_Write);
  /* Test on I2Cx EV9 and clear it */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_ADDRESS10))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send I2Cx slave Address for write */
  I2C_Send7bitAddress(I2Cx, (uint8_t)SLAVE_ADDRESS, I2C_Direction_Transmitter);
  
 #else /* I2C_7BITS_ADDRESS */
      
  /* Test on I2Cx EV5 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send I2Cx slave Address for write */
  I2C_Send7bitAddress(I2Cx, SLAVE_ADDRESS, I2C_Direction_Transmitter);
      
#endif /* I2C_10BITS_ADDRESS */
  
  /* Test on I2Cx EV6 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* I2Cx DMA Enable */
  I2C_DMACmd(I2Cx, ENABLE);
  
  /* Enable DMA TX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_TX, ENABLE);
  
  /* Wait until I2Cx_DMA_STREAM_TX enabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_TX)!= ENABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Wait until DMA Transfer Complete or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetFlagStatus(I2Cx_DMA_STREAM_TX,I2Cx_TX_DMA_TCFLAG) == RESET)&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* I2Cx DMA Disable */
  I2C_DMACmd(I2Cx, DISABLE);
  
  /* Wait until BTF Flag is set before generating STOP or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_GetFlagStatus(I2Cx,I2C_FLAG_BTF))&&(TimeOut != 0x00))  
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Send I2Cx STOP Condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  /* Disable DMA TX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_TX, DISABLE);
  
  /* Wait until I2Cx_DMA_STREAM_TX disabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_TX)!= DISABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Clear any pending flag on Tx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_TX, I2Cx_TX_DMA_TCFLAG | I2Cx_TX_DMA_FEIFLAG | I2Cx_TX_DMA_DMEIFLAG | \
                                       I2Cx_TX_DMA_TEIFLAG | I2Cx_TX_DMA_HTIFLAG);
  
  /* Master Receiver -----------------------------------------------------------*/ 
  
  /* Enable DMA NACK automatic generation */
  I2C_DMALastTransferCmd(I2Cx, ENABLE);
  
  /* Send I2Cx START condition */
  I2C_GenerateSTART(I2Cx, ENABLE);
  
#ifdef I2C_10BITS_ADDRESS  
  /* Test on EV5 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send Header to Slave for write */
  I2C_SendData(I2Cx, HEADER_ADDRESS_Write);

  /* Test on EV9 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_ADDRESS10))&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send slave Address */
  I2C_Send7bitAddress(I2Cx, (uint8_t)SLAVE_ADDRESS, I2C_Direction_Transmitter);

  /* Test on I2Cx EV6 and clear it or time out*/
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Repeated Start */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Test on EV5 and clear it or time out*/
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send Header to Slave for Read */
  I2C_SendData(I2Cx, HEADER_ADDRESS_Read);

#else /* I2C_7BITS_ADDRESS */
      
  /* Test on I2Cx EV5 and clear it or time out*/
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send I2Cx slave Address for write */
  I2C_Send7bitAddress(I2Cx, SLAVE_ADDRESS, I2C_Direction_Receiver);
      
#endif /* I2C_10BITS_ADDRESS */
  
  /* Test on I2Cx EV6 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* I2Cx DMA Enable */
  I2C_DMACmd(I2Cx, ENABLE);
  
  /* Enable DMA RX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_RX, ENABLE);
  
  /* Wait until I2Cx_DMA_STREAM_RX enabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_RX)!= ENABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Transfer complete or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetFlagStatus(I2Cx_DMA_STREAM_RX,I2Cx_RX_DMA_TCFLAG)==RESET)&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send I2Cx STOP Condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  /* Disable DMA RX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_RX, DISABLE);
  
  /* Wait until I2Cx_DMA_STREAM_RX disabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_RX)!= DISABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(I2Cx,DISABLE);
  
  /* Clear any pending flag on Rx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_RX, I2Cx_RX_DMA_TCFLAG | I2Cx_RX_DMA_FEIFLAG | I2Cx_RX_DMA_DMEIFLAG | \
                                       I2Cx_RX_DMA_TEIFLAG | I2Cx_RX_DMA_HTIFLAG);
  
  if (Buffercmp(aTxBuffer, aRxBuffer, RXBUFFERSIZE) == PASSED)
  {
    /* LED2, LED3 and LED4 On */
    STM_EVAL_LEDOn(LED2);
    STM_EVAL_LEDOn(LED3);
    STM_EVAL_LEDOn(LED4);
  }
  else 
  {   /* LED2, LED3 and LED4 Off */
    STM_EVAL_LEDOff(LED2);
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED4);
  }
  
#endif /* I2C_MASTER */
  
/**********************************Slave Code**********************************/
#if defined (I2C_SLAVE)
  
  /* Initialize I2C peripheral */
  /* I2C Init */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DUTYCYCLE;
  I2C_InitStructure.I2C_OwnAddress1 = SLAVE_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  
#ifndef I2C_10BITS_ADDRESS
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
#else
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
#endif /* I2C_10BITS_ADDRESS */
  
  I2C_Init(I2Cx, &I2C_InitStructure);
  
  /* Slave Receiver ------------------------------------------------------------*/

  /* Test on I2C EV1 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED))
  {}

  /* I2Cx DMA Enable */
  I2C_DMACmd(I2Cx, ENABLE); 
  
  /* Enable DMA RX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_RX, ENABLE);
  
  /* Wait until I2Cx_DMA_STREAM_RX enabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_RX)!= ENABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Transfer complete or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetFlagStatus(I2Cx_DMA_STREAM_RX,I2Cx_RX_DMA_TCFLAG)==RESET)&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }  
  /* Test on I2Cx EV4 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_STOP_DETECTED))&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }  
  /* Disable DMA RX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_RX, DISABLE);
  
  /* Wait until I2Cx_DMA_STREAM_RX disabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_RX)!= DISABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(I2Cx,DISABLE);
  
  /* Clear any pending flag on Rx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_RX, I2Cx_RX_DMA_TCFLAG | I2Cx_RX_DMA_FEIFLAG | I2Cx_RX_DMA_DMEIFLAG | \
                                       I2Cx_RX_DMA_TEIFLAG | I2Cx_RX_DMA_HTIFLAG);
  
/* Slave Transmitter ---------------------------------------------------------*/   
  
  /* Test on I2C EV1 and clear it or time out*/
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED))
  {}
  /* I2Cx DMA Enable */
  I2C_DMACmd(I2Cx, ENABLE); 
  
  /* Enable DMA RX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_TX, ENABLE);
  
  /* Wait until I2Cx_DMA_STREAM_TX enabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_TX)!= ENABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Transfer complete or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetFlagStatus(I2Cx_DMA_STREAM_TX,I2Cx_TX_DMA_TCFLAG)==RESET)&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Test on I2Cx EV3-2 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_ACK_FAILURE))&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Disable DMA TX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_TX, DISABLE);
  
  /* Wait until I2Cx_DMA_STREAM_TX disabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_TX)!= DISABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(I2Cx,DISABLE);
  
    /* Clear any pending flag on Tx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_TX, I2Cx_TX_DMA_TCFLAG | I2Cx_TX_DMA_FEIFLAG | I2Cx_TX_DMA_DMEIFLAG | \
                                       I2Cx_TX_DMA_TEIFLAG | I2Cx_TX_DMA_HTIFLAG);
  
  if (Buffercmp(aTxBuffer, aRxBuffer, RXBUFFERSIZE) == PASSED)
  {
    /* LED2, LED3 and LED4 are On */
    STM_EVAL_LEDOn(LED2);
    STM_EVAL_LEDOn(LED3);
    STM_EVAL_LEDOn(LED4);
  }
  else 
  {   /* LED2, LED3 and LED4 are Off */
    STM_EVAL_LEDOff(LED2);
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED4);
  } 
  
#endif /* I2C_SLAVE */
  while(1)
  {}
}
#endif
#endif
#if 0
#define I2C1_DR_Address	((u32)0x40005410)
#define I2C_TIME		((u32)65535)
#define I2C1_SPEED	((u32)400000)

#define TUNER_IIC_BUS   0x02


/*=====================================================================================================*/
/*=====================================================================================================*/
vu8* I2C_ReadPtr;
vu8* I2C_WritePtr;
vu32 I2C_TimeCnt = I2C_TIME;
/*=====================================================================================================*/
/*=====================================================================================================*/
DMA_InitTypeDef DMA_InitStruct;
/*=====================================================================================================*/
/*=====================================================================================================*/
u32 I2C_TimeOut( void )
{
  while(1) {
    printf("I2C_TimeOut 111 \n");
  }
}

void I2C_Config( void )
{
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);  

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = DMA1_Stream0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel = DMA1_Stream6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 |
															DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6);
	DMA_Cmd(DMA1_Stream6, DISABLE);
	DMA_DeInit(DMA1_Stream6);
	DMA_InitStruct.DMA_Channel = DMA_Channel_1;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)I2C1_DR_Address;
	DMA_InitStruct.DMA_Memory0BaseAddr = (u32)0;
	DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStruct.DMA_BufferSize = 0xFFFF;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream6, &DMA_InitStruct);

	DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_FEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_TEIF0 |
															DMA_FLAG_HTIF0 | DMA_FLAG_TCIF0);
	DMA_Cmd(DMA1_Stream0, DISABLE);
	DMA_DeInit(DMA1_Stream0);
	DMA_InitStruct.DMA_Channel = DMA_Channel_1;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)I2C1_DR_Address;
	DMA_InitStruct.DMA_Memory0BaseAddr = (u32)0;
	DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStruct.DMA_BufferSize = 0xFFFF;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream0, &DMA_InitStruct);

	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Stream0, DMA_IT_TC, ENABLE);

	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = I2C1_SPEED;
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStruct);

	I2C_DMACmd(I2C1, ENABLE);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
u32 I2C_DMA_Read( u8* ReadBuf, u8 SlaveAddr, u8 ReadAddr, u8* NumByte )
{
	I2C_ReadPtr = NumByte;

	I2C_TimeCnt = I2C_TIME;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
		if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

	I2C_GenerateSTART(I2C1, ENABLE);

	I2C_TimeCnt = I2C_TIME;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
		if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

	I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Transmitter);

	I2C_TimeCnt = I2C_TIME;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

	I2C_SendData(I2C1, ReadAddr);

	I2C_TimeCnt = I2C_TIME;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
		if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

	I2C_GenerateSTART(I2C1, ENABLE);

	I2C_TimeCnt = I2C_TIME;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
		if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

	I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Receiver);

	if((u16)(*NumByte) < 2) {
		I2C_TimeCnt = I2C_TIME;
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET)
			if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

		I2C_AcknowledgeConfig(I2C1, DISABLE);
		(void)I2C1->SR2;

		I2C_GenerateSTOP(I2C1, ENABLE);

		I2C_TimeCnt = I2C_TIME;
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET)
			if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

		*ReadBuf = I2C_ReceiveData(I2C1);

		(u16)(*NumByte)--;

		I2C_TimeCnt = I2C_TIME;
		while(I2C1->CR1 & I2C_CR1_STOP)
			if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

		I2C_AcknowledgeConfig(I2C1, ENABLE);
	}
	else {
		I2C_TimeCnt = I2C_TIME;
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
			if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

		DMA_InitStruct.DMA_Channel = DMA_Channel_1;
		DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)I2C1_DR_Address;
		DMA_InitStruct.DMA_Memory0BaseAddr = (u32)ReadBuf;
		DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
		DMA_InitStruct.DMA_BufferSize = (u32)(*NumByte);
		DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
		DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
		DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
		DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA1_Stream0, &DMA_InitStruct);

		I2C_DMALastTransferCmd(I2C1, ENABLE);

		DMA_Cmd(DMA1_Stream0, ENABLE);
	}

	I2C_TimeCnt = I2C_TIME;
	while(*NumByte > 0)
		if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

	return SUCCESS;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
u32 I2C_DMA_ReadReg( u8* ReadBuf, u8 SlaveAddr, u8 ReadAddr, u8 NumByte )
{
	I2C_DMA_Read(ReadBuf, SlaveAddr, ReadAddr, (u8*)(&NumByte));

	I2C_TimeCnt = I2C_TIME;
	while(NumByte > 0)
		if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

	return SUCCESS;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
u32 I2C_DMA_Write( u8* WriteBuf, u8 SlaveAddr, u8 WriteAddr, u8* NumByte )
{
	I2C_WritePtr = NumByte;

	I2C_TimeCnt = I2C_TIME;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
		if((I2C_TimeCnt--) == 0) return I2C_TimeOut();

	I2C_GenerateSTART(I2C1, ENABLE);

	I2C_TimeCnt = I2C_TIME;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
		if((I2C_TimeCnt--) == 0) return I2C_TimeOut();

	I2C_TimeCnt = I2C_TIME;
	I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Transmitter);

	I2C_TimeCnt = I2C_TIME;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		if((I2C_TimeCnt--) == 0) return I2C_TimeOut();

	I2C_SendData(I2C1, WriteAddr);

	I2C_TimeCnt = I2C_TIME;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
		if((I2C_TimeCnt--) == 0) return I2C_TimeOut();

	DMA_InitStruct.DMA_Channel = DMA_Channel_1;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)I2C1_DR_Address;
	DMA_InitStruct.DMA_Memory0BaseAddr = (u32)WriteBuf;
	DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStruct.DMA_BufferSize = (u32)(*NumByte);
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream6, &DMA_InitStruct);

	DMA_Cmd(DMA1_Stream6, ENABLE);

	return SUCCESS;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
u32 I2C_DMA_WriteReg( u8* WriteBuf, u8 SlaveAddr, u8 WriteAddr, u8 NumByte )
{
	I2C_DMA_Write(WriteBuf, SlaveAddr, WriteAddr, (u8*)(&NumByte));

	I2C_TimeCnt = I2C_TIME;
	while(NumByte > 0)
		if((I2C_TimeCnt--) == 0)	return I2C_TimeOut();

	return SUCCESS;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
void I2C1_Send_DMA_IRQ( void )
{
	if(DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) != RESET) {
		DMA_Cmd(DMA1_Stream6, DISABLE);
		DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);

		I2C_TimeCnt = I2C_TIME;
		while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF))
			if((I2C_TimeCnt--) == 0) I2C_TimeOut();

		I2C_GenerateSTOP(I2C1, ENABLE);
		*I2C_WritePtr = 0;
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*/
void I2C1_Recv_DMA_IRQ( void )
{
	if(DMA_GetFlagStatus(DMA1_Stream0, DMA_FLAG_TCIF0) != RESET) {
		I2C_GenerateSTOP(I2C1, ENABLE);
		DMA_Cmd(DMA1_Stream0, DISABLE);
		DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0);
		*I2C_ReadPtr = 0;
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*/
void RS232_Send_Data(unsigned char *send_buff,unsigned int length)
{
 	unsigned int i = 0;
	for(i = 0;i < length;i ++)
	{			
		UART4->DR = send_buff[i];
		while((UART4->SR&0X40)==0);	
	}	
}

void I2C_Test(void)
{
	unsigned int i = 0;
    u8 Device_Addr= 0x20;
    u8 write_Addr = 0x02;
    u8 buff_size =  64;
    u8 WriteBuffer[64];
	u8 ReadBuffer[64];
	//unsigned char WriteBuffer[64];
	//unsigned char ReadBuffer[64];
	for(i = 0;i < 64;i ++)//初始化待写数据区
	{
		WriteBuffer[i] = i;
        printf("WriteBuffer[%d]=%x \n",i,WriteBuffer[i]);
	}
	//I2C_Write(I2C2,ADDR_24LC02,0,WriteBuffer,sizeof(WriteBuffer));	//向EEPROM写数据
	//I2C_Read(I2C2,ADDR_24LC02,0,ReadBuffer,sizeof(WriteBuffer));	//向EEPROM读数据

    //I2C_DMA_Write( u8* WriteBuf, u8 SlaveAddr, u8 WriteAddr, u8* NumByte )
	I2C_DMA_Write( WriteBuffer, Device_Addr+(TUNER_IIC_BUS<<8), write_Addr, &(buff_size) );
	RS232_Send_Data(ReadBuffer,64);								//通RS232将读到的数据发出去	
}

/*=====================================================================================================*/

#endif
void SysTick_Handler(void) //
{
    jiff++;
    jiff_flag = 1;

    //
    if(led_on_ms > 0)
    {
        led_on_ms--;
        if(led_on_ms == 0)
        {
			//printf("led_on_ms = %d 111 \n",led_on_ms);
            LED_R_OFF();
        }
    }
    else// if(led_off_ms)
    {
        led_off_ms--;
        if(led_off_ms == 0)
        {
			//printf("led_on_ms = %d 222 \n",led_on_ms);
            LED_R_ON();
            led_on_ms = 200;
            led_off_ms = 800;
        }
    }
}

uint32_t tick_settime(uint32_t t)
{
    return jiff+t;
}
uint32_t tick_elapsed_time(uint32_t t)
{
    return (t -  jiff) & 0x80000000;
}


void led_init(void)
{  
    GPIO_InitTypeDef GPIO_init_l;//?????????  
      
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);   //??  
    GPIO_init_l.GPIO_Pin=GPIO_Pin_3;  
    GPIO_init_l.GPIO_Mode=GPIO_Mode_OUT;  
    GPIO_init_l.GPIO_OType=GPIO_OType_PP;  
    GPIO_init_l.GPIO_Speed=GPIO_Speed_100MHz;  
    GPIO_init_l.GPIO_PuPd=GPIO_PuPd_NOPULL;    
    GPIO_Init(GPIOC,&GPIO_init_l);  
    //GPIOC->BSRRL=GPIO_Pin_3;
    //GPIO_ResetBits(GPIOC,GPIO_Pin_3);
		LED_R_OFF();
    
}

/**
  * @brief  Program entry point
  * @param  None
  * @retval None
  */

extern CDC_IF_Prop_TypeDef  APP_VCP_FOPS;

int dly_cnt = 0;
int dly_repet = 0;

extern int my_usb_status;
extern int usb_check;
extern int usbd_cdc_trigger_reset(void);
extern u8 USBD_HID_RcvBuf[64];

int cdc_configer_sr = 0;

int fgetc(FILE *f) 
{
    int ch;
    while (USART_GetFlagStatus(UART4, USART_FLAG_RXNE) == RESET);
    ch = USART_ReceiveData(UART4);
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
    USART_SendData(UART4, (uint8_t) ch);
    return ch;
}

int main(void)
{
    int stop;
    
    uint32_t i = 0;
    int k;
#if 0
    uint8_t buf_fs[4]; 
    uint8_t buf_hs[4]; 
    buf_fs[0]=0;  
    buf_fs[1]=7;  
    buf_fs[2]=7;  
    buf_fs[3]=0;  
     
    buf_hs[0]=0;  
    buf_hs[1]=8;  
    buf_hs[2]=8;  
    buf_hs[3]=0;  
#endif
    i = 0x1000;
    //data = 0x22;
    stop = 0;
    while (i--);

    APP_VCP_FOPS.pIf_Init();
    //I2C_Config();
    /*!< At this stage the microcontroller clock setting is already configured,
     this is done through SystemInit() function which is called from startup
     file (startup_stm32fxxx_xx.s) before to branch to application main.
     To reconfigure the default setting of SystemInit() function, refer to
     system_stm32fxxx.c file
     */

    USBD_Init(&USB_OTG_FS_dev,
            USB_OTG_FS_CORE_ID,
            &USR_HID_desc,
            &USBD_HID_cb,
            &USR_FS_cb);
    #if 1//chg by huangcaihui 180621
    USBD_Init(&USB_OTG_HS_dev,
            USB_OTG_HS_CORE_ID,
            &USR_HID_desc,
            &USBD_HID_cb1,
            &USR_FS_cb);
    #else
    USBD_Init(&USB_OTG_HS_dev,
            USB_OTG_HS_CORE_ID,
            &USR_desc,
            &USBD_CDC_cb,
            &USR_HS_cb);
    #endif
    led_init();
    // system tick configer
    SysTick_Config(SystemCoreClock / 1000);
    
    //
#if 1
    while(1)
    {
			if (i++ == 0x20000)
			{
                if(my_usb_status){
                    if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_3))
                    {
                        printf("led on 11 \n");
				        LED_R_ON();
                    }
                }
                else{
                    if(!GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_3))
                    {
                        printf("led off 22 \n");
                        LED_R_OFF();
                    }
                }

                if(my_usb_status)
                {
                    if((usb_check ==1) && (k++ <10))
                    {
                        if(USBD_HID_RcvBuf[3] == 0)
                        {
                        USBD_HID_SendReport (&USB_OTG_FS_dev,   
                                                                 USBD_HID_RcvBuf,  
                                                                 4);  
                        printf("FS DataIN  buf[0]=%x,buf[1]=%x,buf[2]=%x,buf[3]=%x \n",USBD_HID_RcvBuf[0],USBD_HID_RcvBuf[1],USBD_HID_RcvBuf[2],USBD_HID_RcvBuf[3]);
                        }
                        else if(USBD_HID_RcvBuf[3] == 1)
                        {
                        USBD_HID_SendReport (&USB_OTG_HS_dev,   
                                                                 USBD_HID_RcvBuf,  
                                                                 4);  
                        printf("HS DataIN  buf[0]=%x,buf[1]=%x,buf[2]=%x,buf[3]=%x \n",USBD_HID_RcvBuf[0],USBD_HID_RcvBuf[1],USBD_HID_RcvBuf[2],USBD_HID_RcvBuf[3]);
                        }
                    }
                    
                    
                }
                else
                {
                    k = 0;
                    usb_check = 0;
                }
                //printf("DataSENT  buf[0]=%x,buf[1]=%x,buf[2]=%x,buf[3]=%x \n",USBD_HID_RcvBuf[0],USBD_HID_RcvBuf[1],USBD_HID_RcvBuf[2],USBD_HID_RcvBuf[3]);
                if(stop == 0)
                {
                    printf("device main() 1111111111111111111 \n");
                    //I2C_Test();
                    stop = 1;
                }
				i = 0;
			}
    }
#else
    /* Main loop */
    while (1)
    {
    }
#endif
}

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif


/*
 * serial port print callback
 */
int fputc(int ch, FILE *f)
{
    extern void EVAL_COM_TX_BYTE(char c);
    //UART_WriteData((uint8_t*)&ch,1);
//    APP_FOPS.pIf_DataRx((uint8_t*)&ch,1);
    EVAL_COM_TX_BYTE(ch);
    return ch;
}
/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
