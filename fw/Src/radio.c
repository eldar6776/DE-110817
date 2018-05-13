/**
 ******************************************************************************
 * File Name          : radio.c
 * Date               : 04/01/2018 5:24:19
 * Description        : radio transciever processing
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
 
/* Includes ------------------------------------------------------------------*/
#include "radio.h"
#include "main.h"
#include "common.h"
#include "anin.h"
#include "dio.h"


/* Typedef -------------------------------------------------------------------*/
/* Define --------------------------------------------------------------------*/
const uint8_t hc12_at_baud[] = 	 {"AT+B"};
const uint8_t hc12_at_chanel[] = {"AT+C"};
const uint8_t hc12_at_transp[] = {"AT+FU"};
const uint8_t hc12_at_power[] =  {"AT+P"};
const uint8_t hc12_at_param[] =  {"AT+R"};
const uint8_t hc12_at_setdb[] =  {"AT+U"};
const uint8_t hc12_at_fwver[] =  {"AT+V"};
const uint8_t hc12_at_sleep[] =  {"AT+SLEEP"};
const uint8_t hc12_at_default[] ={"AT+DEFAULT"};


/* Variable ------------------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

volatile uint8_t receive_pcnt;
volatile uint8_t transfer_error;
volatile uint8_t received_byte_cnt;
volatile uint32_t radio_timer;
volatile uint32_t radio_flags;
volatile uint32_t radio_cycle_timer;

uint8_t radio_cycle_cnt;
uint8_t radio_error_cnt;
uint8_t radio_link_qt;

uint8_t uart_buffer[UART_BUFFER_SIZE];
RADIO_ServiceStateTypeDef RADIO_ServiceState = RADIO_INIT;


/* Macro ---------------------------------------------------------------------*/
#define HC12_SetMode()		(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET));
#define HC12_RxTxMode()		(HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET));


