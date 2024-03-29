/*
   .MACRO RTOS_SetTimerTask(in TaskIndex:K, Timeout:K ms)
   .MACRO RTOS_SetTask(in TaskIndex:K)
   .MACRO RTOS_TimerServiceBody
   .MACRO RTOS_TurnOnTimer
   .MACRO RTOS_INIT
   #define RTOS_NewTask(Name)
   #define RTOS_ClearTaskQueueProcBody
   proc RTOS_InitTimersPool
   proc RTOS_ProcessTaskQueue(void)			//������������ � RTOS_TimerServiceBody
   proc RTOS_SendTask(in Task:Rn=OSRG)		//������ � ������ RTOS_SetTask
   proc RTOS_SetTimer(in Task:Rn=OSRG, Timeout:XYZ=X)//������� ������ RTOS_SetTimerTask
*/
.IFDEF RTOS
	.ERROR "������ RTOS ��� ���������."
.ENDIF
.IFNDEF SyntRutin
	.ERROR "SyntRutin �� ���������"
.ENDIF
.IFNDEF RTOS_TaskQueueSize 
	.ERROR ">>>RTOS_TaskQueueSize: is empty, plees define em before Unit including<<<"
.ELSE
	.DSEG
	RTOS_TaskQueue: 	.byte 	RTOS_TaskQueueSize 	; ����� ������� ����� � SRAM
.ENDIF
.IFNDEF RTOS_TimersPoolSize
	.WARNING ">>>RTOS_TimersPoolSize: is empty. TimerService will not be Existing, should be defined before unit including<<<"
.ELSE
	.DSEG
	RTOS_TimersPool:	.byte 	RTOS_TimersPoolSize*3; ������ ���������� � ��������
.ENDIF
.equ RTOS = 1

.def Counter = r16	; ������� (��������������� ������������ ��� ����������� ������)
.def OSRG = R17		; ��� ������ ���������� �����
.def tmp2 = r18		; ������ ���������.
;**SET_TIMER_TASK***********************************************************
		.MACRO RTOS_SetTimerTask
			ldi 	OSRG, @0
			ldi 	XL, Low(@1)			;
			ldi 	XH, High(@1)		; �������� � ������������
			rcall 	RTOS_SetTimer
		.ENDM

;***SET_TASK****************************************************************
		.MACRO RTOS_SetTask
			ldi OSRG, @0				; ��������� � ��������� ����� ���������� ���������
			rcall RTOS_SendTask				; 
		.ENDM

;***TIMER_SERVICE_BODY******************************************************
		.MACRO RTOS_TimerServiceBody
			.equ RTOS_TimerService.Exists = 1
			push 	OSRG
			in 		OSRG,SREG			; Save Sreg
			push 	OSRG				; ���������� �������� OSRG � �������� ��������� SREG

			push 	ZL	
			push 	ZH					; ���������� �������� Z
			push 	Counter				; ���������� �������� Counter
	
			ldi 	ZL,low(RTOS_TimersPool)	; �������� � ������� Z ������ RAM, 
			ldi 	ZH,high(RTOS_TimersPool); �� �������� ��������� ���������� � ��������

			ldi 	Counter,RTOS_TimersPoolSize ; ������������ ���������� ��������
	
Comp1L01:	ld 		OSRG,Z				; OSRG = [Z] ; �������� ����� �������
			cpi 	OSRG,$FF			; ��������� �� "NOP"
			breq 	Comp1L03			; ���� NOP �� ������� � ��������� �������

			clt							; ���� T ������������ ��� ���������� ���������� �� ��������� �����
			ldd 	OSRG,Z+1			; 
			subi 	OSRG,Low(1) 		; ���������� ������� ����� �������� �� 1
			std 	Z+1,OSRG			;
			breq 	Comp1L02			; ���� 0 �� ���� T �� �������������
			set							; 

Comp1L02:	ldd 	OSRG,Z+2			;
			sbci 	OSRG,High(1) 		; ���������� ������� ����� �������� �� 1
			std 	Z+2,OSRG			;
			brne 	Comp1L03			; ���� �� �������
			brts 	Comp1L03			; ���� �� ������� (�� T)	
	
			ld 		OSRG,Z				; �������� ����� �������
			rcall 	RTOS_SendTask		; ������� � ��������� ������� �����
	
			ldi 	OSRG,$FF			; = NOP (������ ���������, ������ �������������)
			st 		Z, OSRG				; Clear Event

