.equ SyntRutin = 1

; «агрузка числа в порт
			.MACRO 	OUTI
			LDI 	R16,@1
			OUT 	@0,R16
			.ENDMACRO	
			
			.MACRO OUTR
			OUT		@0, @1
			.ENDMACRO


;SRAM LDS/STS analog for Tiny
			.MACRO	LDR; (out Rn@0; const k@1)
			PUSH	ZL
			PUSH	ZH

			LDI		ZL,low(@1)
			LDI		ZH,High(@1)

			LD		@0,Z

			POP		ZH
			POP		ZL
			.ENDM



			.MACRO	STR; (out k@0; const Rn@1)
			PUSH	ZL
			PUSH	ZH

			LDI		ZL,low(@0)
			LDI		ZH,High(@0)

			ST		Z,@1

			POP		ZH
			POP		ZL
			.ENDM

	.macro incset; var Name
		.ifdef @0
			.set @0 = @0 + 1
		.else
			.set @0 = 1
		.endif
	.endm
;===============================================================================
		;TDirection = (
	.equ Forward = 0
	.equ Backword = 1 ;)	0			1				2				3			4
	.macro LoadAddr;in Index:Rn, out Addr: XYZ, in Table: Pointer, ItemSize, Direction: TDirection
		;вычислени и загрузка базы в индексную пару.
		.if		@4 == Backword
			ldi		@1L, low(@2-@3+1)
			ldi		@1H, high(@2-@3+1)
		.elif	@4 == Forward
			ldi		@1L, low(@2)
			ldi		@1H, high(@2)
		.else 
			.error ">>>Unknown Table-index direction<<<"
		.endif
		
		;смещение по индексу от базы
		.if		@3 == 1
		.elif	@3 == 2
			lsl		@0	
		.else
			.error ">>>not realized, till: you can do it $)<<<"
		.endif
		
		;применение смещени€ к базе
		.if		@4 == Backword
			sub		@1L, @0
			sbci	@1H, 0	;»ли второй регистр
		.elif	@4 == Forward
			add		@1L, @0
			brcc	DoNotInc ;стрЄмный вариант вместо adci
			inc		@1H
			DoNotInc:
		.else
			.error ">>>Unknown Table-index direction<<<"
		.endif
	.endm