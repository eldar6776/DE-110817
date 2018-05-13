/**
 ******************************************************************************
 * File Name          : dio.c
 * Date               : 04/01/2018 5:22:19
 * Description        : digital input / output processing
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
 
/* Includes ------------------------------------------------------------------*/
#include "dio.h"
#include "anin.h"
#include "radio.h"
#include "main.h"
#include "common.h"

/* Typedef -------------------------------------------------------------------*/
/* Define --------------------------------------------------------------------*/
/* Variable ------------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

volatile uint32_t ctrl_sw_pause;
volatile uint8_t error_signal_flags;
volatile uint16_t error_signal_timer;

uint8_t error_signal_cnt;
uint32_t ctrl_sw_timer;
uint32_t led_timer;
uint8_t ctrl_sw_flag;
uint8_t led_output;
uint8_t old_led_output;
uint16_t din_state;


/* Macro ---------------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void LED_ErrorSignal(void);
void LED_SetAllOn(void);
void LED_SetAllOff(void);


/* Program code   ------------------------------------------------------------*/
void DIO_Service(void)
{
	uint8_t t;
	static uint8_t dio_pcnt = 0;
	static uint32_t dio_tmr = 0;
	
	static enum
	{
		DIO_INIT			= 0x00,
		DIO_SERVICE			= 0x01,
		BATTERY_MONITOR		= 0x02,
		RADIO_QT_MONITOR	= 0x03,
		NULL_ANIN			= 0x04		
		
	} DIN_ServiceState = DIO_INIT;
	
	static enum
	{
		SETUP_IDLE			= 0x00,
		SETUP_ACTIV			= 0x01,
		SET_ANIN_TYPE		= 0x02,
		SET_VBATT_EMPTY		= 0x03,
		SET_VBATT_FULL		= 0x04,
		SET_PWM_0_15_FREQ	= 0x05,
		SET_PWM_16_31_FREQ	= 0x06		
		
	} SETUP_State = SETUP_IDLE;
	/**
	*	check control switch state
	*/
	if(ControlSwitchState() && IsCTRL_SW_PauseTimerExpired())
	{
		if(!IsCTRL_SW_OldStatePressed())
		{
			CTRL_SW_OldStatePressed();
			ctrl_sw_timer = HAL_GetTick();
			led_output = 0;
			old_led_output = 0xff;
			CTRL_SW_ShortPressed();
			CTRL_SW_StartPauseTimer(CTRL_SW_PAUSE_TIME);
		}
		else if(!IsCTRL_SW_OldStateLongPressed() && (HAL_GetTick() > (ctrl_sw_timer + CTRL_SW_LONG_PRESS_TIME)))
		{
			CTRL_SW_OldStateLongPressed();
			led_output = 0;
			old_led_output = 0xff;
			CTRL_SW_LongPressed();
			CTRL_SW_StartPauseTimer(CTRL_SW_PAUSE_TIME);
		}
		
	}
	else if(!ControlSwitchState() && IsCTRL_SW_PauseTimerExpired())
	{
		CTRL_SW_OldStateReleased();
		CTRL_SW_OldStateLongReleased();
	}	
	/**
	*	select activ dio service 
	*	to read digital input state or to run useful functions 
	*/	
	switch(DIN_ServiceState)
	{
		/** ==========================================================================*/
		/**    		D I G I T A L    I / O   I N I T I A L I Z A T I O N			  */
		/** ==========================================================================*/
		case DIO_INIT:
		{
			/**
			*	check control taster state for setup mode request
			*/
			if(ControlSwitchState()) SETUP_State = SETUP_ACTIV;			
			DIN_ServiceState = DIO_SERVICE;
			old_led_output = 0xff;
			break;
		}		
		/** ==========================================================================*/
		/**  D I G I T A L    I N P U T  &  L E D   S I G N A L   P R O C E S S I N G */
		/** ==========================================================================*/		
		case DIO_SERVICE:
		{
			din_state = 0;
			/**
			*	set inputs of multiplexers CD4051 to input X0
			*	and read digital input 0 and 8
			*/
			SelectMuxInput0();
			Delay(DIO_SETTLING_TIME);
			if(GPIOB->IDR & 0x0002)	din_state |= 0x0001;
			if(GPIOB->IDR & 0x0004)	din_state |= 0x0100;
			/**
			*	set inputs of multiplexers CD4051 to input X1
			*	and read digital input 1 and 9
			*/
			SelectMuxInput1();
			Delay(DIO_SETTLING_TIME);
			if(GPIOB->IDR & 0x0002)	din_state |= 0x0002;
			if(GPIOB->IDR & 0x0004)	din_state |= 0x0200;
			/**
			*	set inputs of multiplexers CD4051 to input X2
			*	and read digital input 2 and 10
			*/
			SelectMuxInput2();
			Delay(DIO_SETTLING_TIME);
			if(GPIOB->IDR & 0x0002)	din_state |= 0x0004;
			if(GPIOB->IDR & 0x0004)	din_state |= 0x0400;
			/**
			*	set inputs of multiplexers CD4051 to input X3
			*	and read digital input 3 and 11
			*/
			SelectMuxInput3();
			Delay(DIO_SETTLING_TIME);
			if(GPIOB->IDR & 0x0002)	din_state |= 0x0008;
			if(GPIOB->IDR & 0x0004)	din_state |= 0x0800;
			/**
			*	set inputs of multiplexers CD4051 to input X4
			*	and read digital input 4 and 12
			*/
			SelectMuxInput4();
			Delay(DIO_SETTLING_TIME);
			if(GPIOB->IDR & 0x0002)	din_state |= 0x0010;
			if(GPIOB->IDR & 0x0004)	din_state |= 0x1000;
			/**
			*	set inputs of multiplexers CD4051 to input X5
			*	and read digital input 5 and 13
			*/
			SelectMuxInput5();
			Delay(DIO_SETTLING_TIME);
			if(GPIOB->IDR & 0x0002)	din_state |= 0x0020;
			if(GPIOB->IDR & 0x0004)	din_state |= 0x2000;
			/**
			*	set inputs of multiplexers CD4051 to input X6
			*	and read digital input 6 and 14
			*/
			SelectMuxInput6();
			Delay(DIO_SETTLING_TIME);
			if(GPIOB->IDR & 0x0002)	din_state |= 0x0040;
			if(GPIOB->IDR & 0x0004)	din_state |= 0x4000;
			/**
			*	set inputs of multiplexers CD4051 to input X7
			*	and read digital input 7 and 15
			*/
			SelectMuxInput7();
			Delay(DIO_SETTLING_TIME);
			if(GPIOB->IDR & 0x0002)	din_state |= 0x0080;
			if(GPIOB->IDR & 0x0004)	din_state |= 0x8000;
			/**
			*	status LED output is set with receiver din state in rs485.c 
			*	here we combine error signal with status LED output
			*/
			LED_ErrorSignal();
			/**
			*	if setup not activ, check control switch event
			*/
			if(IsCTRL_SW_ShortPressed())
			{
				CTRL_SW_ShortReleased();
				dio_tmr = HAL_GetTick();
				DIN_ServiceState = BATTERY_MONITOR;
			}			
			break;
		}
		/** ==========================================================================*/
		/**  			B A T T E R Y    V O L T A G E     M O N I T O R  		      */
		/** ==========================================================================*/
		case BATTERY_MONITOR:
		{
			if(IsCTRL_SW_LongPressed())
			{
				CTRL_SW_LongReleased();
				if (SETUP_State == SETUP_IDLE) DIN_ServiceState = NULL_ANIN;
			}			
			else if(HAL_GetTick() > (dio_tmr + BATTERY_MONITOR_TIME))
			{
				led_output = 0;
				old_led_output = 0xff;
				DIN_ServiceState = DIO_SERVICE;
			}
			else if(IsCTRL_SW_ShortPressed())
			{
				CTRL_SW_ShortReleased();
				DIN_ServiceState = RADIO_QT_MONITOR;
			}
			else
			{
				/**
				*	set status LED output 
				*/
				if(vbatt <= vbatt_min) led_output = 0;
				else if(vbatt >= vbatt_max) led_output = 8;
				else led_output = ((vbatt - vbatt_min) / ((vbatt_max - vbatt_min) / 8));
				
				if(old_led_output != led_output)
				{
					old_led_output = led_output;
					LED_SetAllOff();
					
					if(led_output > 0) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
					if(led_output > 1) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
					if(led_output > 2) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
					if(led_output > 3) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET);
					if(led_output > 4) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
					if(led_output > 5) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
					if(led_output > 6) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
					if(led_output > 7) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
				}
			}
			break;
		}
		/** ==========================================================================*/
		/**  		R A D I O    L I N K     Q U A L I T Y     M O N I T O R  		  */
		/** ==========================================================================*/
		case RADIO_QT_MONITOR:
		{
			if(IsCTRL_SW_ShortPressed())
			{
				CTRL_SW_ShortReleased();
				DIN_ServiceState = DIO_SERVICE;
			}
			else
			{
				led_output = radio_link_qt;
				 
				if(old_led_output != led_output)
				{
					old_led_output = led_output;
					LED_SetAllOff();
					
					if(led_output > 0) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
					if(led_output > 1) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
					if(led_output > 2) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
					if(led_output > 3) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET);
					if(led_output > 4) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
					if(led_output > 5) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
					if(led_output > 6) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
					if(led_output > 7) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
				}	
			}
			break;
		}
		/** ==========================================================================*/
		/**  	S E T    P O T E N T I O M E T E R    N U L L    P O S I T I O N      */
		/** ==========================================================================*/
		case NULL_ANIN:
		{
			if(dio_pcnt == 0)
			{
				LED_SetAllOff();				
				LED_StartTimer(1000);
				++dio_pcnt;
			}
			else if((dio_pcnt == 1) && IsLED_TimerExpired())
			{
				LED_SetAllOn();				
				LED_StartTimer(100);
				++dio_pcnt;
			}
			else if((dio_pcnt == 2) && IsLED_TimerExpired())
			{
				LED_SetAllOff();				
				LED_StartTimer(1000);
				++dio_pcnt;
			}
			else if((dio_pcnt == 3) && IsLED_TimerExpired())
			{
				anin0_null = anin0;
				anin1_null = anin1;
				anin2_null = anin2;
				anin3_null = anin3;
				anin4_null = anin4;
				anin5_null = anin5;
				anin6_null = anin6;
				anin7_null = anin7;
				
				eeprom_buffer[0] = 0x00;
				eeprom_buffer[1] = EE_ANIN0_NULL_OFFSET_ADDRESS;
				eeprom_buffer[2] = (anin0_null >> 8);
				eeprom_buffer[3] = (anin0_null & 0xff);
				HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
				HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
				
				eeprom_buffer[0] = 0x00;
				eeprom_buffer[1] = EE_ANIN1_NULL_OFFSET_ADDRESS;
				eeprom_buffer[2] = (anin1_null >> 8);
				eeprom_buffer[3] = (anin1_null & 0xff);
				HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
				HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
				
				eeprom_buffer[0] = 0x00;
				eeprom_buffer[1] = EE_ANIN2_NULL_OFFSET_ADDRESS;
				eeprom_buffer[2] = (anin2_null >> 8);
				eeprom_buffer[3] = (anin2_null & 0xff);
				HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
				HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
				
				eeprom_buffer[0] = 0x00;
				eeprom_buffer[1] = EE_ANIN3_NULL_OFFSET_ADDRESS;
				eeprom_buffer[2] = (anin3_null >> 8);
				eeprom_buffer[3] = (anin3_null & 0xff);
				HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
				HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
				
				eeprom_buffer[0] = 0x00;
				eeprom_buffer[1] = EE_ANIN4_NULL_OFFSET_ADDRESS;
				eeprom_buffer[2] = (anin4_null >> 8);
				eeprom_buffer[3] = (anin4_null & 0xff);
				HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
				HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
				
				eeprom_buffer[0] = 0x00;
				eeprom_buffer[1] = EE_ANIN5_NULL_OFFSET_ADDRESS;
				eeprom_buffer[2] = (anin5_null >> 8);
				eeprom_buffer[3] = (anin5_null & 0xff);
				HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
				HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
				
				eeprom_buffer[0] = 0x00;
				eeprom_buffer[1] = EE_ANIN6_NULL_OFFSET_ADDRESS;
				eeprom_buffer[2] = (anin6_null >> 8);
				eeprom_buffer[3] = (anin6_null & 0xff);
				HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
				HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
				
				eeprom_buffer[0] = 0x00;
				eeprom_buffer[1] = EE_ANIN7_NULL_OFFSET_ADDRESS;
				eeprom_buffer[2] = (anin7_null >> 8);
				eeprom_buffer[3] = (anin7_null & 0xff);
				HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
				HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);
				
				dio_pcnt = 0;
				DIN_ServiceState = DIO_SERVICE;
			}			
			break;
		}
		
		
		default:
		{
			DIN_ServiceState = DIO_INIT;
			break;
		}
	}
	/**
	*	select activ mode and after control switch short press event,
	*	save changed value to eeprom and put setup mode to idle state
	*/	
	switch (SETUP_State)
	{
		case SETUP_IDLE:
		{
			break;
		}
		/** ==========================================================================*/
		/**  		C H E C K    A N D    S E L E C T    S E T U P  	M O D E	 	  */
		/** ==========================================================================*/
		case SETUP_ACTIV:
		{
			if(++dio_pcnt < 100) break;			
						
			if(IsDIN0_OnlyActiv()) SETUP_State = SET_ANIN_TYPE;
			else if(IsDIN1_OnlyActiv()) SETUP_State = SET_VBATT_EMPTY;
			else if(IsDIN2_OnlyActiv()) SETUP_State = SET_VBATT_FULL;
			else if(IsDIN3_OnlyActiv()) SETUP_State = SET_PWM_0_15_FREQ;
			else if(IsDIN4_OnlyActiv()) SETUP_State = SET_PWM_16_31_FREQ;
			else SETUP_State = SETUP_IDLE;
			
			dio_pcnt = 0;
			break;
		}
		/** ==========================================================================*/
		/**  		S E T    A N A L O G    I N P U T   S I G N A L     T Y P E       */
		/** ==========================================================================*/
		case SET_ANIN_TYPE:
		{
			if(dio_pcnt == 0)
			{
				LED_SetAllOff();
				
				for(t = 0; t < 5; t++)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
					HAL_Delay(50);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_Delay(100);
				}								
				
				++dio_pcnt;
			}
			else if(dio_pcnt == 1)
			{
				if(anin0 > 4000) 
				{
					anin0_set_type = 2;
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
				}
				else if(anin0 < 100) 
				{
					anin0_set_type = 1;
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
				}
				else anin0_set_type = 0;
				
				if(anin1 > 4000) 
				{
					anin1_set_type = 2;
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
				}
				else if(anin1 < 100) 
				{
					anin1_set_type = 1;
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
				}
				else anin1_set_type = 0;
				
				if(anin2 > 4000)
				{
					anin2_set_type = 2;
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
				}
				else if(anin2 < 100) 
				{
					anin2_set_type = 1;
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
				}
				else anin2_set_type = 0;
				
				if(anin3 > 4000) 
				{
					anin3_set_type = 2;
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET);
				}
				else if(anin3 < 100) 
				{
					anin3_set_type = 1;
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET);
				}
				else anin3_set_type = 0;
				
				if(anin4 > 4000) 
				{
					anin4_set_type = 2;
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
				}
				else if(anin4 < 100) 
				{
					anin4_set_type = 1;
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
				}
				else anin4_set_type = 0;
				
				if(anin5 > 4000)
				{ 
					anin5_set_type = 2;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
				}
				else if(anin5 < 100) 
				{
					anin5_set_type = 1;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
				}
				else anin5_set_type = 0;
				
				if(anin6 > 4000) 
				{
					anin6_set_type = 2;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
				}
				else if(anin6 < 100)
				{ 
					anin6_set_type = 1;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
				}
				else anin6_set_type = 0;
				
				if(anin7 > 4000)
				{ 
					anin7_set_type = 2;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
				}
				else if(anin7 < 100) 
				{
					anin7_set_type = 1;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
				}
				else anin7_set_type = 0;
				
				if(IsCTRL_SW_ShortPressed())
				{
					CTRL_SW_ShortReleased();
					++dio_pcnt;
				}
			}
			else if(dio_pcnt == 2)
			{
				LED_SetAllOff();
				
				for(t = 0; t < 5; t++)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
					HAL_Delay(50);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_Delay(100);
				}			
				
				if(anin0_set_type)
				{
					eeprom_buffer[0] = 0x00;
					eeprom_buffer[1] = EE_ANIN0_TYPE_ADDRESS;			
					eeprom_buffer[2] = anin0_set_type;				
					HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
					HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);					
				}
				
				if(anin1_set_type)
				{
					eeprom_buffer[0] = 0x00;
					eeprom_buffer[1] = EE_ANIN1_TYPE_ADDRESS;			
					eeprom_buffer[2] = anin1_set_type;				
					HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
					HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);					
				}
				
				if(anin2_set_type)
				{
					eeprom_buffer[0] = 0x00;
					eeprom_buffer[1] = EE_ANIN2_TYPE_ADDRESS;			
					eeprom_buffer[2] = anin2_set_type;				
					HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
					HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);					
				}
			
				if(anin3_set_type)
				{
					eeprom_buffer[0] = 0x00;
					eeprom_buffer[1] = EE_ANIN3_TYPE_ADDRESS;			
					eeprom_buffer[2] = anin3_set_type;				
					HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
					HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);					
				}
				
				if(anin4_set_type)
				{
					eeprom_buffer[0] = 0x00;
					eeprom_buffer[1] = EE_ANIN4_TYPE_ADDRESS;			
					eeprom_buffer[2] = anin4_set_type;				
					HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
					HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);					
				}
				
				if(anin5_set_type)
				{
					eeprom_buffer[0] = 0x00;
					eeprom_buffer[1] = EE_ANIN5_TYPE_ADDRESS;			
					eeprom_buffer[2] = anin5_set_type;				
					HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
					HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);					
				}
				
				if(anin6_set_type)
				{
					eeprom_buffer[0] = 0x00;
					eeprom_buffer[1] = EE_ANIN6_TYPE_ADDRESS;			
					eeprom_buffer[2] = anin6_set_type;				
					HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
					HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);					
				}
				
				if(anin7_set_type)
				{
					eeprom_buffer[0] = 0x00;
					eeprom_buffer[1] = EE_ANIN7_TYPE_ADDRESS;			
					eeprom_buffer[2] = anin7_set_type;				
					HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 3, EE_TIMEOUT);
					HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);					
				}
				
				dio_pcnt = 0;
				SETUP_State = SETUP_IDLE;
			}
			break;
		}
		/** ==========================================================================*/
		/**  S E T    S U P P L Y   B A T T E R Y    E M P T Y    V O L T A G E 	  */
		/** ==========================================================================*/
		case SET_VBATT_EMPTY:
		{
			if(dio_pcnt == 0)
			{
				LED_SetAllOff();
				
				for(t = 0; t < 5; t++)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
					HAL_Delay(50);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
					HAL_Delay(100);
				}								
				
				++dio_pcnt;
			}
			else if(dio_pcnt == 1)
			{
				if(IsCTRL_SW_ShortPressed())
				{
					CTRL_SW_ShortReleased();
					++dio_pcnt;
				}
			}
			else if(dio_pcnt == 2)
			{
				LED_SetAllOff();
				
				for(t = 0; t < 5; t++)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
					HAL_Delay(50);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
					HAL_Delay(100);
				}			
				
				eeprom_buffer[0] = 0x00;
				eeprom_buffer[1] = EE_EMPTY_BATTERY_VOLTAGE_ADDRESS;
				eeprom_buffer[2] = (vbatt >> 8);
				eeprom_buffer[3] = (vbatt & 0xff);
				HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
				HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);				
				dio_pcnt = 0;
				SETUP_State = SETUP_IDLE;
			}
			break;
		}
		/** ==========================================================================*/
		/**  S E T    S U P P L Y   B A T T E R Y     F U L L   	V O L T A G E 	  */
		/** ==========================================================================*/
		case SET_VBATT_FULL:
		{
			if(dio_pcnt == 0)
			{
				LED_SetAllOff();
				
				for(t = 0; t < 5; t++)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
					HAL_Delay(50);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
					HAL_Delay(100);
				}								
				
				++dio_pcnt;
			}
			else if(dio_pcnt == 1)
			{
				if(IsCTRL_SW_ShortPressed())
				{
					CTRL_SW_ShortReleased();
					++dio_pcnt;
				}
			}
			else if(dio_pcnt == 2)
			{
				LED_SetAllOff();
				
				for(t = 0; t < 5; t++)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
					HAL_Delay(50);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
					HAL_Delay(100);
				}			
				
				eeprom_buffer[0] = 0x00;
				eeprom_buffer[1] = EE_FULL_BATTERY_VOLTAGE_ADDRESS;
				eeprom_buffer[2] = (vbatt >> 8);
				eeprom_buffer[3] = (vbatt & 0xff);
				HAL_I2C_Master_Transmit(&hi2c1, EE_WRITE, eeprom_buffer, 4, EE_TIMEOUT);
				HAL_I2C_IsDeviceReady(&hi2c1, EE_READ, EE_TRIALS, EE_WRITE_DELAY);				
				dio_pcnt = 0;
				SETUP_State = SETUP_IDLE;
			}
			break;
		}
		/** ==========================================================================*/
		/**    S E T    P W M    0   ~   1 5  	O U T P U T    F R E Q U E N C Y      */
		/** ==========================================================================*/
		case SET_PWM_0_15_FREQ:
		{
			if(dio_pcnt == 0)
			{
				LED_SetAllOff();
				
				for(t = 0; t < 5; t++)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET);
					HAL_Delay(50);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET);
					HAL_Delay(100);
				}								
				
				++dio_pcnt;
			}
			else if(dio_pcnt == 1)
			{
				led_output = (anin0 / 500);
				 
				if(old_led_output != led_output)
				{
					old_led_output = led_output;
					LED_SetAllOff();
					
					if(led_output > 0) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
					if(led_output > 1) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
					if(led_output > 2) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
					if(led_output > 3) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET);
					if(led_output > 4) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
					if(led_output > 5) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
					if(led_output > 6) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
					if(led_output > 7) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
				}	
				
				if(IsCTRL_SW_ShortPressed())
				{
					CTRL_SW_ShortReleased();
					++dio_pcnt;
				}
			}
			else if(dio_pcnt == 2)
			{
				LED_SetAllOff();
				
				for(t = 0; t < 5; t++)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET);
					HAL_Delay(50);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET);
					HAL_Delay(100);
				}			
							
				dio_pcnt = 0;
				SETUP_State = SETUP_IDLE;
			}			
			break;
		}
		/** ==========================================================================*/
		/**    S E T    P W M    1 6  ~	 3 1  	O U T P U T    F R E Q U E N C Y      */
		/** ==========================================================================*/
		case SET_PWM_16_31_FREQ:
		{
			if(dio_pcnt == 0)
			{
				LED_SetAllOff();
				
				for(t = 0; t < 5; t++)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
					HAL_Delay(50);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
					HAL_Delay(100);
				}								
				
				++dio_pcnt;
			}
			else if(dio_pcnt == 1)
			{
				led_output = (anin0 / 500);
				 
				if(old_led_output != led_output)
				{
					old_led_output = led_output;
					LED_SetAllOff();
					
					if(led_output > 0) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
					if(led_output > 1) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
					if(led_output > 2) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
					if(led_output > 3) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET);
					if(led_output > 4) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
					if(led_output > 5) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
					if(led_output > 6) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
					if(led_output > 7) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
				}	
				
				if(IsCTRL_SW_ShortPressed())
				{
					CTRL_SW_ShortReleased();
					++dio_pcnt;
				}
			}
			else if(dio_pcnt == 2)
			{
				LED_SetAllOff();
				
				for(t = 0; t < 5; t++)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
					HAL_Delay(50);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
					HAL_Delay(100);
				}			
							
				dio_pcnt = 0;
				SETUP_State = SETUP_IDLE;
			}
			break;
		}
	}
}