Comp1L03:	subi 	ZL,Low(-3)			; Skip Counter
			sbci 	ZH,High(-3)			; Z+=3 - ������� � ���������� �������
			dec 	Counter				; ������� ��������
			brne 	Comp1L01			; Loop	

			pop 	Counter				; ��������������� ����������
			pop 	ZH
			pop 	ZL

			pop 	OSRG				; ��������������� ��������
			out 	SREG,OSRG			; 
			pop 	OSRG
		.ENDM

;== INIT ==============================================================================
		.MACRO	RTOS_INIT
			ldi OSRG, 0x00
			out SREG, OSRG			; ������������� SREG 

			rcall RTOS_InitTimersPool		; �������� ������ �������� ����
			rcall RTOS_InitTaskQueue	; �������� ������� ������� ����
			
			sei						; ��������� ��������� ����������

; Init Timer 2
; �������� ������ ��� ���� ����

			.IFNDEF MainClock
				.ERROR "RTOS: ������� ��������� ���������� �� ������ - MainClock"
			.ENDIF
			.equ RTOS_TimerDivider 	= MainClock/64/1000 	; 1 mS


			ldi OSRG,1<<CTC2|4<<CS20	; Freq = CK/64 - ���������� ����� � ������������
			out TCCR2,OSRG				; ��������� ����� ���������� �������� ���������

			clr OSRG					; ���������� ��������� �������� ���������
			out TCNT2,OSRG				;	
			

			ldi OSRG,low(RTOS_TimerDivider)
			out OCR2,OSRG				; ���������� �������� � ������� ���������
		.ENDM

;== TURNON_TIMER =======================================================================
		.MACRO RTOS_TurnOnTimer
			OUTI	TIMSK,0<<TOIE0|1<<OCF2		; ��������� ���������� �� ������������ �0
		.ENDM

;== NEW_TASK ===========================================================================

		#define RTOS_NewTask(Name)	\
		Name:			\
		RTOS_TaskHD	Name	;Never use this macros by yourself
		.macro RTOS_TaskHD //TaskHD GreenON -> TSGreen = TaskCounter
			incset RTOS_TaskCounter
			.equ TS@0 = RTOS_TaskCounter
			.ORG FLASHEND+1 - RTOS_TaskCounter*2
			.dw @0
			.ORG @0
		.endm	
;== CLEAR TASK QUEUE PROC BODY ==========, 0
			.equ TS@0 = RTOS_TaskCounter
			.ORG FLASHEND - RTOS_TaskCounterHD
		//.equ RTOS_InitTaskQueue.Exists = 1
		push 	ZL
		push 	ZH

		ldi 	ZL, low(RTOS_TaskQueue)
		ldi 	ZH, high(RTOS_TaskQueue)

		ldi 	OSRG, $FF		
		ldi 	Counter, RTOS_TaskQueueSize

CEQL01: st 		Z+, OSRG		;
		dec 	Counter		;
		brne 	CEQL01		; Loop

		pop 	ZH
		pop 	ZL
		ret
	.endm
	//#define RTOS_ClearTaskQueueProcBody \
	.CSEG //\
	RTOS_InitTaskQueue: //\
	RTOS_InitTaskQueueHD

;== CLEAR TIMERS =======================================================================
	.macro RTOS_InitTimersPoolHD
		//.equ RTOS_InitTimersPool.Exists = 1
		push 	ZL
		push 	ZH

		ldi 	ZL, low(RTOS_TimersPool)
		ldi 	ZH, high(RTOS_TimersPool)

		ldi 	Counter, RTOS_TimersPoolSize
		ldi 	OSRG, $FF		; Empty 
		ldi 	Tmp2, $00

CTL01:	st 		Z+, OSRG		; Event
		st 		Z+, Tmp2		; Counter Lo
		st 		Z+, Tmp2		; Counter Hi

		dec 	Counter		;
		brne 	CTL01		; Loop
	
		pop 	ZH
		pop 	ZL
		ret	
	.endm
//	#define RTOS_InitTimersPoolProcBody \
	.CSEG //\
	RTOS_InitTimersPool: //\
	RTOS_InitTimersPoolHD

;== LOAD TASK ADDR ============================================================
	.macro RTOS_LoadTaskAddr;in Index:Rn, tmp:Rn,  out Addr:XYZ=Z
		;.equ RTOS_USE255TASKS; else 128
		LoadAddr @0, Z, FLASHEND, 2, Backword
		lpm		@1, Z+	; mov tmp <- CODE[Z]
		lpm				; mov r0 <- CODE[Z+1]
		mov 	ZH, r0		
		mov 	ZL, @1	; Get Addr
	.endm

