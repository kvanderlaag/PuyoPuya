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
#include "py32f003xx_ll_Start_Kit.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t aTxBuffer[] = "UART Test";
uint8_t aRxBuffer[30];
__IO ITStatus UartReady = RESET;

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_ConfigUsart(USART_TypeDef *USARTx);
static void APP_ConfigDma(USART_TypeDef *USARTx);
static void APP_UsartTransmit_DMA(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size);
static void APP_UsartReceive_DMA(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size);

/**
  * @brief  应用程序入口函数.
  * @param  无
  * @retval int
  */
int main(void)
{
  /* 配置系统时钟 */
  APP_SystemClockConfig();
  
  /* USART配置 */
  APP_ConfigUsart(USART1);
  
  /* 发送"UART Test",并等待发送完成 */
  APP_UsartTransmit_DMA(USART1, (uint8_t*)aTxBuffer, sizeof(aTxBuffer)-1);
  while (UartReady != SET)
  {
  }
  UartReady = RESET;

  while (1)
  {
    /* 接收数据 */
    APP_UsartReceive_DMA(USART1, (uint8_t *)aRxBuffer, 12);
    while (UartReady != SET)
    {
    }
    UartReady = RESET;
    
    /* 发送数据 */
    APP_UsartTransmit_DMA(USART1, (uint8_t*)aRxBuffer, 12);
    while (UartReady != SET)
    {
    }
    UartReady = RESET;
  }
}

/**
  * @brief  系统时钟配置函数
  * @param  无
  * @retval 无
  */
static void APP_SystemClockConfig(void)
{
  /* 使能HSI */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  /* 设置 AHB 分频*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* 配置HSISYS作为系统时钟源 */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS)
  {
  }

  /* 设置 APB1 分频*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_Init1msTick(8000000);

  /* 更新系统时钟全局变量SystemCoreClock(也可以通过调用SystemCoreClockUpdate函数更新) */
  LL_SetSystemCoreClock(8000000);
}

/**
  * @brief  USART配置函数
  * @param  USARTx：USART模块，可以是USART1、USART2
  * @retval 无
  */
static void APP_ConfigUsart(USART_TypeDef *USARTx)
{
  /*使能时钟、初始化引脚、使能NVIC中断*/
  if (USARTx == USART1) 
  {
    /*使能GPIOA时钟*/
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    /*使能USART1时钟*/
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);
        
    /*GPIOA配置*/
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    /*选择2号引脚*/
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    /*选择复用模式*/
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    /*选择输出速度*/
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    /*选择输出模式*/
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    /*选择上拉*/
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    /*复用为USART1功能*/
    GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;
    /*GPIOA初始化*/
    LL_GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    /*选择3号引脚*/
    GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
    /*复用为USART1功能*/
    GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;
    /*GPIOA初始化*/
    LL_GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    /*设置USART1中断优先级*/
    NVIC_SetPriority(USART1_IRQn,0);
    /*使能USART1中断*/
    NVIC_EnableIRQ(USART1_IRQn);
  }
  else if (USARTx == USART2)
  {
    /*使能GPIOA时钟*/
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    /*使能USART1时钟*/
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
    
    /*GPIOA配置*/
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    /*选择2号引脚*/
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    /*选择复用模式*/
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    /*选择输出速度*/
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    /*选择输出模式*/
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    /*选择上拉*/
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    /*复用为USART2功能*/
    GPIO_InitStruct.Alternate = LL_GPIO_AF4_USART2;
    /*GPIOA初始化*/
    LL_GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    /*选择3号引脚*/
    GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
    /*复用为USART2功能*/
    GPIO_InitStruct.Alternate = LL_GPIO_AF4_USART2;
    /*GPIOA初始化*/
    LL_GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    /*设置USART1中断优先级*/
    NVIC_SetPriority(USART2_IRQn,0);
    /*使能USART1中断*/
    NVIC_EnableIRQ(USART2_IRQn);
  }

  /*配置DMA*/
  APP_ConfigDma(USARTx);

  /*配置USART功能*/
  LL_USART_InitTypeDef USART_InitStruct;
  /*设置波特率*/
  USART_InitStruct.BaudRate = 9600;
  /*设置数据长度*/
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  /*停止位*/
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  /*设置校验位*/
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  /*USART初始化*/
  LL_USART_Init(USARTx, &USART_InitStruct);
  
  /*配置为全双工异步模式*/
  LL_USART_ConfigAsyncMode(USARTx);
  
  /*使能UART模块*/
  LL_USART_Enable(USARTx);
}

/**
  * @brief  DMA配置函数
  * @param  USARTx：USART模块，可以是USART1、USART2
  * @retval 无
  */
