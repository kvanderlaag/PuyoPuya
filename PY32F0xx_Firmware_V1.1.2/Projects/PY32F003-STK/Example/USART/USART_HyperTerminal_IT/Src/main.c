/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef UartHandle;
uint8_t aTxBuffer[33];
uint8_t aRxBuffer[12] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SetSysClock(uint32_t SYSCLKSource);
static void APP_SystemClockConfig(void);

/**
  * @brief  应用程序入口函数.
  * @retval int
  */
int main(void)
{
	/* systick初始化 */
  HAL_Init();
	
	GPIO_InitTypeDef  GPIO_InitStruct;

  __HAL_RCC_GPIOB_CLK_ENABLE();                          /* GPIOB时钟使能 */

  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
  GPIO_InitStruct.Pull = GPIO_PULLUP;                    /* 使能上拉 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;          /* GPIO速度 */

  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);                /* GPIO初始化 */
	
  //HAL_SuspendTick();
  APP_SystemClockConfig();
	APP_SetSysClock(RCC_SYSCLKSOURCE_HSI);
	//HAL_ResumeTick();
  
  /* USART初始化 */
  UartHandle.Instance          = USART1;
  UartHandle.Init.BaudRate     = 9600;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  if (HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    APP_ErrorHandler();
  }

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	
	uint32_t clockFreq = HAL_RCC_GetSysClockFreq() / 1000000;
	// __HAL_RCC_GET_SYSCLK_SOURCE()
	sprintf((char*)&aTxBuffer, "PuyoPuya v.0 - %uMHz", clockFreq);
  
  /*通过中断方式发送数据*/
  if (HAL_UART_Transmit_IT(&UartHandle, (uint8_t *)aTxBuffer, strlen((char*)aTxBuffer)) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  while (1)
  {
  }
}

/**
  * @brief  USART错误回调执行函数
  * @param  huart：USART句柄
  * @retval 无
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  printf("Uart Error, ErrorCode = %d\r\n", huart->ErrorCode);
}

/**
  * @brief  USART发送回调执行函数
  * @param  huart：USART句柄
  * @retval 无
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  if (HAL_UART_Receive_IT(UartHandle, (uint8_t *)aRxBuffer, 1) != HAL_OK)
  {
    APP_ErrorHandler();
  }
}

/**
  * @brief  USART接收回调执行函数
  * @param  huart：USART句柄
  * @retval 无
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /*通过中断方式接收数据*/
  if (HAL_UART_Transmit_IT(UartHandle, (uint8_t *)aRxBuffer, 1) != HAL_OK)
  {
    APP_ErrorHandler();
  }

}

/**
  * @brief   系统时钟配置函数
  * @param   无
  * @retval  无
  */
static void APP_SystemClockConfig(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* 配置时钟源HSE/HSI/LSE/LSI */
  RCC_OscInitStruct.OscillatorType = /* RCC_OSCILLATORTYPE_HSE | */ RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;                                                    /* 开启HSI */
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                                                    /* 不分频 */
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_24MHz;                            /* 配置HSI输出时钟为8MHz */
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;                                                    /* 开启HSE */
  RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;                                               /* HSE工作频率范围16M~32M */
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;                                                    /* 开启LSI */

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)                                        /* 初始化RCC振荡器 */
  {
    APP_ErrorHandler();
  }

  /* 初始化CPU,AHB,APB总线时钟 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1; /* RCC系统时钟类型 */
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;                                         /* SYSCLK的源选择为LSI */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;                                             /* APH时钟不分频 */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;                                              /* APB时钟不分频 */

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)                        /* 初始化RCC系统时钟(FLASH_LATENCY_0=24M以下;FLASH_LATENCY_1=48M) */
  {
    APP_ErrorHandler();
  }
}

/**
  * @brief   设置系统时钟
  * @param   SYSCLKSource：系统时钟源
  *            @arg RCC_SYSCLKSOURCE_LSI: LSI作为系统时钟源
  *            @arg RCC_SYSCLKSOURCE_LSE: LSE作为系统时钟源
  *            @arg RCC_SYSCLKSOURCE_HSE: HSE作为系统时钟源
  *            @arg RCC_SYSCLKSOURCE_HSI: HSI作为系统时钟源
  *            @arg RCC_SYSCLKSOURCE_PLLCLK: PLL作为系统时钟源
  * @retval  无
  */
static void APP_SetSysClock(uint32_t SYSCLKSource)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* RCC系统时钟类型 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = SYSCLKSource;                            /* 系统时钟源选择 */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;                        /* APH时钟不分频 */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;                         /* APB时钟2分频 */

  /* 初始化RCC系统时钟(FLASH_LATENCY_0=24M以下;FLASH_LATENCY_1=48M) */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    APP_ErrorHandler();
  }
}


/**
  * @brief  错误执行函数
  * @param  无
  * @retval 无
  */
void APP_ErrorHandler(void)
{
  /* 无限循环 */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  输出产生断言错误的源文件名及行号
  * @param  file：源文件名指针
  * @param  line：发生断言错误的行号
  * @retval 无
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* 用户可以根据需要添加自己的打印信息,
     例如: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* 无限循环 */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
