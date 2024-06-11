@page subMainPage4 Microcontroller code
@tableofcontents

@section sectionMicrocontroller1 Inleiding
Op deze pagina is de C code te vinden voor iedere sensor die gekoppeld is aan de NUCLEO-L432KC microcontrollers.

@section sectionMicrocontroller2 I2C_Slave
Op de microcontrollers wordt er gebruik gemaakt van I2C voor de verbinding tussen de microcontrollers en de Raspberry Pi. De documentatie van deze code is te vinden in het \ref I2C_Slave.c "I2C Slave code" bestand. Hiervoor moet in de main.c de hi2c1 aangezet worden en de HAL_I2C_EnableListen_IT() functie moet aangeroepen worden, dit kan zoals in de volgende code:
@code
/* USER CODE BEGIN 2 */
if (HAL_I2C_EnableListen_IT(&hi2c1) != HAL_OK){
    Error_Handler();
}
/* USER CODE END 2 */
@endcode
Ook moet in de main.c de I2C_REGISTERS als een externe variabele gedefinieerd worden, zodat deze gebruikt kan worden in de main.

@section sectionMicrocontroller3 CO2/Brand sensor
Om te kunnen detecteren of er brand is in het gebouw, wordt er gebruik gemaakt van de SGP30 TVOC eCO2 sensor. De documentatie van de code om deze sensor aan te sturen, is te vinden in het \ref mainCO2.c "CO2/Brand sensor main" bestand.\n
De functionele code hiervan is ook hieronder weergegeven:
@code
HAL_I2C_Master_Transmit(&hi2c3, sensorAddress << 1, (uint8_t[]) {0x20, 0x03}, 2, HAL_MAX_DELAY); // Om de SGP30 sensor te initialiseren
while (1) {


    char buf[50];
    // Read data from SGP30 sensor
    uint8_t data[6]; // buffer om de data in op te slaan

    // Request measurement
    uint8_t command[] = {0x20, 0x08}; // Command voor het lezen van de data
    HAL_I2C_Master_Transmit(&hi2c3, sensorAddress << 1, command, sizeof(command), HAL_MAX_DELAY);

    // Delay voor de meting
    HAL_Delay(20);

    // lees de gemeten data
    HAL_I2C_Master_Receive(&hi2c3, (sensorAddress << 1) | 0x01, data, sizeof(data), HAL_MAX_DELAY);

    // Process data to get CO2 and TVOC
    int16_t CO2 = (data[0] << 8) | data[1];
    uint16_t TVOC = (data[3] << 8) | data[4];

    // Print data to Serial port (UART)
    sprintf(buf, "CO2: %d TVOC: %d \r\n", CO2, TVOC);
    HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
	int uit = 0;
    if(CO2>600 || uit){
    	//alarm = 1;
        buzzer_aan();
    }
}
@endcode