.IFNDEF SyntRutin 
	.ERROR "SyntRutin �� ���������"
.ENDIF

.IFNDEF RTOS_TimerService.Exists
	.ERROR ">>>RTOS TimerService is disabled<<< ='("
.ENDIF

.equ q = 1

		.MACRO PjIntUartRecived
			push 	OSRG
			in 		OSRG,SREG				; ������� OSRG � �����. 
			push 	OSRG	
			PUSH	XL						; SetTimerTask ����� �!!!
			PUSH	XH						; ������� ������ ���!
			PUSH	Tmp2					; �� � Tmp2 ��� ����������

			;OUTI	UDR, 0 ;'Z'
			RTOS_SetTask	TSOnWhite			; ������ ����-������!
			IN 		OSRG,UDR
			OUTR	UDR,OSRG

			CPI		OSRG,'R'				; ��������� �������� ����
			;BREQ	Armed					; ���� = R  - ���� �������� ����

			LDS		Tmp2,R_flag				; ���� �� R � ����� ���������� ���
			CPI		Tmp2,0		
			BREQ	PjIntUartRecived_EXIT	; �� ������� �� �����

			CPI		OSRG,'A'				; �� ������. ��� '�'?	
			;BRNE	U_RCV_EXIT	; ���? ����� �����!

PjIntUartRecived_EXIT:	
			POP		Tmp2
			POP 	XH						; ��������� ������ �� ����������
			POP		XL
			pop 	OSRG					; ��������������� ��������
			out 	SREG,OSRG		 
			pop 	OSRG
			RETI							; <<<<<< �������

Armed:		LDI		OSRG,1					; ������� ���� ����������
			STS		R_flag,OSRG				; ��������� ��� � ���


			;OsSetTimerTask	TS_ResetR,10

			RJMP	PjIntUartRecived_EXIT	; ������� � ������
		.ENDM
		
