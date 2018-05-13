/**
 ******************************************************************************
 * File Name          : rs485.h
 * Date               : 28/02/2016 23:16:19
 * Description        : rs485 communication modul header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RS485_H
#define __RS485_H							100 	// version 1.00


/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"


/* Typedef -------------------------------------------------------------------*/
typedef enum
{
	RS485_INIT 					= 0x00,
	RS485_SEND_STATUS			= 0x01,
	RS485_RECEIVE_STATUS		= 0x02,
	RS485_SETUP_MENU			= 0x03,
	RS485_SETUP_DIO				= 0x04,
	RS485_SETUP_PWM				= 0x05,
	RS485_SETUP_ANIN			= 0x06,
	RS485_SETUP_HC12			= 0x07,
	RS485_SETUP_NRF24L01		= 0x08,
	RS485_SETUP_NRF905			= 0x09,
	RS485_SETUP_WIFI			= 0x0a,
	RS485_SUSPEND				= 0x0b
	
} RS485_ServiceStateTypeDef;
	

/* Define --------------------------------------------------------------------*/
#define BYTE_TRANSFER_TIMEOUT			5					/* 3 ms timeout for next byte transfer */
#define RECEIVER_REINIT_TIMEOUT			250					/* if receiver stop receiving for 250 ms reinit usart */
#define PACKET_TRANSFER_TIMEOUT			50					/* 50 ms for packet transfer and ACK response*/
#define MAX_ERRORS                  	5					/* maximum number of successive error	*/
#define UART_BUFFER_SIZE				64					/* 64 bytes buffer lenght */
#define RS485_CYCLE_TIME				234					/* send status every xxx ms */

#define SOH                         	((uint8_t)0x01) 	/* start of command packet */
#define EOT                         	((uint8_t)0x04) 	/* end of transmission */
#define ACK                         	((uint8_t)0x06) 	/* acknowledge */
#define NAK                         	((uint8_t)0x15) 	/* negative acknowledge */

/** ============================================================================*/
/**                                                                           	*/
/**             R S  4 8 5   P A C K E T   F O R M A T        			   		*/ 
/**                                                                           	*/
/** ============================================================================*/
/** 	
*		command packet
*/
//		PACKET_START_IDENTIFIER
//		DIN_8_15_STATE						
//		DIN_0_7_STATE
//		ANIN7_VALUE
//		ANIN6_VALUE
//		ANIN5_VALUE
//		ANIN4_VALUE
//		ANIN3_VALUE
//		ANIN2_VALUE
//		ANIN1_VALUE
//		ANIN0_VALUE
//		VBATT_VALUE
//		PACKET_CHECKSUM
//		PACKET_END_IDENTIFIER
/** 	
*		response packet
*/
//		PACKET_START_IDENTIFIER
//		DIN_0_7_STATE
//		SYS_STATE
//		PACKET_CHECKSUM
//		PACKET_END_IDENTIFIER


/* Variable ------------------------------------------------------------------*/
extern uint8_t usartBuffer[UART_BUFFER_SIZE];

extern volatile uint8_t receive_pcnt;
extern volatile uint8_t transfer_error;
extern volatile uint8_t received_byte_cnt;

extern volatile uint32_t rs485_timer;
extern volatile uint32_t rs485_flags;
extern volatile uint32_t rs485_cycle_timer;

extern RS485_ServiceStateTypeDef RS485_ServiceState;


/* Macro ---------------------------------------------------------------------*/
#define RS485_DirRx()   				(HAL_GPIO_WritePin (GPIOA, GPIO_PIN_8, GPIO_PIN_SET))
#define RS485_DirTx()   				(HAL_GPIO_WritePin (GPIOA, GPIO_PIN_8, GPIO_PIN_RESET))
#define RS485_StartTimer(TIME)			(rs485_timer = TIME)
#define RS485_ResetTimer()				(rs485_timer = 0)
#define IsRS485_TimerExpired()			(rs485_timer == 0)
#define RS485_StartCycleTimer(CYCLE)	(rs485_cycle_timer = CYCLE)
#define RS485_StopCycleTimer()			(rs485_cycle_timer = 0)
#define IsRS485_CycleTimerExpired()		(rs485_cycle_timer == 0)
#define RS485_RxStatusReady()			(rs485_flags |= 0x00000001)
#define RS485_RxStatusReset()			(rs485_flags &= 0xfffffffe)
#define IsRS485_RxStatusReady()			(rs485_flags & 0x00000001)
#define RS485_TxStatusReady()			(rs485_flags |= 0x00000002)
#define RS485_TxStatusReset()			(rs485_flags &= 0xfffffffd)
#define IsRS485_TxStatusReady()			(rs485_flags & 0x00000002)


/* Function prototypes   -----------------------------------------------------*/
void RS485_Service(void);
void Serial_PutByte(uint8_t param);
void Serial_PutString(uint8_t *p_string);


#endif  /* __RS485_H */


/******************************   END OF FILE  **********************************/
