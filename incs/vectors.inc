.equ Vectors = 1
;вектора тиснуты из m8def.inc
	.macro Vector; in VectorORG, VectorInst
		.ifdef @0 
			.ORG @0		@1
		.endif
	.endm
			.CSEG
			.ORG 	0x0000 	RJMP 	Reset	
			Vector	INT0addr,	RETI	; External Interrupt Request 0
			Vector	INT1addr,	RETI	; External Interrupt Request 1
			Vector	OC2addr	,	RJMP	OutComp2Int	; Timer/Counter2 Compare Match
			Vector	OVF2addr,	RETI	; Timer/Counter2 Overflow
			Vector	ICP1addr,	RETI	; Timer/Counter1 Capture Event
			Vector	OC1Aaddr,	RETI	; Timer/Counter1 Compare Match A
			Vector	OC1Baddr,	RETI	; Timer/Counter1 Compare Match B
			Vector	OVF1addr,	RETI	; Timer/Counter1 Overflow
			Vector	OVF0addr,	RETI	; Timer/Counter0 Overflow
			Vector	SPIaddr	,	RETI	; Serial Transfer Complete
			Vector	URXCaddr,	RJMP	Uart_RCV	; USART, Rx Complete
			Vector	UDREaddr,	RETI	; USART Data Register Empty
			Vector	UTXCaddr,	RETI	; USART, Tx Complete
			Vector	ADCCaddr,	RETI	; ADC Conversion Complete
			Vector	ERDYaddr,	RETI	; EEPROM Ready
			Vector	ACIaddr	,	RETI	; Analog Comparator
			Vector	TWIaddr	,	RETI	; 2-wire Serial Interface
			Vector	SPMRaddr,	RETI	; Store Program Memory Ready

			.ORG	INT_VECTORS_SIZE		;  онец таблицы прерываний
