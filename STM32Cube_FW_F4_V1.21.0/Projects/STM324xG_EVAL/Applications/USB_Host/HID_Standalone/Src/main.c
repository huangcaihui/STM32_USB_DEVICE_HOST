/**
  ******************************************************************************
  * @file    USB_Host/HID_Standalone/Src/main.c
  * @author  MCD Application Team
  * @brief   USB host HID Mouse and Keyboard demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBH_HandleTypeDef hUSBHost;
HID_ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);
static void HID_InitApplication(void);
extern int bClassStart;
extern int bReceiveData_Check;
GPIO_PinState bitstatus0;
GPIO_PinState bitstatus1;
GPIO_PinState bitstatus2;
GPIO_PinState bitstatus3;


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
UART_HandleTypeDef huart4;

//static void MX_GPIO_Init(void);
static void MX_UART4_Init(void);

/* UART4 init function */
static void MX_UART4_Init(void)
{

  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}



static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

#define LED_R_ON() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);//GPIOC->BSRRH = GPIO_PIN_3;
#define LED_R_OFF() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);//GPIOC->BSRRL = GPIO_PIN_3;


int led_on_ms = 200;
int led_off_ms = 800;

void Led_Handler(void) //
{

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

void led_init(void)
{  
    GPIO_InitTypeDef GPIO_init_l;//?????????  
    
    __HAL_RCC_GPIOC_CLK_ENABLE();  
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);   //??  
    GPIO_init_l.Pin=GPIO_PIN_3;//GPIO_Pin_3;  
    GPIO_init_l.Mode=GPIO_MODE_OUTPUT_PP;//GPIO_Mode_OUT;  
    GPIO_init_l.Pull = GPIO_NOPULL;
    GPIO_init_l.Speed=GPIO_SPEED_HIGH;//GPIO_Speed_100MHz;  
    //GPIO_init_l.GPIO_PuPd=GPIO_PuPd_NOPULL;    
    HAL_GPIO_Init(GPIOC,&GPIO_init_l);  
    
	LED_R_ON();
    
}

void MCU_ID_Init(void)
{
    GPIO_InitTypeDef GPIO_init_l;//?????????  
    
    __HAL_RCC_GPIOC_CLK_ENABLE();  
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);   //??  
    GPIO_init_l.Pin=GPIO_PIN_4 | GPIO_PIN_5;//GPIO_Pin_3;  
    GPIO_init_l.Mode=GPIO_MODE_INPUT;//GPIO_Mode_OUT;  
    GPIO_init_l.Pull = GPIO_NOPULL;
    GPIO_init_l.Speed=GPIO_SPEED_HIGH;//GPIO_Speed_100MHz;  
    HAL_GPIO_Init(GPIOC,&GPIO_init_l);  

    __HAL_RCC_GPIOB_CLK_ENABLE();  
    GPIO_init_l.Pin=GPIO_PIN_0;//GPIO_Pin_3;  
    GPIO_init_l.Mode=GPIO_MODE_INPUT;//GPIO_Mode_OUT;  
    GPIO_init_l.Pull = GPIO_NOPULL;
    GPIO_init_l.Speed=GPIO_SPEED_HIGH;//GPIO_Speed_100MHz;  
    HAL_GPIO_Init(GPIOB,&GPIO_init_l);  

    bitstatus0 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4);
    bitstatus1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5);
    bitstatus2 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
}

int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */

  int i,stop;//,k;
  int k;
  //int data;
  
  
#if 1
    uint8_t buf_fs[4]; 
    //uint8_t buf_hs[4]; 
    buf_fs[0]=0;  
    buf_fs[1]=0;  
    buf_fs[2]=0;  
    buf_fs[3]=0;      
#endif
  i = 0;
  k = 0;
  stop = 0;
  //data = 0x02;
  HAL_Init();
  
  
  
  /* Configure the system clock to 168 MHz */
  SystemClock_Config();

  MX_GPIO_Init();
  MX_UART4_Init();

  led_init();
  MCU_ID_Init();
  /* Init HID Application */
