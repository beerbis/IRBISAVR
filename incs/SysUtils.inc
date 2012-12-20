.IFNDEF SyntRutin
	.ERROR "SyntRutin не подключен"
.ENDIF
.equ SysUtils = 1
		
		.MACRO SysInitStack
			OUTI SPL,Low(RAMEND)
			.IFDEF SPH
				OUTI SPH,High(RAMEND)
			.ENDIF
		.ENDM

		.MACRO SysZeroMemory; Инициализация памяти
			LDI	ZL,Low(SRAM_START)	; Адрес начала ОЗУ в индекс
			LDI	ZH,High(SRAM_START)
			CLR	R16					; Очищаем R16
SysZeroMemory_Flush:		
			ST 	Z+,R16				; Сохраняем 0 в ячейку памяти
			CPI	ZH,High(RAMEND+1)	; Достигли конца оперативки?
			BRNE SysZeroMemory_Flush; Нет? Крутимся дальше!
 
			CPI	ZL,Low(RAMEND+1)	; А младший байт достиг конца?
			BRNE	SysZeroMemory_Flush
 
			CLR	ZL			; Очищаем индекс
			CLR	ZH
							; Убиваем все регистры от первого до последнего
			LDI	ZL, 30		; Адрес самого старшего регистра	
			CLR	ZH		; А тут у нас будет ноль
			DEC	ZL		; Уменьшая адрес
			ST	Z, ZH		; Записываем в регистр 0
			BRNE	PC-2		; Пока не перебрали все не успокоились
		.ENDM
