/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dio.h"
#include "anin.h"
#include "radio.h"
#include "common.h"


/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
I2C_HandleTypeDef hi2c1;
IWDG_HandleTypeDef hiwdg;
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;

uint8_t eeprom_buffer[EE_BUFFER_SIZE];


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_IWDG_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void RAM_Init(void);


int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_IWDG_Init();
	MX_GPIO_Init();
	MX_SPI1_Init();
	MX_I2C1_Init();	
	MX_USART1_UART_Init();
	MX_ADC1_Init();
	RAM_Init();
	
	while (1)
	{
		DIO_Service();
		ANIN_Service();
		RADIO_Service();
		HAL_IWDG_Refresh(&hiwdg);
	}
}


/** 
*	System Clock Configuration
*/
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time >
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick 
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


/* ADC1 init function */
static void MX_ADC1_Init(void)
{
	ADC_ChannelConfTypeDef sConfig;

	/**Common config 
	*/
	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 9;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel 
	*/
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}


/* I2C1 init function */
static void MX_I2C1_Init(void)
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}

/* IWDG init function */
static void MX_IWDG_Init(void)
{
	/**
	*	~409 ms timeout
	*	(((1 / 40000 Hz) * reload) * divider)
	*/
	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
	hiwdg.Init.Reload = 4095;
	if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}


/* SPI1 init function */
static void MX_SPI1_Init(void)
{
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}


/* USART1 init function */
static void MX_USART1_UART_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 9600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}


/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
#ifdef ULINK_DEBUGGER
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);
#else
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
#endif
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_8, GPIO_PIN_SET);
	
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
							|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_5|GPIO_PIN_8
							|GPIO_PIN_9, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_4, GPIO_PIN_SET);

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PD0 PD1 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pins : PA2 PA3 PA4 PA8 
						   PA11 PA12 PA15 */
#ifdef ULINK_DEBUGGER
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_8 
						  |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15;
#else
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_8 
						  |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
#endif
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB0 PB10 PB11 PB12 
						   PB13 PB14 PB15 PB4 
						   PB5 PB8 PB9 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12 
						  |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_4 
						  |GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PB1 PB2 PB3 */
	GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure peripheral I/O remapping */
	__HAL_AFIO_REMAP_PD01_ENABLE();

}


