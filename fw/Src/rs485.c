/**
 ******************************************************************************
 * File Name          : rs485.c
 * Date               : 28/02/2016 23:16:19
 * Description        : rs485 communication modul
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
#include "rs485.h"
#include "main.h"
#include "common.h"
#include "anin.h"
#include "dio.h"

/* Typedef -------------------------------------------------------------------*/
RS485_ServiceStateTypeDef RS485_ServiceState = RS485_INIT;

/* Define --------------------------------------------------------------------*/
/* Variable ------------------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

uint8_t usartBuffer[UART_BUFFER_SIZE];

volatile uint8_t receive_pcnt;
volatile uint8_t transfer_error;
volatile uint8_t received_byte_cnt;

volatile uint32_t rs485_timer;
volatile uint32_t rs485_flags;
volatile uint32_t rs485_cycle_timer;

/* Macro ---------------------------------------------------------------------*/
/* Program code   ------------------------------------------------------------*/
void RS485_Service(void)
{

//	if(IsRS485_TxStatusReady()) RS485_ServiceState = RS485_SEND_STATUS;
		
	
	switch(RS485_ServiceState)
	{
		/** ==========================================================================*/
		/**     	R S 4 8 5   	B U S     	I N I T I A L I Z A T I O N		  	  */
		/** ==========================================================================*/
		case RS485_INIT:
		{
			ClearBuffer(usartBuffer, UART_BUFFER_SIZE);
			RS485_ResetTimer();
			RS485_RxStatusReset();
			RS485_TxStatusReset();
			RS485_DirRx();
			receive_pcnt = 0;
			received_byte_cnt = 0;
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
			/**
			*   start usart receiving in interrupt mode
			*   to get packet header for address checking
			*/
			if(HAL_UART_Receive_IT(&huart1, usartBuffer, UART_BUFFER_SIZE) != HAL_OK)
			{
				_Error_Handler(__FILE__, __LINE__);				
			}
			else
			{
				RS485_ServiceState = RS485_SEND_STATUS;
			}
			break;
		}		
		/** ==========================================================================*/
		/**     	S E N D     U S E R	   C O M M A N D 	T O	  R E C E I V E R	  */
		/** ==========================================================================*/		
		case RS485_SEND_STATUS:
		{
			if(!IsRS485_CycleTimerExpired()) break;
			
			if (huart1.RxState == HAL_UART_STATE_BUSY_RX)
			{
				__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
				huart1.RxState = HAL_UART_STATE_READY;
				huart1.gState = HAL_UART_STATE_READY;
			}
			
			receive_pcnt = 0;
			received_byte_cnt = 0;
			RS485_RxStatusReset();
			RS485_TxStatusReset();			
			
			ClearBuffer(usartBuffer, UART_BUFFER_SIZE);
			usartBuffer[0] =  SOH;
			usartBuffer[1] =  (din_state >> 8);
			usartBuffer[2] =  (din_state & 0xff);
			usartBuffer[3] =  (anin7 / 16);
			usartBuffer[4] =  (anin6 / 16);
			usartBuffer[5] =  (anin5 / 16);
			usartBuffer[6] =  (anin4 / 16);
			usartBuffer[7] =  (anin3 / 16);
			usartBuffer[8] =  (anin2 / 16);
			usartBuffer[9] =  (anin1 / 16);
			usartBuffer[10] = (anin0 / 16);
			usartBuffer[11] = (vbatt / 16);
			usartBuffer[12] = CalcCRC(&usartBuffer[1], 10);
			usartBuffer[13] = EOT;
			RS485_DirTx();
			HAL_UART_Transmit(&huart1, usartBuffer, 14, 50);
			while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) continue; 
			RS485_DirRx();		
			ClearBuffer(usartBuffer, UART_BUFFER_SIZE);		
			
			if(HAL_UART_Receive_IT(&huart1, usartBuffer, UART_BUFFER_SIZE) != HAL_OK)
			{
				_Error_Handler(__FILE__, __LINE__);
				
			}
			
			RS485_ServiceState = RS485_RECEIVE_STATUS;
			RS485_StartTimer(PACKET_TRANSFER_TIMEOUT);
			RS485_StartCycleTimer(RS485_CYCLE_TIME);
			break;
		}
		/** ==========================================================================*/
		/**     	U P D A T E		L E D 	S I G N A L 	S T A T U S				  */
		/** ==========================================================================*/	
		case RS485_RECEIVE_STATUS:
		{
			if(IsRS485_RxStatusReady())
			{
				RS485_RxStatusReset();
				
				if(usartBuffer[0] == ACK)
				{
					led_output = usartBuffer[2];
					
					if(usartBuffer[3] & (1 << 0)) ERROR_PwmControllerFailureSet();
					else ERROR_PwmControllerFailureReset();
					
					if(usartBuffer[3] & (1 << 1)) ERROR_PwmOverloadWarningSet();
					else ERROR_PwmOverloadWarningReset();
					
					if(usartBuffer[3] & (1 << 2)) ERROR_PwmOverloadProtectionSet();
					else ERROR_PwmOverloadProtectionReset();
					
					if(usartBuffer[3] & (1 << 3)) ERROR_TemperatureSensorSet();
					else ERROR_TemperatureSensorReset();
					
					if(usartBuffer[3] & (1 << 4)) ERROR_ReceiverPowerFailureSet();
					else ERROR_ReceiverPowerFailureReset();
					
//					if(usartBuffer[3] & (1 << 5))
//					if(usartBuffer[3] & (1 << 6))
//					if(usartBuffer[3] & (1 << 7))

					transfer_error = 0;
				}
				else ++transfer_error;				
			}
			else if(IsRS485_CycleTimerExpired())
			{
				if(++transfer_error > MAX_ERRORS)
				{
					RS485_ServiceState = RS485_SUSPEND;
				}
				else 
				{
					RS485_ServiceState = RS485_INIT;
				}
			}
			break;
		}
		/** ==========================================================================*/
		/**  	D I S P L A Y		T E R M I N A L 	S E T U P 		M E N U		  */
		/** ==========================================================================*/
		case RS485_SETUP_MENU:
		{
			break;
		}
		/** ==========================================================================*/
		/**  		S E T U P 		M E N U		-	D I O 	C O N F I G	  			  */
		/** ==========================================================================*/
		case RS485_SETUP_DIO:
		{
			break;
		}
		/** ==========================================================================*/
		/**  		S E T U P 	M E N U		-	P W M	 	C O N F I G	  		  	  */
		/** ==========================================================================*/
		case RS485_SETUP_PWM:
		{
			break;
		}
		/** ==========================================================================*/
		/**  		S E T U P 	M E N U		-	A N I N	 	C O N F I G	  		  	  */
		/** ==========================================================================*/
		case RS485_SETUP_ANIN:
		{
			break;
		}
		/** ==========================================================================*/
		/**  		S E T U P 	M E N U		-	H C 1 2	  	 R A D I O   M O D U L    */
		/** ==========================================================================*/
		case RS485_SETUP_HC12:
		{
			break;
		}
		/** ==========================================================================*/
		/**  		S E T U P 	M E N U		-	2 , 4 G H Z	  R A D I O   M O D U L   */
		/** ==========================================================================*/
		case RS485_SETUP_NRF24L01:
		{
			break;
		}
		/** ==========================================================================*/
		/**  		S E T U P 	M E N U		-	N R F 9 0 5	  R A D I O   M O D U L   */
		/** ==========================================================================*/
		case RS485_SETUP_NRF905:
		{
			break;
		}
		/** ==========================================================================*/
		/**  		S E T U P 	M E N U		-	W I F I 	  R A D I O   M O D U L   */
		/** ==========================================================================*/
		case RS485_SETUP_WIFI:
		{
			break;
		}
		
		/** ==========================================================================*/
		/**  			R S 4 8 5   P R O C E S S 		 S U S P E N D E D    		  */
		/** ==========================================================================*/
		case RS485_SUSPEND:
		{
			break;
		}
		
		
		default:
		{	
			RS485_ServiceState = RS485_INIT;
			break;
		}
	}
}