//  HID_InitApplication();//TT.Henry 180710
  
  /* Init Host Library */
  USBH_Init(&hUSBHost, USBH_UserProcess, 0);
  
  /* Add Supported Class */
  USBH_RegisterClass(&hUSBHost, USBH_HID_CLASS);
  
  /* Start Host Process */
  USBH_Start(&hUSBHost);
  
  /* Run Application (Blocking mode) */
  while (1)
  {
    /* USB Host Background task */
    USBH_Process(&hUSBHost); 
     
    /* HID Menu Process */
    HID_MenuProcess();
    if(k++ == 0x100)
    {
        k=0;
        
        buf_fs[0]=bitstatus0;  
        buf_fs[1]=bitstatus1;  
        buf_fs[2]=bitstatus2;  
        buf_fs[3]=0;   
        if(bClassStart)
        {
            if(stop == 0)
            {
                stop = 1;

                printf("bClassStart !!!!!!!!!!11 \n");
                printf("buf_fs[0]=%x,buf_fs[1]=%x,buf_fs[2]=%x,buf_fs[3]=%x \n",buf_fs[0],buf_fs[1],buf_fs[2],buf_fs[3]);
                USBH_InterruptSendData (&hUSBHost,buf_fs,4,3); 
            }
        }
        else
        {
            stop = 0;
            bReceiveData_Check = 0;
        }   

        #if 1
            if(bClassStart )//&& bReceiveData_Check)
                Led_Handler();
            else
                LED_R_OFF();
        #else
            LED_R_ON();
        #endif
    }
    if (i++ == 0x20000)
	{
        i = 0;
        
        
        //USBH_InterruptSendData (&hUSBHost,buf_fs,4,3);
        //USBH_BulkSendData(&hUSBHost,buf_fs,4,1);
        
        
        
        #if 0        
        if(stop == 0)
        {
            stop = 1;
            for(k = 0; k < 10 ;k++)
            {
                //printf(" 111111111111 host \n");
                HAL_UART_Transmit(&huart4, (uint8_t *)&data, 1, 0xFFFF); 
            }
        }
       
        #endif
    }
  }
}

/**
  * @brief  HID application Init
  * @param  None
  * @retval None
  */
static void HID_InitApplication(void)
{
  /* Configure Key Button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);                
  
  /* Configure Joystick in EXTI mode */
  BSP_JOY_Init(JOY_MODE_EXTI);

  /* Initialize the LCD */
  BSP_LCD_Init();
  
  /* Init the LCD Log module */
  LCD_LOG_Init();
  
#ifdef USE_USB_HS 
  LCD_LOG_SetHeader((uint8_t *)" USB OTG HS HID Host");
#else
  LCD_LOG_SetHeader((uint8_t *)" USB OTG FS HID Host");
#endif
  
  
  LCD_UsrLog("USB Host library started.\n"); 
  
  /* Start HID Interface */
 // HID_MenuInit();//TT.Henry 180710
}

/**
  * @brief  User Process
  * @param  phost: Host Handle
  * @param  id: Host Library user message ID
  * @retval None
  */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
  switch(id)
  { 
  case HOST_USER_SELECT_CONFIGURATION:
    break;
    
  case HOST_USER_DISCONNECTION:
    Appli_state = APPLICATION_DISCONNECT;
    break;
    
  case HOST_USER_CLASS_ACTIVE:
    Appli_state = APPLICATION_READY;
    break;
    
  case HOST_USER_CONNECTION:
    Appli_state = APPLICATION_START;
    break;
    
  default:
    break; 
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
#if 0
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

#else
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;//25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}
#endif

/* USER CODE END 4 */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart4, (uint8_t *)&ch, 1, 0xFFFF); 

  return ch;
}

void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
