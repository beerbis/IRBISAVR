.IFNDEF SyntRutin 
	.ERROR "SyntRutin не подключен"
.ENDIF

.IFNDEF RTOS_TimerService.Exists
	.ERROR ">>>RTOS TimerService is disabled<<< ='("
.ENDIF

.equ q = 1

		.MACRO PjIntUartRecived
			push 	OSRG
			in 		OSRG,SREG				; Спасаем OSRG и флаги. 
			push 	OSRG	
			PUSH	XL						; SetTimerTask юзает Х!!!
			PUSH	XH						; Поэтому прячем его!
			PUSH	Tmp2					; Ну и Tmp2 нам пригодится

			;OUTI	UDR, 0 ;'Z'
			RTOS_SetTask	TSOnWhite			; Зажечь бело-лунный!
			IN 		OSRG,UDR
			OUTR	UDR,OSRG

			CPI		OSRG,'R'				; Проверяем принятый байт
			;BREQ	Armed					; Если = R  - идем взводить флаг

			LDS		Tmp2,R_flag				; Если не R и флага готовности нет
			CPI		Tmp2,0		
			BREQ	PjIntUartRecived_EXIT	; То переход на выход

			CPI		OSRG,'A'				; Мы готовы. Это 'А'?	
			;BRNE	U_RCV_EXIT	; Нет? Тогда выход!

PjIntUartRecived_EXIT:	
			POP		Tmp2
			POP 	XH						; Процедура выхода из прерывания
			POP		XL
			pop 	OSRG					; Восстанавливаем регистры
			out 	SREG,OSRG		 
			pop 	OSRG
			RETI							; <<<<<< Выходим

Armed:		LDI		OSRG,1					; Взводим флаг готовности
			STS		R_flag,OSRG				; Сохраняем его в ОЗУ


			;OsSetTimerTask	TS_ResetR,10

			RJMP	PjIntUartRecived_EXIT	; Переход к выходу
		.ENDM
		
