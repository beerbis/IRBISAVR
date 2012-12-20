/*
   .MACRO RTOS_SetTimerTask(in TaskIndex:K, Timeout:K ms)
   .MACRO RTOS_SetTask(in TaskIndex:K)
   .MACRO RTOS_TimerServiceBody
   .MACRO RTOS_TurnOnTimer
   .MACRO RTOS_INIT
   #define RTOS_NewTask(Name)
   #define RTOS_ClearTaskQueueProcBody
   proc RTOS_InitTimersPool
   proc RTOS_ProcessTaskQueue(void)			//Используется в RTOS_TimerServiceBody
   proc RTOS_SendTask(in Task:Rn=OSRG)		//Обёрнут в макрос RTOS_SetTask
   proc RTOS_SetTimer(in Task:Rn=OSRG, Timeout:XYZ=X)//Обёрнуты макрос RTOS_SetTimerTask
*/
.IFDEF RTOS
	.ERROR "Модуль RTOS уже подключен."
.ENDIF
.IFNDEF SyntRutin
	.ERROR "SyntRutin не подключен"
.ENDIF
.IFNDEF RTOS_TaskQueueSize 
	.ERROR ">>>RTOS_TaskQueueSize: is empty, plees define em before Unit including<<<"
	.ELSE
		.DSEG
		RTOS_TaskQueue: 	.byte 	RTOS_TaskQueueSize 	; Адрес очереди задач в SRAM
.ENDIF
.IFNDEF RTOS_TimersPoolSize
	.WARNING ">>>RTOS_TimersPoolSize: is empty. TimerService will not be Existing, should be defined before unit including<<<"
	.ELSE
		.DSEG
		RTOS_TimersPool:	.byte 	RTOS_TimersPoolSize*3; Адреса информации о таймерах
.ENDIF
.equ RTOS = 1

.def Counter = r16	; Счетчик (преимущественно используется для организации циклов)
.def OSRG = R17		; для любого системного блуда
.def tmp2 = r18		; кажись безопасен.
;**SET_TIMER_TASK***********************************************************
		.MACRO RTOS_SetTimerTask
			ldi 	OSRG, @0
			ldi 	XL, Low(@1)			;
			ldi 	XH, High(@1)		; ЗАдержка в милисекундах
			rcall 	RTOS_SetTimer
		.ENDM

;***SET_TASK****************************************************************
		.MACRO RTOS_SetTask
			ldi OSRG, @0				; Обращение к процедуре через событийный диспетчер
			rcall RTOS_SendTask				; 
		.ENDM

;***TIMER_SERVICE_BODY******************************************************
		.MACRO RTOS_TimerServiceBody
			.equ RTOS_TimerService.Exists = 1
			push 	OSRG
			in 		OSRG,SREG			; Save Sreg
			push 	OSRG				; Сохранение регистра OSRG и регистра состояния SREG

			push 	ZL	
			push 	ZH					; сохранение Регистра Z
			push 	Counter				; сохранение Регистра Counter
	
			ldi 	ZL,low(RTOS_TimersPool)	; Загрузка с регистр Z адреса RAM, 
			ldi 	ZH,high(RTOS_TimersPool); по которому находится информация о таймерах

			ldi 	Counter,RTOS_TimersPoolSize ; максимальное количество таймеров
	
Comp1L01:	ld 		OSRG,Z				; OSRG = [Z] ; Получить номер события
			cpi 	OSRG,$FF			; Проверить на "NOP"
			breq 	Comp1L03			; Если NOP то переход к следующей позиции

			clt							; Флаг T используется для сохранения информации об окончании счёта
			ldd 	OSRG,Z+1			; 
			subi 	OSRG,Low(1) 		; Уменьшение младшей части счётчика на 1
			std 	Z+1,OSRG			;
			breq 	Comp1L02			; Если 0 то флаг T не устанавливаем
			set							; 

Comp1L02:	ldd 	OSRG,Z+2			;
			sbci 	OSRG,High(1) 		; Уменьшение старшей части счётчика на 1
			std 	Z+2,OSRG			;
			brne 	Comp1L03			; Счёт не окончен
			brts 	Comp1L03			; Счёт не окончен (по T)	
	
			ld 		OSRG,Z				; Получить номер события
			rcall 	RTOS_SendTask		; послать в системную очередь задач
	
			ldi 	OSRG,$FF			; = NOP (задача выполнена, таймер самоудаляется)
			st 		Z, OSRG				; Clear Event

