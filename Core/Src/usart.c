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
// ����1�жϷ������
// ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
uint8_t USART_RX_BUF[USART_REC_LEN]; // ���ջ���,���USART_REC_LEN���ֽ�.
// ����״̬
// bit15��	������ɱ�־
// bit14��	���յ�0x0d
// bit13~0��	���յ�����Ч�ֽ���Ŀ
uint16_t USART_RX_STA = 0; // ����״̬���

uint8_t aRxBuffer[RXBUFFERSIZE]; // HAL��ʹ�õĴ��ڽ��ջ���

// ���ջص�����
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == LPUART1) // �����P����1
  {
    if ((USART_RX_STA & 0x8000) == 0) // ����δ���
    {
      if (USART_RX_STA & 0x4000) // ���յ���0x0d
      {
        if (aRxBuffer[0] != 0x0a)
          USART_RX_STA = 0; // ���մ���,���¿�ʼ
        else
          USART_RX_STA |= 0x8000; // ���������
      }
      else // ��û�յ�0X0D
      {
        if (aRxBuffer[0] == 0x0d)
          USART_RX_STA |= 0x4000;
        else
        {
          USART_RX_BUF[USART_RX_STA & 0X3FFF] = aRxBuffer[0];
          USART_RX_STA++;
          if (USART_RX_STA > (USART_REC_LEN - 1))
            USART_RX_STA = 0; // �������ݴ���,���¿�ʼ����
        }
      }
    }
    HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)aRxBuffer, RXBUFFERSIZE); // ������һ�ν����ж�
  }
}
/* USER CODE END 0 */

UART_HandleTypeDef hlpuart1;

/* LPUART1 init function */

void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */
  // �����Ҫ���� HAL_UART_Init() ֮��
  // �ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
  HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)aRxBuffer, RXBUFFERSIZE);
  /* USER CODE END LPUART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(uartHandle->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspInit 0 */

  /* USER CODE END LPUART1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
    PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* LPUART1 clock enable */
    __HAL_RCC_LPUART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**LPUART1 GPIO Configuration
    PA2     ------> LPUART1_TX
    PA3     ------> LPUART1_RX
    */
    GPIO_InitStruct.Pin = LPUART1_TX_Pin|LPUART1_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_LPUART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* LPUART1 interrupt Init */
    HAL_NVIC_SetPriority(LPUART1_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(LPUART1_IRQn);
  /* USER CODE BEGIN LPUART1_MspInit 1 */

  /* USER CODE END LPUART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspDeInit 0 */

  /* USER CODE END LPUART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LPUART1_CLK_DISABLE();

    /**LPUART1 GPIO Configuration
    PA2     ------> LPUART1_TX
    PA3     ------> LPUART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, LPUART1_TX_Pin|LPUART1_RX_Pin);

    /* LPUART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(LPUART1_IRQn);
  /* USER CODE BEGIN LPUART1_MspDeInit 1 */

  /* USER CODE END LPUART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
int fputc(int ch, FILE *f)
{
  // �����ĸ����ڿ��Ը���huart1Ϊ��������
  HAL_UART_Transmit(&hlpuart1, (uint8_t *)&ch, 1, 0xff);
  return ch;
}
/* USER CODE END 1 */
