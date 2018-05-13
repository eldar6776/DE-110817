/**
 ******************************************************************************
 * File Name          : dio.h
 * Date               : 04/01/2018 5:22:19
 * Description        : digital input / output processing header
 ******************************************************************************
 */
 
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DIO_H
#define __DIO_H
 
 
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"


/* Typedef -------------------------------------------------------------------*/
/* Define --------------------------------------------------------------------*/
#define ERROR_SIGNAL_CYCLE_TIME		2800	// 2,8 s pause before next error signal cycle
#define ERROR_SIGNAL_ACTIV_TIME		100		// 100 ms status LED error signal activ time
#define ERROR_SIGNAL_PAUSE_TIME		300		// 300 ms pause between two error activ signals
#define ERROR_SIGNAL_COUNT			3		// 3 short signals for error status
#define CTRL_SW_SHORT_PRESS_TIME	1000	// 1 s short press time less than ...
#define CTRL_SW_LONG_PRESS_TIME		3000	// 3 s long press time more than...
#define BATTERY_MONITOR_TIME		5432	// battery status display time
#define CTRL_SW_PAUSE_TIME			543		// 123 ms control switch inactive time
#define DIO_SETTLING_TIME			200

/* Variable ------------------------------------------------------------------*/
extern volatile uint8_t error_signal_flags;
extern volatile uint16_t error_signal_timer;
extern volatile uint32_t ctrl_sw_pause;

extern uint8_t error_signal_cnt;
extern uint32_t ctrl_sw_timer;
extern uint32_t led_timer;
extern uint8_t ctrl_sw_flag;
extern uint8_t led_output;
extern uint16_t din_state;

/* Macro ---------------------------------------------------------------------*/
#define ControlSwitchState()				(GPIOB->IDR & (1 << 3))

#define SelectMuxInput0()					(GPIOB->ODR &= 0x1fff)
#define SelectMuxInput1()					((GPIOB->ODR &= 0x1fff), (GPIOB->ODR |= 0x2000))
#define SelectMuxInput2()					((GPIOB->ODR &= 0x1fff), (GPIOB->ODR |= 0x4000))
#define SelectMuxInput3()					((GPIOB->ODR &= 0x1fff), (GPIOB->ODR |= 0x6000))
#define SelectMuxInput4()					((GPIOB->ODR &= 0x1fff), (GPIOB->ODR |= 0x8000))
#define SelectMuxInput5()					((GPIOB->ODR &= 0x1fff), (GPIOB->ODR |= 0xa000))
#define SelectMuxInput6()					((GPIOB->ODR &= 0x1fff), (GPIOB->ODR |= 0xc000))
#define SelectMuxInput7()					((GPIOB->ODR &= 0x1fff), (GPIOB->ODR |= 0xe000))

#define ERROR_BatteryLowLevelSet()			(error_signal_flags |= 0x01)
#define ERROR_BatteryLowLevelReset()		(error_signal_flags &= 0xfe)
#define IsERROR_BatteryLowLevelActiv()		(error_signal_flags & 0x01)
#define ERROR_BatteryEmptySet()				(error_signal_flags |= 0x02)
#define ERROR_BatteryEmptyReset()			(error_signal_flags &= 0xfd)
#define IsERROR_BatteryEmptyActiv()			(error_signal_flags & 0x02)
#define ERROR_PwmControllerFailureSet()		(error_signal_flags |= 0x04)
#define ERROR_PwmControllerFailureReset()	(error_signal_flags &= 0xfb)
#define IsERROR_PwmControllerFailureActiv()	(error_signal_flags & 0x04)
#define ERROR_PwmOverloadWarningSet()		(error_signal_flags |= 0x08)
#define ERROR_PwmOverloadWarningReset()		(error_signal_flags &= 0xf7)
#define IsERROR_PwmOverloadWarningActiv()	(error_signal_flags & 0x08)
#define ERROR_PwmOverloadProtectionSet()	(error_signal_flags |= 0x10)
#define ERROR_PwmOverloadProtectionReset()	(error_signal_flags &= 0xef)
#define IsERROR_PwmOverloadProtectionActiv()(error_signal_flags & 0x10)
#define ERROR_TemperatureSensorSet()		(error_signal_flags |= 0x20)
#define ERROR_TemperatureSensorReset()		(error_signal_flags &= 0xdf)
#define IsERROR_TemperatureSensorActiv()	(error_signal_flags & 0x20)
#define ERROR_RadioLinkFailureSet()			(error_signal_flags |= 0x40)
#define ERROR_RadioLinkFailureReset()		(error_signal_flags &= 0xbf)
#define IsERROR_RadioLinkFailureActiv()		(error_signal_flags & 0x40)
#define ERROR_ReceiverPowerFailureSet()		(error_signal_flags |= 0x80)
#define ERROR_ReceiverPowerFailureReset()	(error_signal_flags &= 0x7f)
#define IsERROR_ReceiverPowerFailureActiv()	(error_signal_flags & 0x80)