static void RAM_Init(void)
{
#ifdef EE_WRITE_DEFAULT
	HAL_Delay(100);
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_BATTERY_TYPE_ADDRESS;
	eeprom_buffer[2] = BATTERY_TYPE_DEFAULT;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
    HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_EMPTY_BATTERY_VOLTAGE_ADDRESS;
	eeprom_buffer[2] = (VBATT_MIN_DEFAULT >> 8);
	eeprom_buffer[3] = (VBATT_MIN_DEFAULT & 0xff);
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_WARNING_BATTERY_VOLTAGE_ADDRESS;
	eeprom_buffer[2] = (VBATT_WARNING_DEFAULT >> 8);
	eeprom_buffer[3] = (VBATT_WARNING_DEFAULT & 0xff);
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
    HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_FULL_BATTERY_VOLTAGE_ADDRESS;
	eeprom_buffer[2] = (VBATT_MAX_DEFAULT >> 8);
	eeprom_buffer[3] = (VBATT_MAX_DEFAULT & 0xff);
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
    HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN0_TYPE_ADDRESS;
	eeprom_buffer[2] = ANIN0_TYPE_DEFAULT;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
    HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN1_TYPE_ADDRESS;
	eeprom_buffer[2] = ANIN1_TYPE_DEFAULT;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN2_TYPE_ADDRESS;
	eeprom_buffer[2] = ANIN2_TYPE_DEFAULT;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN3_TYPE_ADDRESS;
	eeprom_buffer[2] = ANIN3_TYPE_DEFAULT;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN4_TYPE_ADDRESS;
	eeprom_buffer[2] = ANIN4_TYPE_DEFAULT;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN5_TYPE_ADDRESS;
	eeprom_buffer[2] = ANIN5_TYPE_DEFAULT;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN6_TYPE_ADDRESS;
	eeprom_buffer[2] = ANIN6_TYPE_DEFAULT;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN7_TYPE_ADDRESS;
	eeprom_buffer[2] = ANIN7_TYPE_DEFAULT;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN0_NULL_OFFSET_ADDRESS;
	eeprom_buffer[2] = (ANIN0_NULL_OFFSET_DEFAULT >> 8);
	eeprom_buffer[3] = (ANIN0_NULL_OFFSET_DEFAULT & 0xff);
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN1_NULL_OFFSET_ADDRESS;
	eeprom_buffer[2] = (ANIN1_NULL_OFFSET_DEFAULT >> 8);
	eeprom_buffer[3] = (ANIN1_NULL_OFFSET_DEFAULT & 0xff);
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN2_NULL_OFFSET_ADDRESS;
	eeprom_buffer[2] = (ANIN2_NULL_OFFSET_DEFAULT >> 8);
	eeprom_buffer[3] = (ANIN2_NULL_OFFSET_DEFAULT & 0xff);
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN3_NULL_OFFSET_ADDRESS;
	eeprom_buffer[2] = (ANIN3_NULL_OFFSET_DEFAULT >> 8);
	eeprom_buffer[3] = (ANIN3_NULL_OFFSET_DEFAULT & 0xff);
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN4_NULL_OFFSET_ADDRESS;
	eeprom_buffer[2] = (ANIN4_NULL_OFFSET_DEFAULT >> 8);
	eeprom_buffer[3] = (ANIN4_NULL_OFFSET_DEFAULT & 0xff);
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
    HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN5_NULL_OFFSET_ADDRESS;
	eeprom_buffer[2] = (ANIN5_NULL_OFFSET_DEFAULT >> 8);
	eeprom_buffer[3] = (ANIN5_NULL_OFFSET_DEFAULT & 0xff);
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN6_NULL_OFFSET_ADDRESS;
	eeprom_buffer[2] = (ANIN6_NULL_OFFSET_DEFAULT >> 8);
	eeprom_buffer[3] = (ANIN6_NULL_OFFSET_DEFAULT & 0xff);
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN7_NULL_OFFSET_ADDRESS;
	eeprom_buffer[2] = (ANIN7_NULL_OFFSET_DEFAULT >> 8);
	eeprom_buffer[3] = (ANIN7_NULL_OFFSET_DEFAULT & 0xff);
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
	HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
#endif
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_BATTERY_TYPE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, &batt_type, 1, EE_TIMEOUT);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_EMPTY_BATTERY_VOLTAGE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, eeprom_buffer, 2, EE_TIMEOUT);
	vbatt_min = ((eeprom_buffer[0] << 8) + eeprom_buffer[1]);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_WARNING_BATTERY_VOLTAGE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, eeprom_buffer, 2, EE_TIMEOUT);
	vbatt_warning = ((eeprom_buffer[0] << 8) + eeprom_buffer[1]);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_FULL_BATTERY_VOLTAGE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, eeprom_buffer, 2, EE_TIMEOUT);
	vbatt_max = ((eeprom_buffer[0] << 8) + eeprom_buffer[1]);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN0_TYPE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, &anin0_type, 1, EE_TIMEOUT);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN1_TYPE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, &anin1_type, 1, EE_TIMEOUT);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN2_TYPE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, &anin2_type, 1, EE_TIMEOUT);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN3_TYPE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, &anin3_type, 1, EE_TIMEOUT);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN4_TYPE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, &anin4_type, 1, EE_TIMEOUT);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN5_TYPE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, &anin5_type, 1, EE_TIMEOUT);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN6_TYPE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, &anin6_type, 1, EE_TIMEOUT);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN7_TYPE_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, &anin7_type, 1, EE_TIMEOUT);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN0_NULL_OFFSET_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, eeprom_buffer, 2, EE_TIMEOUT);
	anin0_null = ((eeprom_buffer[0] << 8) + eeprom_buffer[1]);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN1_NULL_OFFSET_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, eeprom_buffer, 2, EE_TIMEOUT);
	anin1_null = ((eeprom_buffer[0] << 8) + eeprom_buffer[1]);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN2_NULL_OFFSET_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, eeprom_buffer, 2, EE_TIMEOUT);
	anin2_null = ((eeprom_buffer[0] << 8) + eeprom_buffer[1]);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN3_NULL_OFFSET_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, eeprom_buffer, 2, EE_TIMEOUT);
	anin3_null = ((eeprom_buffer[0] << 8) + eeprom_buffer[1]);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN4_NULL_OFFSET_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, eeprom_buffer, 2, EE_TIMEOUT);
	anin4_null = ((eeprom_buffer[0] << 8) + eeprom_buffer[1]);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN5_NULL_OFFSET_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, eeprom_buffer, 2, EE_TIMEOUT);
	anin5_null = ((eeprom_buffer[0] << 8) + eeprom_buffer[1]);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN6_NULL_OFFSET_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, eeprom_buffer, 2, EE_TIMEOUT);
	anin6_null = ((eeprom_buffer[0] << 8) + eeprom_buffer[1]);
	
	eeprom_buffer[0] = 0x00;
	eeprom_buffer[1] = EE_ANIN7_NULL_OFFSET_ADDRESS;
	HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 2, EE_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EE_READ, eeprom_buffer, 2, EE_TIMEOUT);
	anin7_null = ((eeprom_buffer[0] << 8) + eeprom_buffer[1]);
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
