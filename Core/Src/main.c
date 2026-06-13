/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
	uint8_t ledMask;
	uint32_t duration;

} BlinkPatternStep;

typedef struct
{
	const BlinkPatternStep *steps;
	uint8_t stepCount;

} BlinkPattern;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PIN_BUTTON_MID GPIO_PIN_15
#define PIN_BUTTON_LEFT GPIO_PIN_8
#define PIN_BUTTON_RIGHT GPIO_PIN_6
#define PIN_BUTTON_UP GPIO_PIN_9
#define PIN_BUTTON_DOWN GPIO_PIN_11
#define MIN_BLINK_DELAY 100
#define MAX_BLINK_DELAY 1000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t blinkingEnabled = 0;
volatile uint8_t changePatternFlag = 0;
volatile uint8_t patternIndex = 0;
volatile uint16_t blinkDelay = 200;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/**
 * @brief Decodes the active pattern and sets LED states.
 * Uses 'static' variables to remember the current step between function calls
 * without blocking the main loop. Resets the step if a new pattern is detected.
 */
void setLEDs(const BlinkPattern *blinkPattern)
{
	static uint8_t currentStep = 0;
	static const BlinkPattern *lastPattern = NULL;

	// Reset step counter for new pattern
	if(blinkPattern != lastPattern)
	{
		currentStep = 0;
		lastPattern = blinkPattern;
	}


	uint8_t mask = blinkPattern->steps[currentStep].ledMask;
	uint8_t duration = blinkPattern->steps[currentStep].duration;

	HAL_GPIO_WritePin(GPIOD, LED_BLUE_Pin, (mask & 0b0001) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, LED_RED_Pin, (mask & 0b0010) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, LED_ORANGE_Pin,(mask & 0b0100) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, LED_GREEN_Pin,(mask & 0b1000) ? GPIO_PIN_SET : GPIO_PIN_RESET);

	HAL_Delay(blinkDelay*duration);


	currentStep = (currentStep + 1) % blinkPattern->stepCount;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//=============== Pattern definition section ================
// 1000 -- green, 0100 -- orange, 0010 -- red, 0001 -- blue
const BlinkPatternStep pattern_allBlink[] =
{
		{0b1111, 2},
		{0b0000, 2}
};

const BlinkPatternStep pattern_twoDiodes[] =
{
		{0b1010, 2},
		{0b0101, 2}
};
const BlinkPatternStep pattern_police[] =
{
		{0b0011, 1},
		{0b0000, 1},
		{0b0011, 1},
		{0b0000, 5}
};
const BlinkPatternStep pattern_snake[] =
{
		{0b1100, 1},
		{0b0110, 1},
		{0b0011, 1},
		{0b1001, 1}
};
//============== Pattern definition section END ==============



const BlinkPattern blinkPatterns[] =
{
		{pattern_snake, sizeof(pattern_snake)/sizeof(pattern_snake[0])},
		{pattern_allBlink, sizeof(pattern_allBlink)/sizeof(pattern_allBlink[0])},
		{pattern_police, sizeof(pattern_police)/sizeof(pattern_police[0])},
		{pattern_twoDiodes, sizeof(pattern_twoDiodes)/sizeof(pattern_twoDiodes[0])}
};

const uint8_t numberOfPatterns = sizeof(blinkPatterns)/sizeof(blinkPatterns[0]);
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
  /* USER CODE BEGIN 2 */
  const BlinkPattern *currentPattern = &blinkPatterns[0];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(blinkingEnabled)
	  {
		  // Check if ISR requested a pattern change
		  if(changePatternFlag)
		  {
			  currentPattern = &blinkPatterns[patternIndex];
			  changePatternFlag = 0; // Clear the flag after the change
		  }

		  // Execute led blinking if enabled
		  setLEDs(currentPattern);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED_GREEN_Pin|LED_ORANGE_Pin|LED_RED_Pin|LED_BLUE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_GREEN_Pin LED_ORANGE_Pin LED_RED_Pin LED_BLUE_Pin */
  GPIO_InitStruct.Pin = LED_GREEN_Pin|LED_ORANGE_Pin|LED_RED_Pin|LED_BLUE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PC6 PC8 PC9 PC11 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		// Button start/stop blinking
		case PIN_BUTTON_MID:
			blinkingEnabled = !blinkingEnabled;

			// Turn off LEDs to ensure they don't freeze
			// in the ON state when blinking is paused.
			if(!blinkingEnabled)
			{
				HAL_GPIO_WritePin(GPIOD, LED_GREEN_Pin|LED_ORANGE_Pin|LED_RED_Pin|LED_BLUE_Pin, GPIO_PIN_RESET);
			}
			break;

		// Button previous pattern
		case PIN_BUTTON_LEFT:

			if(patternIndex == 0)
			{
				patternIndex = numberOfPatterns - 1;

			} else {

				patternIndex--;
			}

			changePatternFlag = 1;

			break;

		// Button next pattern
		case PIN_BUTTON_RIGHT:

			patternIndex = (patternIndex + 1) % numberOfPatterns;
			changePatternFlag = 1;

			break;

		// Button speed up by 50 ms
		case PIN_BUTTON_UP:
			if(blinkDelay > MIN_BLINK_DELAY) blinkDelay -= 50;
			break;

		// Button slow down by 50 ms
		case PIN_BUTTON_DOWN:
			if(blinkDelay < MAX_BLINK_DELAY) blinkDelay += 50;
			break;

		default:
			break;
	}
}
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
#ifdef USE_FULL_ASSERT
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