#define ERROR_StartTimer(TIME)				(error_signal_timer = TIME)
#define ERROR_StopTimer()					(error_signal_timer = 0)
#define IsERROR_TimerExpired()				(error_signal_timer == 0)
#define LED_StartTimer(LED_TIME)			(led_timer = LED_TIME)
#define LED_StopTimer()						(led_timer = 0)
#define IsLED_TimerExpired()				(led_timer == 0)
#define CTRL_SW_StartPauseTimer(PAUSE)		(ctrl_sw_pause = PAUSE)
#define CTRL_SW_StopPauseTimer()			(ctrl_sw_pause = 0)
#define IsCTRL_SW_PauseTimerExpired()		(ctrl_sw_pause == 0)
#define CTRL_SW_OldStatePressed()			(ctrl_sw_flag |= 0x01)
#define CTRL_SW_OldStateReleased()			(ctrl_sw_flag &= 0xfe)
#define IsCTRL_SW_OldStatePressed()			(ctrl_sw_flag & 0x01)
#define CTRL_SW_ShortPressed()				(ctrl_sw_flag |= 0x02)
#define CTRL_SW_ShortReleased()				(ctrl_sw_flag &= 0xfd)
#define IsCTRL_SW_ShortPressed()			(ctrl_sw_flag & 0x02)
#define CTRL_SW_LongPressed()				(ctrl_sw_flag |= 0x04)
#define CTRL_SW_LongReleased()				(ctrl_sw_flag &= 0xfb)
#define IsCTRL_SW_LongPressed()				(ctrl_sw_flag & 0x04)
#define CTRL_SW_OldStateLongPressed()		(ctrl_sw_flag |= 0x08)
#define CTRL_SW_OldStateLongReleased()		(ctrl_sw_flag &= 0xf7)
#define IsCTRL_SW_OldStateLongPressed()		(ctrl_sw_flag & 0x08)

#define IsDIN0_Activ()						((din_state & 0x0001) != 0)
#define IsDIN1_Activ()						((din_state & 0x0002) != 0)
#define IsDIN2_Activ()						((din_state & 0x0004) != 0)
#define IsDIN3_Activ()						((din_state & 0x0008) != 0)
#define IsDIN4_Activ()						((din_state & 0x0010) != 0)
#define IsDIN5_Activ()						((din_state & 0x0020) != 0)
#define IsDIN6_Activ()						((din_state & 0x0040) != 0)
#define IsDIN7_Activ()						((din_state & 0x0080) != 0)
#define IsDIN8_Activ()						((din_state & 0x0100) != 0)
#define IsDIN9_Activ()						((din_state & 0x0200) != 0)
#define IsDIN10_Activ()						((din_state & 0x0400) != 0)
#define IsDIN11_Activ()						((din_state & 0x0800) != 0)
#define IsDIN12_Activ()						((din_state & 0x1000) != 0)
#define IsDIN13_Activ()						((din_state & 0x2000) != 0)
#define IsDIN14_Activ()						((din_state & 0x4000) != 0)
#define IsDIN15_Activ()						((din_state & 0x8000) != 0)

#define IsDIN0_OnlyActiv()					(((din_state & 0x0001) != 0) && ((din_state & 0xfffe) == 0))
#define IsDIN1_OnlyActiv()					(((din_state & 0x0002) != 0) && ((din_state & 0xfffd) == 0))
#define IsDIN2_OnlyActiv()					(((din_state & 0x0004) != 0) && ((din_state & 0xfffb) == 0))
#define IsDIN3_OnlyActiv()					(((din_state & 0x0008) != 0) && ((din_state & 0xfff7) == 0))
#define IsDIN4_OnlyActiv()					(((din_state & 0x0010) != 0) && ((din_state & 0xffef) == 0))
#define IsDIN5_OnlyActiv()					(((din_state & 0x0020) != 0) && ((din_state & 0xffdf) == 0))
#define IsDIN6_OnlyActiv()					(((din_state & 0x0040) != 0) && ((din_state & 0xffbf) == 0))
#define IsDIN7_OnlyActiv()					(((din_state & 0x0080) != 0) && ((din_state & 0xff7f) == 0))
#define IsDIN8_OnlyActiv()					(((din_state & 0x0100) != 0) && ((din_state & 0xfeff) == 0))
#define IsDIN9_OnlyActiv()					(((din_state & 0x0200) != 0) && ((din_state & 0xfdff) == 0))
#define IsDIN10_OnlyActiv()					(((din_state & 0x0400) != 0) && ((din_state & 0xfbff) == 0))
#define IsDIN11_OnlyActiv()					(((din_state & 0x0800) != 0) && ((din_state & 0xf7ff) == 0))
#define IsDIN12_OnlyActiv()					(((din_state & 0x1000) != 0) && ((din_state & 0xefff) == 0))
#define IsDIN13_OnlyActiv()					(((din_state & 0x2000) != 0) && ((din_state & 0xdfff) == 0))
#define IsDIN14_OnlyActiv()					(((din_state & 0x4000) != 0) && ((din_state & 0xbfff) == 0))
#define IsDIN15_OnlyActiv()					(((din_state & 0x8000) != 0) && ((din_state & 0x7fff) == 0))


/* Function prototypes   -----------------------------------------------------*/
void DIO_Service(void);


#endif  /* __DIO_H */


/******************************   END OF FILE  ********************************/
