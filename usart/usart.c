/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

uint8_t uart2_rxbuf[RX_BUFFER_MXSIZE];		//指令缓存区	
uint8_t uart2_RX_rxbuf[RX_BUFFER_MXSIZE];	//usart数据缓冲区
uint8_t uart2_rx_dma_status=0;              //接收状态
uint16_t dma_recv_len=0;					//接收长度

/* 重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART2->SR & 0X40) == 0);               /* 等待上一个字符发送完成 */
    USART2->DR = (uint8_t)ch;                       /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}


/* USER CODE END 0 */

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
    __HAL_UART_CLEAR_IDLEFLAG(&huart2);           //打开空闲中断
    __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
  
    HAL_UART_Receive_DMA(&huart2,uart2_RX_rxbuf,RX_BUFFER_MXSIZE);
  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Channel6;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    UART_RxIdleCallback
  * @param   huart: 输入
**			 hdma_usart_rx: 输入
  * @brief   中断回调函数，此函数需放入USART2_IRQHandler()函数中
  * @Data    2023-10-12
 **/
/* -------------------------------- end -------------------------------- */
void UART_RxIdleCallback(UART_HandleTypeDef *huart,DMA_HandleTypeDef *hdma_usart_rx){

    static uint16_t shengyu_data=0;
    static uint16_t last_shengyu_data=RX_BUFFER_MXSIZE;
    uint16_t index = 0;
    if( huart->Instance == USART2){
        
        if(__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE)){
            __HAL_UART_CLEAR_IDLEFLAG(huart);
         
            shengyu_data = __HAL_DMA_GET_COUNTER(hdma_usart_rx);                                        //本次剩余空间

            if( shengyu_data < last_shengyu_data ){    
                dma_recv_len = last_shengyu_data - shengyu_data;                                        //上一次剩余空间-本次剩余空间，本次接收到数据数
                index = (RX_BUFFER_MXSIZE-__HAL_DMA_GET_COUNTER(hdma_usart_rx))-dma_recv_len;           //本次接收的数据的起始位置
                memcpy(uart2_rxbuf,uart2_RX_rxbuf+index,dma_recv_len);                                  //将接收到的数据存入BUFF            
            }else if(shengyu_data > last_shengyu_data){                                                //进入了循环，本次剩余大于上次剩余
                
                dma_recv_len = last_shengyu_data+(RX_BUFFER_MXSIZE-shengyu_data);
                memcpy(uart2_rxbuf,uart2_RX_rxbuf+(RX_BUFFER_MXSIZE-last_shengyu_data),last_shengyu_data);
                memcpy(uart2_rxbuf+last_shengyu_data,uart2_RX_rxbuf,dma_recv_len-last_shengyu_data);
            }
            
            last_shengyu_data = shengyu_data;                       //记录本次剩余的空间数
            uart2_rx_dma_status=1;
            HAL_UART_Receive_DMA(&huart2,uart2_RX_rxbuf,RX_BUFFER_MXSIZE);
        }
    }
}
/* USER CODE END 1 */