Comp1L03:	subi 	ZL,Low(-3)			; Skip Counter
			sbci 	ZH,High(-3)			; Z+=3 - переход к следующему таймеру
			dec 	Counter				; счетчик таймеров
			brne 	Comp1L01			; Loop	

			pop 	Counter				; восстанавливаем переменные
			pop 	ZH
			pop 	ZL

			pop 	OSRG				; Восстанавливаем регистры
			out 	SREG,OSRG			; 
			pop 	OSRG
		.ENDM

;== INIT ==============================================================================
		.MACRO	RTOS_INIT
			ldi OSRG, 0x00
			out SREG, OSRG			; Инициализация SREG 

			rcall RTOS_InitTimersPool		; Очистить список таймеров РТОС
			rcall RTOS_InitTaskQueue	; Очистить очередь событий РТОС
			
			sei						; Разрешить обработку прерываний

; Init Timer 2
; Основной таймер для ядра РТОС

			.IFNDEF MainClock
				.ERROR "RTOS: Частота основного осцилятора не задана - MainClock"
			.ENDIF
			.equ RTOS_TimerDivider 	= MainClock/64/1000 	; 1 mS


			ldi OSRG,1<<CTC2|4<<CS20	; Freq = CK/64 - Установить режим и предделитель
			out TCCR2,OSRG				; Автосброс после достижения регистра сравнения

			clr OSRG					; Установить начальное значение счётчиков
			out TCNT2,OSRG				;	
			

			ldi OSRG,low(RTOS_TimerDivider)
			out OCR2,OSRG				; Установить значение в регистр сравнения
		.ENDM

;== TURNON_TIMER =======================================================================
		.MACRO RTOS_TurnOnTimer
			OUTI	TIMSK,0<<TOIE0|1<<OCF2		; Разрешаем прерывание по переполнению Т0
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
;== CLEAR TASK QUEUE PROC BODY =========================================================
	.macro RTOS_InitTaskQueueHD
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
	.macro RTOS_LoadTaskAddr;in Index:Rn, tmp:Rn(только не Z), out Z
		;.equ RTOS_USE255TASKS; else 128
		ldi		ZL, low(FLASHEND+1)
		ldi		ZH, high(FLASHEND+1)
		;Index*sizeOf(Pointer); *2
		lsl 	@0		;Ограничивает число задач от 256-2 до 128-1; $FF отдельно, 0 попадает.
		sub		ZL, @0
		sbci	ZH, 0	;Или второй регистр
	
		lpm		@1, Z+	; mov tmp <- CODE[Z]
		lpm				; mov r0 <- CODE[Z+1]
		mov 	ZL, @1	; Get Addr
		mov 	ZH, r0		
	.endm

;== PROCESS TASK QUEUE =======================================================
.CSEG
RTOS_ProcessTaskQueue:
;вытаскиваем индекс задачи из начала очереди.
		ldi 	ZL, low(RTOS_TaskQueue)
		ldi 	ZH, high(RTOS_TaskQueue); !Можно съэкономит на повторах ldi ZH, 0

		ld 		OSRG, Z		; Get Task Index
		cpi 	OSRG, $FF	; No Task or Addr out of Range;???
		breq 	PTQL02		; Exit;
		push 	OSRG		
		
; Сдвигаем очередь на 1 вперёд.
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
		;begin Работа с очередью не защищена от прерываний.
SEQL01: ld 		Tmp2, Z+
		cpi 	Tmp2, $FF
		breq 	SEQL02		;Нашли коенц - идём втыкать!

		dec 	Counter		;
		breq 	SEQL03		;queue overloaded - ERROR-AHTUNG-ALARM-ОЛОЛО-ПЫЩПЫЩ!!!!11одинодин - ну, или их тютелька-в-тютельку)
		rjmp 	SEQL01		;try next queue cell

SEQL02: st 		-Z, OSRG	; Store Task
		;Признак конца никто не ставит???
		;end Работа с очередью не защищена от прерываний.


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
STL03:	;поовторный цикл - нахер TODO: объеденить.

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