void Serial_PutString(uint8_t *p_string)
{   
    uint16_t length = 0;

	RS485_DirTx();
    while (p_string[length] != '\0') length++;		
	RS485_StartTimer(PACKET_TRANSFER_TIMEOUT);
	HAL_UART_Transmit(&huart1, p_string, length, PACKET_TRANSFER_TIMEOUT);
	while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) continue;	
	RS485_ResetTimer(); 
    RS485_DirRx();	
}


void Serial_PutByte(uint8_t param)
{ 
	RS485_DirTx();
	RS485_StartTimer(PACKET_TRANSFER_TIMEOUT);
    HAL_UART_Transmit(&huart1, &param, 1, PACKET_TRANSFER_TIMEOUT);	
	while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) continue;
	RS485_ResetTimer();
	RS485_DirRx();  
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) 
{
    __HAL_UART_CLEAR_PEFLAG(&huart1);
    __HAL_UART_CLEAR_FEFLAG(&huart1);
    __HAL_UART_CLEAR_NEFLAG(&huart1);
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);
    __HAL_UART_CLEAR_OREFLAG(&huart1);
	__HAL_UART_FLUSH_DRREGISTER(&huart1);
	huart->ErrorCode = HAL_UART_ERROR_NONE;
	receive_pcnt = 0;
	received_byte_cnt = 0;
	//++transfer_error;;
}


/******************************   END OF FILE  **********************************/
