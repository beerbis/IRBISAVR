.IFDEF Usart
	.ERROR "Модуль Usart уже подключен"
.ENDIF
.IFNDEF SyntRutin
	.ERROR "SyntRutin Не подключен"
.ENDIF
.equ Usart = 1

		.MACRO	UsartINIT
			.equ UsartBaudrate 		= @0
			.equ UsartBaudDivider 	= MainClock/(16*UsartBaudrate)-1
			OUTI 	UBRRL,low(UsartBaudDivider)
			OUTI 	UBRRH,high(UsartBaudDivider)
			OUTI 	UCSRA, 0
			OUTI 	UCSRB,(1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE)
			OUTI 	UCSRC,(1<<URSEL)|(3<<UCSZ0);|(1<<UCSZ2)	
		.ENDM
