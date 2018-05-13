/**
 ******************************************************************************
 * File Name          : anin.h
 * Date               : 04/01/2018 5:21:19
 * Description        : analog input processing header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ANIN_H
#define __ANIN_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"


/* Typedef -------------------------------------------------------------------*/
/* Define --------------------------------------------------------------------*/
#define ADC_TIMEOUT					5				// 5 ms timeout for ad conversion
#define VBATT_MAX_DEFAULT			0x0840			// default full battery voltage (6 x NiMh or NiCd battery)
#define VBATT_MIN_DEFAULT			0x0580			// default empty battery voltage (6 x NiMh or NiCd battery)
#define VBATT_WARNING_DEFAULT		0x05c6			// default warning battery voltage (6 x NiMh or NiCd battery)
#define ANIN0_TYPE_DEFAULT			2				// analog input 0 is joystick potentiometer type default
#define ANIN1_TYPE_DEFAULT			2				// analog input 1 is joystick potentiometer type default
#define ANIN2_TYPE_DEFAULT			2				// analog input 2 is joystick potentiometer type default
#define ANIN3_TYPE_DEFAULT			2				// analog input 3 is joystick potentiometer type default
#define ANIN4_TYPE_DEFAULT			2				// analog input 4 is common potentiometer type default
#define ANIN5_TYPE_DEFAULT			2				// analog input 5 is common potentiometer type default
#define ANIN6_TYPE_DEFAULT			2				// analog input 6 is common potentiometer type default
#define ANIN7_TYPE_DEFAULT			1				// analog input 7 is common potentiometer type default
#define ANIN0_NULL_OFFSET_DEFAULT	0x0800			// analog input 0 default null offset
#define ANIN1_NULL_OFFSET_DEFAULT	0x0800			// analog input 1 default null offset
#define ANIN2_NULL_OFFSET_DEFAULT	0x0800			// analog input 2 default null offset
#define ANIN3_NULL_OFFSET_DEFAULT	0x0800			// analog input 3 default null offset
#define ANIN4_NULL_OFFSET_DEFAULT	0x0800			// analog input 4 default null offset
#define ANIN5_NULL_OFFSET_DEFAULT	0x0800			// analog input 5 default null offset
#define ANIN6_NULL_OFFSET_DEFAULT	0x0800			// analog input 6 default null offset
#define ANIN7_NULL_OFFSET_DEFAULT	0				// analog input 7 default null offset
#define BATTERY_TYPE_DEFAULT		1				// default battery type  NiCd
#define PWM_BUFFER_SIZE				16				// pwm buffer size
#define ANIN_POWERON_DELAY			1234			// 1,5 s after power on analog conversion suspended

/* Variable ------------------------------------------------------------------*/
extern  volatile uint32_t anin_timer;
extern uint16_t anin0;
extern uint8_t	anin0_type;
extern uint8_t	anin0_set_type;
extern uint16_t anin0_null;
extern uint16_t anin1;
extern uint8_t	anin1_type;
extern uint8_t	anin1_set_type;
extern uint16_t anin1_null;
extern uint16_t anin2;
extern uint8_t	anin2_type;
extern uint8_t	anin2_set_type;
extern int16_t 	anin2_null;
extern uint16_t anin3;
extern uint8_t	anin3_type;
extern uint8_t	anin3_set_type;
extern uint16_t anin3_null;
extern uint16_t anin4;
extern uint8_t	anin4_type;
extern uint8_t	anin4_set_type;
extern uint16_t anin4_null;
extern uint16_t anin5;
extern uint8_t	anin5_type;
extern uint8_t	anin5_set_type;
extern uint16_t anin5_null;
extern uint16_t anin6;
extern uint8_t	anin6_type;
extern uint8_t	anin6_set_type;
extern uint16_t anin6_null;
extern uint16_t anin7;
extern uint8_t	anin7_type;
extern uint8_t	anin7_set_type;
extern uint16_t anin7_null;
extern uint8_t batt_type;
extern uint16_t vbatt;
extern uint16_t vbatt_min;
extern uint16_t vbatt_warning;
extern uint16_t vbatt_max;
extern uint8_t pwm[PWM_BUFFER_SIZE];


/* Macro ---------------------------------------------------------------------*/
#define ANIN_StartTimer(ANIN_TIME)		(anin_timer = ANIN_TIME)
#define ANIN_StopTimer()				(anin_timer = 0)
#define IsANIN_TimerExpired()			(anin_timer == 0)


/* Function prototypes   -----------------------------------------------------*/
void ANIN_Service(void);
 
 
#endif  /* __ANIN_H */
 
 
/******************************   END OF FILE  *******************************/

