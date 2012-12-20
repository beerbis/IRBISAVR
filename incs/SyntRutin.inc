.equ SyntRutin = 1

; Загрузка числа в порт
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