/* Program code   ------------------------------------------------------------*/
void RADIO_Service(void)
{	
	if(IsERROR_BatteryEmptyActiv()) return;
	/** ==========================================================================*/
	/**    	R E C E I V E D		S T A T U S		P A C K E T  	S E R V I C E	  */
	/** ==========================================================================*/
	if(IsRADIO_RxStatusReady())
	{
		RADIO_RxStatusReset();
		
		if(++radio_cycle_cnt > 7)
		{			
			radio_link_qt = (radio_cycle_cnt - radio_error_cnt);
			radio_cycle_cnt = 0;
			radio_error_cnt = 0;
		}		
		
		ERROR_RadioLinkFailureReset();			
		led_output = uart_buffer[0];
		
		if(uart_buffer[1] & (1 << 0)) ERROR_PwmControllerFailureSet();
		else ERROR_PwmControllerFailureReset();
		
		if(uart_buffer[1] & (1 << 1)) ERROR_PwmOverloadWarningSet();
		else ERROR_PwmOverloadWarningReset();
		
		if(uart_buffer[1] & (1 << 2)) ERROR_PwmOverloadProtectionSet();
		else ERROR_PwmOverloadProtectionReset();
		
		if(uart_buffer[1] & (1 << 3)) ERROR_TemperatureSensorSet();
		else ERROR_TemperatureSensorReset();
		
		if(uart_buffer[1] & (1 << 5)) ERROR_ReceiverPowerFailureSet();
		else ERROR_ReceiverPowerFailureReset();
		
		RADIO_ServiceState = RADIO_INIT;
	}

		
	switch(RADIO_ServiceState)
	{
		/** ==========================================================================*/
		/**     	R A D I O      M O D U L E		I N I T I A L I Z A T I O N		  */
		/** ==========================================================================*/
		case RADIO_INIT:
		{
			HC12_RxTxMode();
			RADIO_StopCycleTimer();
			RADIO_RxStatusReset();
			RADIO_TxStatusReset();
			receive_pcnt = 0;
			received_byte_cnt = 0;
			ClearBuffer(uart_buffer, UART_BUFFER_SIZE);
			/**
			*   clear uart rx busy flag if previous interrupt receiving
			*	function is disrupted before successfuly completed
			*/
			if (huart1.RxState == HAL_UART_STATE_BUSY_RX)
			{
				__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
				huart1.RxState = HAL_UART_STATE_READY;
				huart1.gState = HAL_UART_STATE_READY;
			}
				
			RADIO_StartCycleTimer(RADIO_CYCLE_PERIOD);
			RADIO_ServiceState = RADIO_SEND_STATUS;	
			break;
		}		
		/** ==========================================================================*/
		/**    	S E N D 	S T A T U S	 	T O 	C O M M A N D		U N I T 	  */
		/** ==========================================================================*/
		case RADIO_SEND_STATUS:
		{
			if(!IsRADIO_CycleTImerExpired()) break;
			/**
			*   clear uart rx busy flag if previous interrupt receiving
			*	function is disrupted before successfuly completed
			*/
			if (huart1.RxState == HAL_UART_STATE_BUSY_RX)
			{
				__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
				huart1.RxState = HAL_UART_STATE_READY;
				huart1.gState = HAL_UART_STATE_READY;
			}
			ClearBuffer(uart_buffer, UART_BUFFER_SIZE);
			uart_buffer[0] =   SOH;
			uart_buffer[1] =  (din_state & 0xff);
			uart_buffer[2] =  (din_state >> 8);
			uart_buffer[3] =  pwm[0];
			uart_buffer[4] =  pwm[1];
			uart_buffer[5] =  pwm[2];
			uart_buffer[6] =  pwm[3];
			uart_buffer[7] =  pwm[4];
			uart_buffer[8] =  pwm[5];
			uart_buffer[9] =  pwm[6];
			uart_buffer[10] = pwm[7];
			uart_buffer[11] = pwm[8];
			uart_buffer[12] = pwm[9];
			uart_buffer[13] = pwm[10];
			uart_buffer[14] = pwm[11];
			uart_buffer[15] = pwm[12];
			uart_buffer[16] = pwm[13];
			uart_buffer[17] = pwm[14];
			uart_buffer[18] = pwm[15];
			uart_buffer[19] = CalcCRC(&uart_buffer[1], 18);
			uart_buffer[20] = EOT;
			
			HAL_UART_Transmit(&huart1, uart_buffer, 21, 40);
			while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) continue;	
			ClearBuffer(uart_buffer, UART_BUFFER_SIZE);		
			
			if(HAL_UART_Receive_IT(&huart1, uart_buffer, UART_BUFFER_SIZE) != HAL_OK)
			{
				_Error_Handler(__FILE__, __LINE__);
				
			}
			
			RADIO_StartCycleTimer(RADIO_CYCLE_PERIOD);
			RADIO_ServiceState = RADIO_RECEIVE_STATUS;
			break;
		}		
		/** ==========================================================================*/
		/**    	S T A T U S		R E C E I V I N G	 T I M E O U T 		C H E C K 	  */
		/** ==========================================================================*/
		case RADIO_RECEIVE_STATUS:
		{
			if(IsRADIO_CycleTImerExpired()) 
			{
				++radio_cycle_cnt;
				++radio_error_cnt;
			
				if(radio_cycle_cnt > 7)
				{			
					radio_link_qt = (radio_cycle_cnt - radio_error_cnt);
					
					if(!IsERROR_RadioLinkFailureActiv() && (radio_link_qt == 0))
					{
						ERROR_RadioLinkFailureSet();
						led_output = 0;
					}
					
					radio_cycle_cnt = 0;
					radio_error_cnt = 0;
				}
				
				RADIO_StartCycleTimer(RADIO_CYCLE_PERIOD);	
				RADIO_ServiceState = RADIO_INIT;				
			}
			break;
		}
		
		
		default:
		{
			RADIO_ServiceState = RADIO_INIT;
			break;
		}
	}
}


/******************************   END OF FILE  *******************************/