;== PROCESS TASK QUEUE =======================================================*2+1
.CSEG
RTOS_ProcessTaskQueue:
;����������� ������ ������ �� ������ �������.
		ldi 	ZL, low(RTOS_TaskQueue)
		ldi 	ZH, high(RTOS_TaskQueue); !����� ���������� �� �������� ldi ZH, 0

		ld 		OSRG, Z		; Get Task Index
		cpi 	OSRG, $FF	; No Task or Addr out of Range;???
		breq 	PTQL02		; Exit;
		push 	OSRG		
		
; �������� ������� �� 1 �����.
		ldi 	Counter, RTOS_TaskQueueSize-1
		ldi 	ZL, low(RTOS_TaskQueue)
		ldi 	ZH, high(RTOS_TaskQueue)
	
		cli
	
PTQL01:	ldd 	OSRG, Z+1 	;	Shift Queues
		st 		Z+, OSRG		

		dec 	Counter		
		brne 	PTQL01		; Loop
		ldi 	OSRG, $FF	
		st 		Z, OSRG		
	
		sei

		pop 	OSRG
		RTOS_LoadTaskAddr OSRG, OSRG
		ijmp 			; Minimize Stack Usage
PTQL02:	ret	

;== SEND TASK =========================================================
.CSEG 
RTOS_SendTask:
		push 	ZL
		push 	ZH
		push 	Tmp2
		push 	Counter

		ldi 	ZL,low(RTOS_TaskQueue)
		ldi 	ZH,high(RTOS_TaskQueue)

		ldi 	Counter, RTOS_TaskQueueSize
		;begin ������ � �������� �� �������� �� ����������.
SEQL01: ld 		Tmp2, Z+
		cpi 	Tmp2, $FF
		breq 	SEQL02		;����� ����� - ��� �������!

		dec 	Counter		;
		breq 	SEQL03		;queue overloaded - ERROR-AHTUNG-ALARM-�����-������!!!!11�������� - ��, ��� �� ��������-�-��������)
		rjmp 	SEQL01		;try next queue cell

SEQL02: st 		-Z, OSRG	; Store Task
		;������� ����� ����� �� ������???
		;end ������ � �������� �� �������� �� ����������.


SEQL03:					; EXIT
		pop 	Counter
		pop 	Tmp2
		pop 	ZH
		pop 	ZL
		ret	

;== SET TIMER ==========================================================
.CSEG
RTOS_SetTimer:
		push 	ZL
		push 	ZH
		push 	Tmp2
		push 	Counter

		ldi 	ZL,low(RTOS_TimersPool)
		ldi 	ZH,high(RTOS_TimersPool)

		ldi 	Counter, RTOS_TimersPoolSize
	
STL01: 	ld 		Tmp2, Z		; Value / Counter
		cp 		Tmp2, OSRG		; Search for Event
		breq 	STL02
	
		subi 	ZL, Low(-3)	; Skip Counter
		sbci 	ZH, High(-3)	; Z+=3

		dec 	Counter		;
		breq 	STL03		; Loop
		rjmp 	STL01
	
STL02:	;cli
		std 	Z+1, XL		; Critical Section
		std 	Z+2, XH		; Update Counter
		;sei				; leave Critical Section
		rjmp	STL06		; Exit
STL03:	;���������� ���� - ����� TODO: ����������.

		ldi 	ZL, low(RTOS_TimersPool)
		ldi 	ZH, high(RTOS_TimersPool)

		ldi 	Counter, RTOS_TimersPoolSize
	
STL04:	ld 		Tmp2, Z		; Value / Counter
		cpi 	Tmp2, $FF		; Search for Empty Timer
		breq 	STL05
	
		subi 	ZL, Low(-3)	; Skip Counter
		sbci 	ZH, High(-3)	; Z+=2

		dec 	Counter		;
		breq 	STL06		; No Empty Timer
		rjmp 	STL04
	
STL05:	cli
		st 		Z, OSRG		; Set Event 
		std 	Z+1, XL
		std 	Z+2, XH
		sei

STL06:
		pop 	Counter
		pop 	Tmp2
		pop 	ZH
		pop 	ZL
		ret	
;====[The End]======================================================