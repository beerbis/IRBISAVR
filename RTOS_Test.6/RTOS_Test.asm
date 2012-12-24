/*
 * RTOS_Test.asm
 *
 *  Created: 20.12.2012 21:49:14
 *   Author: irbis
 */ 
 			.equ MainClock 		= 1000000		; CPU Clock
			;.include "m32def.inc"				; Используем ATMega32
			.include "..\incs\vectors.inc"
			.include "..\incs\SyntRutin.inc"	; Макросы рутинного выбора и просто макрооперации: OUTI, OUTM,...
			.include "..\incs\SysUtils.inc"		; Макросы общего назначения - вездепотребные - одноразововставлябельные.
			.equ RTOS_TaskQueueSize = 10
			.equ RTOS_TimersPoolSize = 5
			.include "..\incs\RTOS.inc"	; Макросы: в очередь таймеров, в очередь исполнения, служба таймеров(на прерывании у нас)
			;.include "..\incs\Usart.inc"
			.equ MzLinePort = PORTD
			.equ MzLinePin  = 6
			.include "..\incs\AzbukaMorze.inc"
.DSEG
R_flag:		.byte	1

;== Interrupts ==================================================================
.CSEG
OutComp2Int: RTOS_TimerServiceBody; Прерывание Timer2.
			RETI	
.include "ProjectIntMacro.inc"		;Тела моих прерываний(векторов)- с глаз долой.
Uart_RCV:	PjIntUartRecived

;== Main Code Here ==============================================================
Reset:		SysZeroMemory
			SysInitStack
			RTOS_INIT
			RTOS_TurnOnTimer
			;UsartINIT 9600


Background:	RCALL 	OnGreen
			RCALL	OnRed
			RCALL	OnWhite
			RCALL	TSMzLoadLetter


Main:		SEI								; Разрешаем прерывания.
			wdr								; Reset Watch DOG
			rcall 	RTOS_ProcessTaskQueue		; Обработка очереди процессов
			rjmp 	Main					; Основной цикл микроядра РТОС

;== Tasks =======================================================================
RTOS_NewTask(OnGreen)	
			SBI		PORTB,1		; Зажечь зеленый
			RTOS_SetTimerTask	TSOffGreen, 500
			RET
RTOS_NewTask(OffGreen)	
			CBI 	PORTB,1		; Погасить зеленый
			RTOS_SetTimerTask	TSOnGreen,500
			RET
RTOS_NewTask(OnRed)
			SBI		PORTB,2
			RTOS_SetTimerTask	TSOffRed,30
			RET
RTOS_NewTask(OffRed)
			CBI		PORTB,2
			RTOS_SetTimerTask	TSOnRed, 300
			RET
RTOS_NewTask(OnWhite)
			SBI		PORTB,3
			RTOS_SetTimerTask	TSOffWhite,150
			RET
RTOS_NewTask(OffWhite)
			CBI		PORTB,3
			RTOS_SetTimerTask	TSOnWhite, 66
			RET

;== [THE END] ===========================================================
