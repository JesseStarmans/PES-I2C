/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBOUNCE_DELAY_MS 20 // Adjust this value as needed
#define MAX_DUTY_CYCLE  100  // Maximum duty cycle percentage
#define MIN_DUTY_CYCLE  10   // Minimum duty cycle percentage
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim15;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
extern uint8_t I2C_REGISTERS[10];
uint32_t lastButtonPressTime = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM15_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void buzzer_aan();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int beweging = 1;
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM15_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /**
    ******************************************************************************
    * @brief      While-loop
    *
    * @details    In de while wordt de waarde van de sensor uitgelezen en wordt de LED aangezet
    * 			  als de sensor waarde 1 is en hij aanstaat. Ook wordt dan de Timer van 5 seconde
    * 			  gestart.
    *
    * @retval     uint8_t sensorValue leest de waarde van de Bewegingssensor
    * 			  uint8_t alarmStatus krijgt een waarde over I2C en zal hierop reageren.
    ******************************************************************************
    */
  if (HAL_I2C_EnableListen_IT(&hi2c1) != HAL_OK){
	  HAL_UART_Transmit(&huart2, (uint8_t*)"I2C fail", 9, 100);
  	  Error_Handler();
    }

  uint8_t alarmStatus = 0;
  uint8_t knopStatus = 0;
  uint8_t aan = 0;
  while (1) {
      // Als I2C register het id 206 stuurt zal alarmStatus gelijk worden aan index 2 van dit register.
		if (I2C_REGISTERS[1] == 206){
			alarmStatus = I2C_REGISTERS[2];
			knopStatus = I2C_REGISTERS[3];
			I2C_REGISTERS[0] = 1;
			HAL_Delay(100);
			for(int i = 0; i < 10; i++){
				I2C_REGISTERS[i] = 0;
			}
		}

      // Lees de bewegingssensor waarde uit
      uint8_t sensorValue = HAL_GPIO_ReadPin(Bewegings_Sensor_GPIO_Port, Bewegings_Sensor_Pin);
      // Als er beweging wordt gedetecteerd zal de Led aangaan en wordt de timer gestart.}
      if (sensorValue == 1 && beweging == 1) {
    	  HAL_TIM_Base_Start_IT(&htim2); //timer 2 wordt gestart
    	  __HAL_TIM_SET_COUNTER(&htim2, 0); // counter van de timer wordt op 0 gezet
    	  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET); // De LED wordt aangezet.
      }

	  // zet de buzzer aan als de alarmStatus == 1
      if(alarmStatus == 1 && knopStatus == 2 && !aan){
    	  aan = 1;
    	  buzzer_aan();
      }
      // om de buzzer te stoppen als alarmStatus != 1
      else if (knopStatus == 1)  {
    	  aan = 0;
    	  HAL_TIM_Base_Stop(&htim1); /**< Stop de timer */
    	  HAL_TIM_Base_Stop(&htim15); /**< Stop de timer */
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00707CBB;
  hi2c1.Init.OwnAddress1 = 32;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 4800-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 3000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 4800-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 20000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 10-1;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 65535;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_TIM_ENABLE_OCxPRELOAD(&htim15, TIM_CHANNEL_2);
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim15, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */
  HAL_TIM_MspPostInit(&htim15);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD_Knop_Pin|LD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LD_Knop_Pin LD4_Pin */
  GPIO_InitStruct.Pin = LD_Knop_Pin|LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Licht_Knop_Pin */
  GPIO_InitStruct.Pin = Licht_Knop_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Licht_Knop_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Beweging_Knop_Pin */
  GPIO_InitStruct.Pin = Beweging_Knop_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Beweging_Knop_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Bewegings_Sensor_Pin */
  GPIO_InitStruct.Pin = Bewegings_Sensor_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(Bewegings_Sensor_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


/**
  ******************************************************************************
  * @brief      Zet de buzzer aan.
  *
  * @details    Deze functie zet de buzzer aan en start een timer van 250 ms om dan
  * 			om deze periode van 250 ms de frequentie te veranderen van de buzzer.
  *
  * @retval     int   Geeft aan of de sensor is uitgeschakeld (0).
  ******************************************************************************
  */
void buzzer_aan(){
	  HAL_TIM_Base_Start_IT(&htim1); // start timer1
	  HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_2); // start timer16 die de buzzer aanstuurt
	  uint32_t soft_pulse = (__HAL_TIM_GetAutoreload(&htim15) * 10) / 100; // om een zacht pulse te genereren
	  int freq = 3200000 / 3200;
	  __HAL_TIM_SET_AUTORELOAD(&htim15,freq - 1);
	  __HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_2, soft_pulse);

}

/**
  ******************************************************************************
  * @brief      Zet de bewegingssensor uit.
  *
  * @details    Deze functie zet de bewegingssensor uit, stuurt een bericht via UART
  *             om de status te melden, zet de LED uit en stopt de timer.
  *
  * @retval     int   Geeft aan of de sensor is uitgeschakeld (0).
  ******************************************************************************
  */
int Sensor_uit(){
    int aan = 2; /**< Variabele om aan te geven dat de sensor uit is */
    uint8_t tekst2[]={"Bewegingssensor uit!\n\r"}; /**< Tekst om via UART te verzenden */
   HAL_UART_Transmit(&huart2, tekst2, sizeof tekst2, HAL_MAX_DELAY); /**< Verzend bericht via UART */
    HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET); /**< Zet LED uit */
    HAL_TIM_Base_Stop(&htim2); /**< Stop de timer */
    return aan; /**< Geef aan dat sensor uit is */
}

