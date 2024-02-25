/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "hrtim.h"
#include "usart.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "buck-boost.h"
#include "usmart.h"
#include "display.h"
#include "display_test.h"
#include "lvgl.h"
#include "display_conf.h"
#include "lv_port_disp_template.h"
#include "showAD.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float ADval;
//float overall_out_u_fact, overall_in_u;  // 总输入输出电压(ADC实测值)
//float overall_out_u_target;  // 目标输出电压
RunMode_t runMode = noMode;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void changeModeFunc(void)
{
  if(v_out_target + delta_v < v_in)
  {
    //printf("enter buck \r\n");
    // 同步Buck模式
    switch(runMode)
    {
      case noMode:break;
      case SynchronizeBuckMode: /*synchronization_buck_mode(exitMode);*/ break;
      case IncreaseBuckboostMode: increase_buckboost_mode(exitMode); break;
      case DecreaseBuckboostMode: decrease_buckboost_mode(exitMode); break;
      case SynchronizeBoostMode: synchronization_boost_mode(exitMode); break;
    }

    if(runMode != SynchronizeBuckMode)
      synchronization_buck_mode(enterMode);

    runMode = SynchronizeBuckMode;
    synchronization_buck_mode(executeMode);
  }
  else if(v_out_target <= v_in && v_in <= v_out_target + delta_v)
  {
    //printf("enter reduce buck-boost\r\n");
    // 降压Buck Boost模式
    switch(runMode)
    {
      case noMode:break;
      case SynchronizeBuckMode: synchronization_buck_mode(exitMode); break;
      case IncreaseBuckboostMode: increase_buckboost_mode(exitMode); break;
      case DecreaseBuckboostMode: /*decrease_buckboost_mode(exitMode);*/ break;
      case SynchronizeBoostMode: synchronization_boost_mode(exitMode); break;
    }

    if(runMode != DecreaseBuckboostMode)
      decrease_buckboost_mode(enterMode);

    runMode = DecreaseBuckboostMode;
    decrease_buckboost_mode(executeMode);  // execute mode
  }
  else if(v_in < v_out_target && v_out_target <= v_in + delta_v)
  {
    // 升压Buck Boost模式
    //printf("enter incrase buck-boost\r\n");
    switch(runMode)
    {
      case noMode:break;
      case SynchronizeBuckMode: synchronization_buck_mode(exitMode); break;
      case IncreaseBuckboostMode: /*increase_buckboost_mode(exitMode);*/ break;
      case DecreaseBuckboostMode: decrease_buckboost_mode(exitMode); break;
      case SynchronizeBoostMode: synchronization_boost_mode(exitMode); break;
    }

    if(runMode != IncreaseBuckboostMode)
      increase_buckboost_mode(enterMode);

    runMode = IncreaseBuckboostMode;
    increase_buckboost_mode(executeMode);
  }
  else if(v_in + delta_v < v_out_target)
  {
    // 同步Boost模式
//    printf("enter boost \r\n");
    switch(runMode)
    {
      case noMode:break;
      case SynchronizeBuckMode: synchronization_buck_mode(exitMode); break;
      case IncreaseBuckboostMode: increase_buckboost_mode(exitMode); break;
      case DecreaseBuckboostMode: decrease_buckboost_mode(exitMode); break;
      case SynchronizeBoostMode: /*synchronization_boost_mode(exitMode);*/ break;
    }

    if(runMode != SynchronizeBoostMode)
      synchronization_boost_mode(enterMode);

    runMode = SynchronizeBoostMode;
    synchronization_boost_mode(executeMode);
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_HRTIM1_Init();
  MX_LPUART1_UART_Init();
  MX_SPI3_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim3);
  lv_init();
	lv_port_disp_init();
	AD_label_event_init();
  
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_TIMERID_MASTER);  // 开启主时钟同步
  HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_MASTER);
  
  // HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TA2);
  // HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_A);
  // HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB1 | HRTIM_OUTPUT_TB2);
  // HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_B);
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
  HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
  HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2);
  HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_E);
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2);
  HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_F);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x0, HRTIM_COMPAREUNIT_1, 0);  // 0x0 to 0x5 for timers A to F
  // __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x1, HRTIM_COMPAREUNIT_1, 0);  // PWM set zero
  __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x2, HRTIM_COMPAREUNIT_1, 0);
  __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x3, HRTIM_COMPAREUNIT_1, 0);
  __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x4, HRTIM_COMPAREUNIT_1, 0);
  __HAL_HRTIM_SETCOMPARE(&hhrtim1, 0x5, HRTIM_COMPAREUNIT_1, 0);
  
  usmart_dev.init(0);		//初始化USMART
  config_param_init();  // 预设参数初始化
  v_out_target = 1.8;  // 这个考虑定时器中断进行修改
  while (1)
  {
    change_AD_data(0, 11.0);
    // v_in 是否需要做成动态采集    
    v_in = Get_Adc_MidValue(overall_in.adcx_ptr, overall_in.ADC_Channel);
    printf("v_in: %f\r\n", v_in);
    
    if(v_in > 0.9f)  // 必须要有一定的输入电压
      changeModeFunc();
    else
      printf("v_in is too low to run\r\n");
    
    HAL_Delay(120);  // TODO 时间暂时调大，便于调试

    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    
    usmart_dev.scan(); // 执行usmart扫描
    lv_timer_handler();
    
    printf("draw: %f,%f,%f\r\n", v_out_target, Get_Adc_MidValue(overall_out.adcx_ptr, overall_out.ADC_Channel),
                                    Get_Adc_MidValue(overall_in.adcx_ptr, overall_in.ADC_Channel)
                                    );
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