static void APP_ConfigDma(USART_TypeDef *USARTx)
{
  /*使能DMA时钟、syscfg的时钟*/
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);

  
  /*利用DMA通道LL_DMA_CHANNEL_1进行发送*/
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_MEMORY_TO_PERIPH | \
                      LL_DMA_MODE_NORMAL                   | \
                      LL_DMA_PERIPH_NOINCREMENT  | \
                      LL_DMA_MEMORY_INCREMENT  | \
                      LL_DMA_PDATAALIGN_BYTE | \
                      LL_DMA_MDATAALIGN_BYTE | \
                      LL_DMA_PRIORITY_LOW);
  
 
 /*利用DMA通道LL_DMA_CHANNEL_2进行接收*/
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_PERIPH_TO_MEMORY | \
                      LL_DMA_MODE_NORMAL                   | \
                      LL_DMA_PERIPH_NOINCREMENT  | \
                      LL_DMA_MEMORY_INCREMENT  | \
                      LL_DMA_PDATAALIGN_BYTE | \
                      LL_DMA_MDATAALIGN_BYTE | \
                      LL_DMA_PRIORITY_LOW);

    /* 配置通道USART1、USART2的DMA请求映像*/
  if (USARTx ==  USART1)
  {
    /* USART1_TX对应通道LL_DMA_CHANNEL_1，USART1_RX对应通道LL_DMA_CHANNEL_2*/
    SET_BIT(SYSCFG->CFGR3, 0x0605);
  }
  else
  {
    /*USART2_TX对应通道LL_DMA_CHANNEL_1，USART2_RX对应通道LL_DMA_CHANNEL_2*/
    SET_BIT(SYSCFG->CFGR3, 0x0807); 
  }    
  
  /*设置中断优先级*/
  NVIC_SetPriority(DMA1_Channel1_IRQn, 1);
  /*使能中断*/
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  
  /*设置中断优先级*/
  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1);
  /*使能中断*/
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

/**
  * @brief  USART发送函数
  * @param  USARTx：USART模块，可以是USART1、USART2
  * @param  pData：发送缓冲区
  * @param  Size：发送缓冲区大小
  * @retval 无
  */
static void APP_UsartTransmit_DMA(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size)
{  
  /*配置DMA通道1*/
  LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
  LL_DMA_ClearFlag_GI1(DMA1);
  uint32_t *temp = (uint32_t *)&pData;
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, *(uint32_t *)temp);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&(USARTx->DR));
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, Size);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
  
  LL_USART_ClearFlag_TC(USARTx);

  /*使能USART_DMA通道*/
  LL_USART_EnableDMAReq_TX(USARTx);
}

/**
  * @brief  USART接收函数
  * @param  USARTx：USART模块，可以是USART1、USART2
  * @param  pData：接收缓冲区
  * @param  Size：接收缓冲区大小
  * @retval 无
  */
static void APP_UsartReceive_DMA(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size)
{
  /*配置DMA通道2*/
  LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
  LL_DMA_ClearFlag_GI2(DMA1);
  uint32_t *temp = (uint32_t *)&pData;
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, *(uint32_t *)temp);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)&USARTx->DR);
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, Size);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_2);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
  
  LL_USART_ClearFlag_ORE(USARTx);
  
  /*使能USART_DMA通道*/
  LL_USART_EnableDMAReq_RX(USARTx);
}

/**
  * @brief  USART中断处理函数
  * @param  USARTx：USART模块，可以是USART1、USART2
  * @retval 无
  */
void APP_UsartIRQCallback(USART_TypeDef *USARTx)
{
  /*发送完成*/
  if ((LL_USART_IsActiveFlag_TC(USARTx) != RESET) && (LL_USART_IsEnabledIT_TC(USARTx) != RESET))
  {
    LL_USART_DisableIT_TC(USARTx);
    UartReady = SET;
  
    return;
  }
}

/**
  * @brief  利用DMA进行发送的中断处理函数
  * @param  无
  * @retval 无
  */
void APP_DmaChannel1IRQCallback(void)
{
  if(LL_DMA_IsActiveFlag_TC1(DMA1) == 1)
  {
    LL_DMA_ClearFlag_GI1(DMA1);
    LL_USART_DisableDMAReq_TX(USART1);
    LL_USART_EnableIT_TC(USART1);
  }
}

/**
  * @brief  利用DMA进行接收的中断处理函数
  * @param  无
  * @retval 无
  */
void APP_DmaChannel2_3_IRQCallback(void)
{
  if(LL_DMA_IsActiveFlag_TC2(DMA1) == 1)
  {
    LL_DMA_ClearFlag_GI2(DMA1);
    LL_USART_DisableDMAReq_RX(USART1);
    UartReady = SET;
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
