/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define _Gornji_Granicnik_Pin GPIO_PIN_2
#define _Gornji_Granicnik_GPIO_Port GPIOC
#define _Donji_Granicnik_Pin GPIO_PIN_5
#define _Donji_Granicnik_GPIO_Port GPIOA
#define motor_Pin GPIO_PIN_6
#define motor_GPIO_Port GPIOA
#define motorA7_Pin GPIO_PIN_7
#define motorA7_GPIO_Port GPIOA
#define otvaranje_vrata_interruptom_Pin GPIO_PIN_12
#define otvaranje_vrata_interruptom_GPIO_Port GPIOB
#define otvaranje_vrata_interruptom_EXTI_IRQn EXTI15_10_IRQn
#define trig_Pin GPIO_PIN_7
#define trig_GPIO_Port GPIOC
#define echo_Pin GPIO_PIN_8
#define echo_GPIO_Port GPIOC
#define zatvaranje_vrata_interruptom_Pin GPIO_PIN_11
#define zatvaranje_vrata_interruptom_GPIO_Port GPIOA
#define zatvaranje_vrata_interruptom_EXTI_IRQn EXTI15_10_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
