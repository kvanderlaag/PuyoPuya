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

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef UartHandle;
uint8_t aTxBuffer[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
uint8_t aRxBuffer[12] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_LedConfig(void);


/**
  * @brief  应用程序入口函数.
  * @param  无
  * @retval int
  */
/*
int main(void)
{
  // 初始化所有外设，Flash接口，SysTick
  HAL_Init();                                 
  
  // 初始化LED
  APP_LedConfig();

  while (1)
  {
    // 延时250ms
    HAL_Delay(250);                            
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1); 
  }
}
*/
int main(void)
{
  /* systick初始化 */
	APP_LedConfig();
	
  HAL_Init();
  
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
  
  /*通过POLLING方式发送数据*/
  //if (HAL_UART_Transmit(&UartHandle, (uint8_t *)aTxBuffer, 12, 5000) != HAL_OK)
  //{
	//	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1); 
  //  APP_ErrorHandler();
  //}

  while (1)
  {
  }
}

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
  if (HAL_UART_Receive_IT(UartHandle, (uint8_t *)aRxBuffer, 12) != HAL_OK)
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
  if (HAL_UART_Transmit_IT(UartHandle, (uint8_t *)aRxBuffer, 12) != HAL_OK)
  {
    APP_ErrorHandler();
  }

}

/**
  * @brief  初始化LED
  * @param  无
  * @retval 无
  */
static void APP_LedConfig(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  __HAL_RCC_GPIOB_CLK_ENABLE();                          /* GPIOB时钟使能 */

  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
  GPIO_InitStruct.Pull = GPIO_PULLUP;                    /* 使能上拉 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;          /* GPIO速度 */

  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);                /* GPIO初始化 */
}


/**
  * @brief  错误执行函数
  * @param  无
  * @retval 无
  */
void APP_ErrorHandler(void)
{
  /* 无限循环 */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
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
