/*
 * RTOS_Test.asm
 *
 *  Created: 20.12.2012 21:49:14
 *   Author: irbis
 */ 
 			.equ MainClock 		= 1000000		; CPU Clock
			;.include "m32def.inc"				; ���������� ATMega32
			.include "..\incs\vectors.inc"
			.include "..\incs\SyntRutin.inc"	; ������� ��������� ������ � ������ �������������: OUTI, OUTM,...
			.include "..\incs\SysUtils.inc"		; ������� ������ ���������� - �������������� - ������������������������.
			.equ RTOS_TaskQueueSize = 10
			.equ RTOS_TimersPoolSize = 5
			.include "..\incs\RTOS.inc"	; �������: � ������� ��������, � ������� ����������, ������ ��������(�� ���������� � ���)
			;.include "..\incs\Usart.inc"
			.equ MzLinePort = PORTD
			.equ MzLinePin  = 6
			.include "..\incs\AzbukaMorze.inc"
.DSEG
R_flag:		.byte	1

;== Interrupts ==================================================================
.CSEG
OutComp2Int: RTOS_TimerServiceBody; ���������� Timer2.
			RETI	
.include "ProjectIntMacro.inc"		;���� ���� ����������(��������)- � ���� �����.
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


Main:		SEI								; ��������� ����������.
			wdr								; Reset Watch DOG
			rcall 	RTOS_ProcessTaskQueue		; ��������� ������� ���������
			rjmp 	Main					; �������� ���� ��������� ����

;== Tasks =======================================================================
RTOS_NewTask(OnGreen)	
			SBI		PORTB,1		; ������ �������
			RTOS_SetTimerTask	TSOffGreen, 500
			RET
RTOS_NewTask(OffGreen)	
			CBI 	PORTB,1		; �������� �������
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