void LED_ErrorSignal(void)
{
	if(IsERROR_TimerExpired())
	{
		++error_signal_cnt;
		
		if(IsERROR_BatteryLowLevelActiv()) 		 	led_output ^= (1 << 0);
		if(IsERROR_BatteryEmptyActiv())				led_output ^= (1 << 1);					
		if(IsERROR_PwmControllerFailureActiv())  	led_output ^= (1 << 2);
		if(IsERROR_PwmOverloadWarningActiv()) 	 	led_output ^= (1 << 3);
		if(IsERROR_PwmOverloadProtectionActiv()) 	led_output ^= (1 << 4);
		if(IsERROR_TemperatureSensorActiv()) 	 	led_output ^= (1 << 5);					
		if(IsERROR_RadioLinkFailureActiv()) 	 	led_output ^= (1 << 6);			
		if(IsERROR_ReceiverPowerFailureActiv())  	led_output ^= (1 << 7);
		
		if(error_signal_cnt >= (ERROR_SIGNAL_COUNT * 2))
		{
			error_signal_cnt = 0;
			ERROR_StartTimer(ERROR_SIGNAL_CYCLE_TIME);
		}
		else if(error_signal_cnt & 0x01) ERROR_StartTimer(ERROR_SIGNAL_ACTIV_TIME);
		else ERROR_StartTimer(ERROR_SIGNAL_PAUSE_TIME);
	}
	/**
	*	set status LED output 
	*/
	if(old_led_output != led_output)
	{
		old_led_output = led_output;
		LED_SetAllOff();
	}
	/*
	*	if battery empty signal only LED 2
	*/
	if(led_output & 0x02) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);

	if(!IsERROR_BatteryEmptyActiv())
	{
		if(led_output & 0x01) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);				
		if(led_output & 0x04) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
		if(led_output & 0x08) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET);
		if(led_output & 0x10) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
		if(led_output & 0x20) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
		if(led_output & 0x40) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
		if(led_output & 0x80) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);				
	}
}

void LED_SetAllOn(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);	
}

void LED_SetAllOff(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
}


/******************************   END OF FILE  ********************************/
