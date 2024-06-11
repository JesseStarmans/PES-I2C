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
#include <stdio.h>
#include <string.h>
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
I2C_HandleTypeDef hi2c3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
extern uint8_t I2C_REGISTERS[10];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C3_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//uint8_t buf[12] = {0};
	//HAL_StatusTypeDef ret;
	//double valu;
	//float temp_c;
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
  MX_I2C3_Init();
  MX_TIM1_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  if (HAL_I2C_EnableListen_IT(&hi2c1) != HAL_OK){
	  HAL_UART_Transmit(&huart2, (uint8_t*)"I2C fail", 9, 100);
  	  Error_Handler();
  }

  uint8_t sensorAddressCO2 = 0x58;// Sensor address CO2
  uint8_t dataCO2[6]; // Data buffer to store sensor readings
  uint8_t commandCO2[] = {0x20, 0x08}; // Command for reading measurement data
  uint8_t alarmStatus = 0; // alarm status Co2
  uint8_t requestTemp = 20; // gewilde temperatuur
  uint8_t verwarmingStatus = 0; // Status verwarming
  uint16_t grenswaarde = 800; // Grenswaarde Co2 voor alarm
  uint8_t aan = 1;

  // Initiate de Co2 sensor (15-20sec calibratie)
  HAL_I2C_Master_Transmit(&hi2c3, 0x58 << 1, (uint8_t[]) {0x20, 0x03}, 2, HAL_MAX_DELAY); // om de SGP30 sensor te initialiseren
  for(int i = 0; i < 20; i++){
		HAL_I2C_Master_Transmit(&hi2c3, sensorAddressCO2 << 1, commandCO2, sizeof(commandCO2), HAL_MAX_DELAY);

		// Wait for measurement to complete
		HAL_Delay(20);

		// Read measurement data
		HAL_I2C_Master_Receive(&hi2c3, (sensorAddressCO2 << 1) | 0x01, dataCO2, sizeof(dataCO2), HAL_MAX_DELAY);
		HAL_Delay(1000);
  }
  // Ready to receive commands
  HAL_UART_Transmit(&huart2, (uint8_t*)"Done\n", 6, HAL_MAX_DELAY);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{

		/*-------------- Set gewilde temperatuur --------------*/
		if (I2C_REGISTERS[1] == 201){
			requestTemp = I2C_REGISTERS[2];
			// Geeft bij server aan dat data in registers staat
			I2C_REGISTERS[0] = 1;
			// Delay om zeker te weten dat hij ontvangen is door server
			HAL_Delay(100);
			// Reset registers
			for(int i = 0; i < 10; i++){
				I2C_REGISTERS[i] = 0;
			}
		}
		/*-------------- Vraag temperatuur op --------------*/
		else if (I2C_REGISTERS[1] == 200){
			//HAL_UART_Transmit(&huart2, (uint8_t*)"I2C test", 9, 100);
			char buf[50];
			// Read data from SHT3x sensor
			uint8_t sensorAddress = 0x44; // Sensor address
			uint8_t data[6]; // Data buffer to store sensor readings

			// Request temperature measurement
			uint8_t command[] = {0x2C, 0x06};
			HAL_I2C_Master_Transmit(&hi2c3, sensorAddress << 1, command, sizeof(command), HAL_MAX_DELAY);

			// Wait for measurement to complete
			HAL_Delay(20);

			// Read measurement data
			HAL_I2C_Master_Receive(&hi2c3, (sensorAddress << 1) | 0x01, data, sizeof(data), HAL_MAX_DELAY);

			// Process data to get temperature and humidity
			int16_t tempRaw = (data[0] << 8) | data[1];
			float temperature = -45 + 175 * ((float)tempRaw / 65535);
			uint16_t humRaw = (data[3] << 8) | data[4];
			float humidity = 100 * ((float)humRaw / 65535);
			// if lager dan gewenste temperatuur; zet de verwarming aan.
			if (temperature >= (float)requestTemp){
				verwarmingStatus = 2;
			}
			else {verwarmingStatus = 1;}
			// Print data to Serial port (uart)
			sprintf(buf, "%.2lfC Luchtvochtigheid: %.1lf VerwarmingStatus: %d \r\n", temperature, humidity, verwarmingStatus);
			HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
			// Empty registers
			for(int i = 0; i < 10; i++){
				I2C_REGISTERS[i] = 0;
			}

			int intPart, decPart;
			// Neem het gehele deel van de float
			intPart = (int)temperature;

			// Neem het decimale deel van de float
			float decimal = temperature - (float)(intPart);
			// Converteer het decimale deel naar een percentage (bijv. 0.56 wordt 56)
			decPart = (int)(decimal * 100);
			// Plaats data in juiste register(s)
			I2C_REGISTERS[1] = (uint8_t) intPart;
			I2C_REGISTERS[2] = (uint8_t) decPart;
			I2C_REGISTERS[3] = (uint8_t)humidity;
			I2C_REGISTERS[4] = verwarmingStatus;

			I2C_REGISTERS[0] = 1; //Aangeven bij server dat alles is geschreven.
			// Delay om server de tijd te geven om data te lezen
			HAL_Delay(100);
			// Reset Registers
			for(int i = 0; i < 10; i++){
				I2C_REGISTERS[i] = 0;
			}
		}
		/*-------------- Lees Co2 uit --------------*/
		else if(I2C_REGISTERS[1] == 203){
			char buf2[50];
			HAL_I2C_Master_Transmit(&hi2c3, sensorAddressCO2 << 1, commandCO2, sizeof(commandCO2), HAL_MAX_DELAY);

			// Wait for measurement to complete
			HAL_Delay(20);

			// Read measurement data
			HAL_I2C_Master_Receive(&hi2c3, (sensorAddressCO2 << 1) | 0x01, dataCO2, sizeof(dataCO2), HAL_MAX_DELAY);

			// Process data to get CO2
			uint16_t CO2 = (dataCO2[0] << 8) | dataCO2[1];
			// if CO2 hoger is dan grenswaarde; zet het alarm aan.
			if(CO2>grenswaarde){
				alarmStatus = 1;
			}
			else {
				alarmStatus = 2;
			}

			// Process data to TVOC
			uint16_t TVOC = (dataCO2[3] << 8) | dataCO2[4];
			// Print to seriele monitor
			sprintf(buf2, " CO2: %d\r\n", CO2);
			HAL_UART_Transmit(&huart2, (uint8_t*)buf2, strlen(buf2), 100);
			// Reset Registers
			for(int i = 0; i < 10; i++){
				I2C_REGISTERS[i] = 0;
			}
			// Opsplitsen van de 16-bits CO2 waarde in twee 8-bits waarden
			uint8_t CO2_high = (uint8_t)(CO2 >> 8); // Hoger byte
			uint8_t CO2_low = (uint8_t)(CO2 & 0xFF); // Lager byte

			// Sla de gesplitste waarden op in de I2C registers
			I2C_REGISTERS[1] = CO2_high;
			I2C_REGISTERS[2] = CO2_low;
			I2C_REGISTERS[3] = alarmStatus;
			I2C_REGISTERS[0] = 1;

			// Geef server kans om data uit te lezen
			HAL_Delay(100);
			// Reset Registers
			for(int i = 0; i < 10; i++){
				I2C_REGISTERS[i] = 0;
			}
		}
		/*-------------- Set grenswaarde van Co2 --------------*/
		else if(I2C_REGISTERS[1] == 204){
			grenswaarde = (I2C_REGISTERS[2] << 8 | I2C_REGISTERS[3]);
			// Geef aan dat data in Register is geschreven
			I2C_REGISTERS[0] = 1;
			// Delay om server kans te geven om data te lezen
			HAL_Delay(100);
			// Reset Registers
			for(int i = 0; i < 10; i++){
				I2C_REGISTERS[i] = 0;
			}
		}
		/*-------------- Reset alarm --------------*/
		else if (I2C_REGISTERS[1] == 205){
			alarmStatus = 2;
		}
		else if (I2C_REGISTERS[1] == 206){
			alarmStatus = I2C_REGISTERS[2];
			I2C_REGISTERS[0] = 1;
			HAL_Delay(100);
			for(int i = 0; i < 10; i++){
				I2C_REGISTERS[i] = 0;
			}
		}

	  // zet de buzzer aan als de alarmStatus == 1
//	  if(alarmStatus == 1 && aan){
//		  aan = 0;
//		  buzzer_aan();
//	  }
//	  // om de buzzer te stoppen als alarmStatus != 1
//	  else  {
//		  aan =1;
//		  HAL_TIM_Base_Stop(&htim1); /**< Stop de timer */
//		  HAL_TIM_Base_Stop(&htim16); /**< Stop de timer */
//	  }

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
  hi2c1.Init.OwnAddress1 = 64;
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
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x00707CBB;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

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
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 10-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim16) != HAL_OK)
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
  if (HAL_TIM_OC_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_TIM_ENABLE_OCxPRELOAD(&htim16, TIM_CHANNEL_1);
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */
  HAL_TIM_MspPostInit(&htim16);

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
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

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
	  HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1); // start timer16 die de buzzer aanstuurt
	  uint32_t soft_pulse = (__HAL_TIM_GetAutoreload(&htim16) * 10) / 100; // om een zacht pulse te genereren
	  int freq = 3200000 / 3200;
	  __HAL_TIM_SET_AUTORELOAD(&htim16,freq - 1);
	  __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, soft_pulse);

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
    if (htim == &htim1){
    	//uint8_t tekst1[]={"interrupt1!\n\r"};
    	//HAL_UART_Transmit(&huart2, tekst1, sizeof tekst1, HAL_MAX_DELAY);
    	int freq = __HAL_TIM_GetAutoreload(&htim16);
    	uint32_t soft_pulse = (__HAL_TIM_GetAutoreload(&htim16) * 10) / 100; // 10% duty cycle for soft tone
    	uint32_t hard_pulse = (__HAL_TIM_GetAutoreload(&htim16) * 90) / 100; //100% duty cycle for hard tone

    	if (freq == (3200000 / 4000)-1){
    		uint8_t tekst2[]={"interrupt2!\n\r"};
    		HAL_UART_Transmit(&huart2, tekst2, sizeof tekst2, HAL_MAX_DELAY);
    		uint32_t freq1 = 3200000 / 3200;
    		__HAL_TIM_SET_AUTORELOAD(&htim16, (freq1 - 1));
    		__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, hard_pulse);
    	}
    	else{
    		uint8_t tekst3[]={"interrupt3!\n\r"};
    		HAL_UART_Transmit(&huart2, tekst3, sizeof tekst3, HAL_MAX_DELAY);
    		uint32_t freq2 = 3200000 / 4000;
    		__HAL_TIM_SET_AUTORELOAD(&htim16, (freq2 - 1));
    		__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, soft_pulse);
    	}
    	//__HAL_TIM_SET_COUNTER(&htim2, 0);
    	//HAL_TIM_Base_Start_IT(&htim1);
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
