/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

extern ADC_HandleTypeDef hadc2;

/* USER CODE BEGIN Private defines */
typedef struct _ADC_gather
{
  ADC_HandleTypeDef * adcx_ptr;
  uint32_t ADC_Channel;
}adc_gather;
extern adc_gather  phase1out_u, phase1out_i, phase2out_u, phase2out_i, 
                   phase3out_u, phase3out_i, phase1in_u , phase1in_i , 
                   phase2in_u , phase2in_i , phase3in_u , phase3in_i , 
                   overall_out, overall_in;

/* USER CODE END Private defines */

void MX_ADC1_Init(void);
void MX_ADC2_Init(void);

/* USER CODE BEGIN Prototypes */
float Get_Adc_Average(ADC_HandleTypeDef * hadc_ptr, uint32_t ch, uint8_t times);
float Get_Adc_MidValue(ADC_HandleTypeDef * hadc_ptr, uint32_t ch);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

