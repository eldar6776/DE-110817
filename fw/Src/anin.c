/**
 ******************************************************************************
 * File Name          : anin.c
 * Date               : 04/01/2018 5:19:19
 * Description        : analog input processing
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
 
/* Includes ------------------------------------------------------------------*/
#include "anin.h"
#include "dio.h"
#include "common.h"
#include "main.h"


/* Typedef -------------------------------------------------------------------*/
/* Define --------------------------------------------------------------------*/
/* Variable ------------------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;

volatile uint32_t anin_timer;
uint16_t anin0;
uint8_t	 anin0_type;
uint8_t	 anin0_set_type;
uint16_t anin0_null;
uint16_t anin1;
uint8_t	 anin1_type;
uint8_t	 anin1_set_type;
uint16_t anin1_null;
uint16_t anin2;
uint8_t	 anin2_type;
uint8_t	 anin2_set_type;
int16_t  anin2_null;
uint16_t anin3;
uint8_t	 anin3_type;
uint8_t	 anin3_set_type;
uint16_t anin3_null;
uint16_t anin4;
uint8_t	 anin4_type;
uint8_t	 anin4_set_type;
uint16_t anin4_null;
uint16_t anin5;
uint8_t	 anin5_type;
uint8_t	 anin5_set_type;
uint16_t anin5_null;
uint16_t anin6;
uint8_t	 anin6_type;
uint8_t	 anin6_set_type;
uint16_t anin6_null;
uint16_t anin7;
uint8_t	 anin7_type;
uint8_t	 anin7_set_type;
uint16_t anin7_null;
uint8_t batt_type;
uint16_t vbatt;
uint16_t vbatt_min;
uint16_t vbatt_warning;
uint16_t vbatt_max;
uint8_t pwm[PWM_BUFFER_SIZE];


/* Macro ---------------------------------------------------------------------*/
/* Program code   ------------------------------------------------------------*/
void ANIN_Service(void)
{
	static enum
	{
		ANIN_INIT				= 0x00,
		ANIN_MULTIPLEXER		= 0x01,
		ANIN_BATTERY_VOLTAGE	= 0x02
		
	} ANIN_ServiceState = ANIN_INIT;
	
	ADC_ChannelConfTypeDef sConfig;
	uint32_t pwm_cnt;

	switch(ANIN_ServiceState)
	{
		/** ==========================================================================*/
		/**     A N A L O G   C O N V E R T E R     I N I T I A L I Z A T I O N		  */
		/** ==========================================================================*/
		case ANIN_INIT:
		{	
			ClearBuffer(pwm, PWM_BUFFER_SIZE);
			ANIN_StartTimer(ANIN_POWERON_DELAY);			
			ANIN_ServiceState = ANIN_MULTIPLEXER;
			break;
		}		
		/** ==========================================================================*/
		/**   M U L T I P L E X E R	  A N A L O G   I N P U T   C O N V E R S I O N   */
		/** ==========================================================================*/		
		case ANIN_MULTIPLEXER:
		{
			sConfig.Channel = ADC_CHANNEL_0;
			sConfig.Rank = 1;
			sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
			if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
			{
				_Error_Handler(__FILE__, __LINE__);
			}
			
			ClearBuffer(pwm, PWM_BUFFER_SIZE);
			/**
			*	set adc chanel 0 input and start conversion
			*	of multiplexer analog input 0 value
			*/
			SelectMuxInput0();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT);
			anin0 = HAL_ADC_GetValue(&hadc1);			
			
			if(anin0_type == 1)
			{
				/**
				*	for common type potentiometer check limits of measured analog signal value:
				*	for wiper position below preset null position round pwm value to zero
				*	and for wiper position close to maximum set pwm to 100%
				*/
				if(anin0 < anin0_null) pwm[0] = 0;
				else if((anin0 - anin0_null) > 4080) pwm[0] = 0xff;
				else pwm[0] = ((anin0 - anin0_null) / 16);
			}
			else if(anin0_type == 2)
			{
				/**
				*	for joystick type potentiometer pwm value increase twice for same 
				*	wiper deflection in common type potentiometer  
				*	if wiper position is close to maximum deflection 
				* 	in either direction set pwm to 100%
				*/
				if(anin0 > anin0_null)
				{
					if((anin0 - anin0_null) > 2040) pwm[0] = 255;
					else pwm[0] = ((anin0 - anin0_null) / 8);					
				}
				else if(anin0 < anin0_null)
				{
					if((anin0_null - anin0) > 2040) pwm[1] = 255;
					else pwm[1] = ((anin0_null - anin0) / 8);				
				}
			}		
			/**
			*	convert multiplexer analog input 1 value
			*/
			SelectMuxInput1();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT);
			anin1 = HAL_ADC_GetValue(&hadc1);
			
			if(anin1_type == 1)
			{
				if(anin1 < anin1_null) pwm[anin0_type] = 0;
				else if((anin1 - anin1_null) > 4080) pwm[anin0_type] = 0xff;
				else pwm[anin0_type] = ((anin1 - anin1_null) / 16);
			}
			else if(anin1_type == 2)
			{
				if(anin1 > anin1_null)
				{
					if((anin1 - anin1_null) > 2040) pwm[anin0_type] = 255;
					else pwm[anin0_type] = ((anin1 - anin1_null) / 8);					
				}
				else if(anin1 < anin1_null)
				{
					if((anin1_null - anin1) > 2040) pwm[anin0_type + 1] = 255;
					else pwm[anin0_type + 1] = ((anin1_null - anin1) / 8);					
				}
			}
			/**
			*	convert multiplexer analog input 2 value
			*/
			SelectMuxInput2();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT);
			anin2 = HAL_ADC_GetValue(&hadc1);
			pwm_cnt = anin0_type + anin1_type;
			
			if(anin2_type == 1)
			{
				if(anin2 < anin2_null) pwm[pwm_cnt] = 0;
				else if((anin2 - anin2_null) > 4080) pwm[pwm_cnt] = 0xff;
				else pwm[pwm_cnt] = ((anin2 - anin2_null) / 16);
			}
			else if(anin2_type == 2)
			{
				if(anin2 > anin2_null)
				{
					if((anin2 - anin2_null) > 2040) pwm[pwm_cnt] = 255;
					else pwm[pwm_cnt] = ((anin2 - anin2_null) / 8);
				}
				else if(anin2 < anin2_null)
				{
					if((anin2_null - anin2) > 2040) pwm[pwm_cnt + 1] = 255;
					else pwm[pwm_cnt + 1] = ((anin2_null - anin2) / 8);
				}
			}		
			/**
			*	convert multiplexer analog input 3 value
			*/
			SelectMuxInput3();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT);
			anin3 = HAL_ADC_GetValue(&hadc1);
			pwm_cnt = anin0_type + anin1_type + anin2_type;
			
			if(anin3_type == 1)
			{
				if(anin3 < anin3_null) pwm[pwm_cnt] = 0;
				else if((anin3 - anin3_null) > 4080) pwm[pwm_cnt] = 0xff;
				else pwm[pwm_cnt] = ((anin3 - anin3_null) / 16);
			}
			else if(anin3_type == 2)
			{
				if(anin3 > anin3_null)
				{
					if((anin3 - anin3_null) > 2040) pwm[pwm_cnt] = 255;
					else pwm[pwm_cnt] = ((anin3 - anin3_null) / 8);
				}
				else if(anin3 < anin3_null)
				{
					if((anin3_null - anin3) > 2040) pwm[pwm_cnt + 1] = 255;
					else pwm[pwm_cnt + 1] = ((anin3_null - anin3)  / 8);
				}
			}		
			/**
			*	convert multiplexer analog input 4 value
			*/
			SelectMuxInput4();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT);
			anin4 = HAL_ADC_GetValue(&hadc1);
			pwm_cnt = anin0_type + anin1_type + anin2_type + anin3_type;
			
			if(anin4_type == 1)
			{
				if(anin4 < anin4_null) pwm[pwm_cnt] = 0;
				else if((anin4 - anin4_null) > 4080) pwm[pwm_cnt] = 0xff;
				else pwm[pwm_cnt] = ((anin4 - anin4_null) / 16);
			}
			else if(anin4_type == 2)
			{
				if(anin4 > anin4_null)
				{
					if((anin4 - anin4_null) > 2040) pwm[pwm_cnt] = 255;
					else pwm[pwm_cnt] = ((anin4 - anin4_null) / 8);
				}
				else if(anin4 < anin4_null)
				{
					if((anin4_null - anin4) > 2040) pwm[pwm_cnt + 1] = 255;
					else pwm[pwm_cnt + 1] = ((anin4_null - anin4) / 8);
				}
			}		
			/**
			*	convert multiplexer analog input 5 value
			*/
			SelectMuxInput5();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT);
			anin5 = HAL_ADC_GetValue(&hadc1);
			pwm_cnt = anin0_type + anin1_type + anin2_type + anin3_type + anin4_type;
			
			if(anin5_type == 1)
			{
				if(anin5 < anin5_null) pwm[pwm_cnt] = 0;
				else if((anin5 - anin5_null) > 4080) pwm[pwm_cnt] = 0xff;
				else pwm[pwm_cnt] = ((anin5 - anin5_null) / 16);
			}
			else if(anin5_type == 2)
			{
				if(anin5 > anin5_null)
				{
					if((anin5 - anin5_null) > 2040) pwm[pwm_cnt] = 255;
					else pwm[pwm_cnt] = ((anin5 - anin5_null) / 8);
				}
				else if(anin5 < anin5_null)
				{
					if((anin5_null - anin5) > 2040) pwm[pwm_cnt + 1] = 255;
					else pwm[pwm_cnt + 1] = ((anin5_null - anin5) / 8);
				}
			}		
			/**
			*	convert multiplexer analog input 6 value
			*/
			SelectMuxInput6();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT);
			anin6 = HAL_ADC_GetValue(&hadc1);
			pwm_cnt = anin0_type + anin1_type + anin2_type + anin3_type + anin4_type + anin5_type;
			
			if(anin6_type == 1)
			{
				if(anin6 < anin6_null) pwm[pwm_cnt] = 0;
				else if((anin6 - anin6_null) > 4080) pwm[pwm_cnt] = 0xff;
				else pwm[pwm_cnt] = ((anin6 - anin6_null) / 16);
			}
			else if(anin6_type == 2)
			{
				if(anin6 > anin6_null)
				{
					if((anin6 - anin6_null) > 2040) pwm[pwm_cnt] = 255;
					else pwm[pwm_cnt] = ((anin6 - anin6_null) / 8);
				}
				else if(anin6 < anin6_null)
				{
					if((anin6_null - anin6) > 2040) pwm[pwm_cnt + 1] = 255;
					else pwm[pwm_cnt + 1] = ((anin6_null - anin6) / 8);
				}
			}		
			/**
			*	convert multiplexer analog input 7 value
			*/
			SelectMuxInput7();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT);
			anin7 = HAL_ADC_GetValue(&hadc1);
			pwm_cnt = anin0_type + anin1_type + anin2_type + anin3_type + anin4_type + anin5_type + anin6_type;
			
			if(anin7_type == 1)
			{
				if(anin7 < anin7_null) pwm[pwm_cnt] = 0;
				else if((anin7 - anin7_null) > 4080) pwm[pwm_cnt] = 0xff;
				else pwm[pwm_cnt] = ((anin7 - anin7_null) / 16);
			}
			else if(anin7_type == 2)
			{
				if(anin7 > anin7_null)
				{
					if((anin7 - anin7_null) > 2040) pwm[pwm_cnt] = 255;
					else pwm[pwm_cnt] = ((anin7 - anin7_null) / 8);
				}
				else if(anin7 < anin7_null)
				{
					if((anin7_null - anin7) > 2040) pwm[pwm_cnt + 1] = 255;
					else pwm[pwm_cnt + 1] = ((anin7_null - anin7) / 8);
				}
			}
			/**
			*	wait for reference to stabilize after power on
			*/
			if(!IsANIN_TimerExpired()) ClearBuffer(pwm, PWM_BUFFER_SIZE);
			ANIN_ServiceState = ANIN_BATTERY_VOLTAGE;
			break;
		}
		/** ==========================================================================*/
		/**   			B A T T E R Y   V O L T A G E    C O N V E R S I O N   		  */
		/** ==========================================================================*/
		case ANIN_BATTERY_VOLTAGE:
		{
			sConfig.Channel = ADC_CHANNEL_1;
			sConfig.Rank = 1;
			sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
			if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
			{
				_Error_Handler(__FILE__, __LINE__);
			}
			/**
			*	set adc chanel 1 input and start conversion
			*	of supply battery voltage value
			*/
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT);
			vbatt = HAL_ADC_GetValue(&hadc1);
			
			if(vbatt <= (vbatt_min + ((vbatt_max - vbatt_min) / 10))) ERROR_BatteryLowLevelSet();
			else ERROR_BatteryLowLevelReset();
			
			if((vbatt <= vbatt_min) && !IsERROR_BatteryEmptyActiv())
			{
				ERROR_BatteryEmptySet();
				led_output = 0;
			}
			else if(vbatt > vbatt_min) ERROR_BatteryEmptyReset();
			
			ANIN_ServiceState = ANIN_MULTIPLEXER;
			break;
		}
		
		
		default:
		{
			ANIN_ServiceState = ANIN_INIT;
			break;
		}
	}
}


/******************************   END OF FILE  *******************************/