/**
  ******************************************************************************
  * @brief      Zet de bewegingssensor aan.
  *
  * @details    Deze functie zet de bewegingssensor aan en geeft aan dat de sensor
  *             weer is ingeschakeld.
  *
  * @retval     int   Geeft aan dat de sensor is ingeschakeld (1).
  ******************************************************************************
  */
int Sensor_aan(){
    int aan = 1; /**< Variabele om aan te geven dat de sensor aan is */
    return aan; /**< Geef aan dat sensor aan is */
}

/**
  ******************************************************************************
  * @brief      Callback-functie voor GPIO-interrupts.
  *
  * @details    Deze functie wordt aangeroepen wanneer een GPIO-interrupt wordt gedetecteerd.
  *             Het voert debouncing uit en schakelt de bewegingssensor in of uit, afhankelijk
  *             van welke knop is ingedrukt.
  *
  * @param[in]  GPIO_Pin   De pin waarvoor de interrupt is geactiveerd.
  ******************************************************************************
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    static uint32_t last_interrupt_time = 0; /**< Tijd van de laatste interrupt */
    uint32_t current_time = HAL_GetTick(); /**< Huidige tijd */
    uint32_t debounce_delay = 200; /**< Debounce-vertraging in milliseconden */

    // Controleer of de tijd sinds de laatste knopdruk voldoet aan de debounce-vertraging
    if ((current_time - last_interrupt_time) > debounce_delay) {
    	 char msg[50];
    	 sprintf(msg, "Interrupt on pin: %d\n\r", GPIO_Pin);
    	 HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        // Controleer of de lichtknop is ingedrukt
        if (GPIO_Pin == Licht_Knop_Pin) {
            uint8_t tekst1[]={"interrupt!\n\r"}; /**< Tekst om via UART te verzenden */
           HAL_UART_Transmit(&huart2, tekst1, sizeof tekst1, HAL_MAX_DELAY); /**< Verzend bericht via UART */
            HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin); /**< Toggle de LED */
            if(__HAL_TIM_GET_COUNTER(&htim2) > 0){
                HAL_TIM_Base_Stop(&htim2); /**< Stop de timer indien actief */
            }
        }
        // Controleer of de bewegingsknop is ingedrukt
        if (GPIO_Pin == Beweging_Knop_Pin) {
            if(beweging == 1){ // Controleer of de sensor aan staat
                beweging = Sensor_uit(); // Zet sensor uit
                HAL_GPIO_WritePin(LD_Knop_GPIO_Port, LD_Knop_Pin, GPIO_PIN_SET); // Zet LED op knop aan
            }
            else {
                uint8_t tekst2[]={"Bewegingssensor aan!\n\r"}; /**< Tekst om via UART te verzenden */
               HAL_UART_Transmit(&huart2, tekst2, sizeof tekst2, HAL_MAX_DELAY); /**< Verzend bericht via UART */
                beweging = Sensor_aan(); // Zet sensor aan
                HAL_GPIO_WritePin(LD_Knop_GPIO_Port, LD_Knop_Pin, GPIO_PIN_RESET); // Zet LED op knop uit
            }
        }
        last_interrupt_time = current_time; /**< Update de tijd van de laatste interrupt */
    }
}

/**
  ******************************************************************************
  * @brief      Callback-functie voor periode-afgelopen gebeurtenissen van de timer.
  *
  * @details    Deze functie wordt aangeroepen wanneer de periode van de timer is verstreken.
  *             Het veranderd de frequentie en hardheid van de buzzer naar aanleiding van de status hiervan.
  *
  * @param[in]  htim   Handle van de timer waarvoor de gebeurtenis is opgetreden.
  ******************************************************************************
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // Controleer of het de juiste timer is
    if (htim == &htim2 ) {
        HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET); /**< Zet LED uit */
        HAL_TIM_Base_Stop(&htim2); /**< Stop de timer */
    }

    if (htim == &htim1){
    	//uint8_t tekst1[]={"interrupt1!\n\r"};
    	//HAL_UART_Transmit(&huart2, tekst1, sizeof tekst1, HAL_MAX_DELAY);
    	int freq = __HAL_TIM_GetAutoreload(&htim15);
    	uint32_t soft_pulse = (__HAL_TIM_GetAutoreload(&htim15) * 10) / 100; // 10% duty cycle for soft tone
    	uint32_t hard_pulse = (__HAL_TIM_GetAutoreload(&htim15) * 90) / 100; //100% duty cycle for hard tone

    	if (freq == (3200000 / 4000)-1){
//    		uint8_t tekst2[]={"interrupt2!\n\r"};
//    		HAL_UART_Transmit(&huart2, tekst2, sizeof tekst2, HAL_MAX_DELAY);
    		uint32_t freq1 = 3200000 / 3200;
    		__HAL_TIM_SET_AUTORELOAD(&htim15, (freq1 - 1));
    		__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_2, hard_pulse);
    	}
    	else{
//    		uint8_t tekst3[]={"interrupt3!\n\r"};
//    		HAL_UART_Transmit(&huart2, tekst3, sizeof tekst3, HAL_MAX_DELAY);
    		uint32_t freq2 = 3200000 / 4000;
    		__HAL_TIM_SET_AUTORELOAD(&htim15, (freq2 - 1));
    		__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_2, soft_pulse);
    	}
    	__HAL_TIM_SET_COUNTER(&htim1, 0);
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
