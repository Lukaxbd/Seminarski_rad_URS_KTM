/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_LEN 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

#define TRIG_PIN GPIO_PIN_7
#define TRIG_PORT GPIOC
#define ECHO_PIN GPIO_PIN_8
#define ECHO_PORT GPIOC

uint32_t pMillis;
uint32_t Value1 = 0;
uint32_t Value2 = 0;
uint16_t Distance  = 0;
uint32_t Difference;
uint8_t flag_vrata = 0;
uint8_t flag_btn = 0;
uint8_t flag_aut = 0;
uint8_t stanje_vozila = 0;
uint32_t startTime = 0;
uint8_t flag_stanje = 0;

uint8_t RX_BUFFER[BUFFER_LEN] = {0};
char msg[40];


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void delay (uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim4, 0);
	while (__HAL_TIM_GET_COUNTER (&htim4) < time);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_12)
	{
		flag_stanje = 1;
	}

	if (GPIO_Pin == GPIO_PIN_11)
	{
		flag_btn = 2;
	}
}

void otvaranje_vrata(void)
{

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 1);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 300);
		HAL_Delay(50);
		for(int i=150; i<=600; i=i+5)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, i);
			HAL_Delay(10);
		}
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 600);
		HAL_Delay(100);
		flag_btn = 3;
}


void zatvaranje_vrata (void)
{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 300);
		HAL_Delay(50);
		for(int i=150; i<=600; i=i+5)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, i);
			HAL_Delay(10);
		}
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 600);
		HAL_Delay(100);
		flag_btn = 4;
}

void ultrazvucni(void)
{
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
	delay(10);
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

	pMillis = HAL_GetTick();
	//wait for echo pin to go high
	while (!(HAL_GPIO_ReadPin (ECHO_PORT, ECHO_PIN)) && pMillis + 10 >  HAL_GetTick());
	Value1 = __HAL_TIM_GET_COUNTER (&htim2);

	pMillis = HAL_GetTick();
	// wait for the echo pin to go low
	while ((HAL_GPIO_ReadPin (ECHO_PORT, ECHO_PIN)) && pMillis + 50 > HAL_GetTick());
	Value2 = __HAL_TIM_GET_COUNTER (&htim2);

	if (Value2 > Value1)
			{
				Difference = Value2-Value1;
			}

		else if (Value1 > Value2)
			{
				Difference = (0xffff - Value1) + Value2;
			}

	Distance = ((float)Difference * 0.034)/2;
	HAL_Delay(50);
}


void buzzer(void) {

        if (Distance <= 35 && Distance > 25 && flag_vrata == 1)
        {
            __HAL_TIM_SET_AUTORELOAD(&htim8, 2000-1); //500 Hz
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1000-1); //50% DutyCycle
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
        }
        else if (Distance <= 25 && Distance > 15 && flag_vrata == 1)
        {
            __HAL_TIM_SET_AUTORELOAD(&htim8, 1000-1); //1 kHz
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 700-1); //70% DutyCycle
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
        }
        else if (Distance <= 15 && flag_vrata == 1)
        {
            __HAL_TIM_SET_AUTORELOAD(&htim8, 500-1); //2 kHz
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 450-1); //90% DutyCycle
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
        }
        else
        {
            HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);
        }
}

void stanje()
{
    ultrazvucni();

            if (Distance < 20)
            {
                stanje_vozila = 1; // Vozilo je parkirano
            }
            else if (Distance > 50)
            {
                stanje_vozila = 2; // Vozilo je izvan garaže
            }
        flag_stanje=0;
        HAL_Delay(10);
        flag_btn=1;
}

void otvaranje_bluetooth(){
	if(RX_BUFFER[0] == '0')
	{
		flag_stanje = 1;
	}
	else if(RX_BUFFER[0] == '1')
	{
		flag_btn = 2;
	}
	else if (RX_BUFFER[0] == '2')
	{
		flag_aut = 1;
		flag_stanje = 1;
	}
}

void ispis_udaljenosti(){
	if(Distance <= 35 && Distance > 25 && flag_vrata==1)
	{
	  sprintf(msg, "Udaljenost velika:%dcm\r\n", Distance);
	  HAL_UART_Transmit_IT(&huart1, (uint8_t *)msg, strlen(msg));
	}
	else if(Distance <= 25 && Distance > 15 && flag_vrata==1)
	{
      sprintf(msg, "Udaljenost srednja:%dcm\r\n", Distance);
      HAL_UART_Transmit_IT(&huart1, (uint8_t *)msg, strlen(msg));
	}
	else if(Distance <= 15 && flag_vrata==1)
	{
	  sprintf(msg, "Udaljenost mala:%dcm\r\n", Distance);
	  HAL_UART_Transmit_IT(&huart1, (uint8_t *)msg, strlen(msg));
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if(huart->Instance == huart1.Instance){
      otvaranje_bluetooth();
      HAL_UART_Receive_IT(&huart1, RX_BUFFER, BUFFER_LEN);
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM8_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_Base_Start(&htim2);
  HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, 0);
  HAL_TIM_Base_Start(&htim4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_UART_Receive_IT(&huart1, RX_BUFFER, BUFFER_LEN);

  while (1)
  {
	  if(flag_stanje == 1)
	  {
		  stanje();
	  }

	  switch(flag_btn)
	  {
	  case(1):

		  if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2)==0 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)==1)
		  	  	{
		  			  otvaranje_vrata();
		  			  flag_vrata = 1;
		  	  	}
		  else
		  {
			  flag_btn = 0;
		  }
	  	  break;

	  case(2):

		  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)==0 && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2)==1)
		 	  	  	{
		 	  			  zatvaranje_vrata();
		 	  			  flag_vrata = 0;
		 	  	  	}
		  else
		  		  {
		  			  flag_btn = 0;
		  		  }
	  	  break;

	  case(3):
		  if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2)==1)
				  	{
				  	  	  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0); //stop PWM signal
			  			  if(stanje_vozila==2 && flag_aut==1)
			  			  {
			  				  if(Distance < 15)
			  				  {
			  					  flag_btn=2;
			  					  flag_aut=0;
			  				  }
			  			  }
			  			  else if(stanje_vozila==1 && flag_aut==1)
			  			  {
			  				  ultrazvucni();
			  				  if(Distance > 60)
			  				  {
			  					  HAL_Delay(2000);
			  					  flag_btn=2;
			  					  flag_aut=0;
			  				  }
			  			  }
				  	}
	  	  break;

	  case(4):
	  	  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)==1)
	  			 	{
	  			 	  	  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);  //stop PWM signal
	  			 	}
	  	  break;
	  }


	 if (flag_vrata==1 && stanje_vozila==2)
	  {
		  ultrazvucni();
		  ispis_udaljenosti();
		  buzzer();
		  HAL_Delay(200);
	  }
	 else if(flag_vrata==0)
	 {
		 HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);
	 }





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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
