.IFNDEF SyntRutin
	.ERROR "SyntRutin �� ���������"
.ENDIF
.equ SysUtils = 1
		
		.MACRO SysInitStack
			OUTI SPL,Low(RAMEND)
			.IFDEF SPH
				OUTI SPH,High(RAMEND)
			.ENDIF
		.ENDM

		.MACRO SysZeroMemory; ������������� ������
			LDI	ZL,Low(SRAM_START)	; ����� ������ ��� � ������
			LDI	ZH,High(SRAM_START)
			CLR	R16					; ������� R16
SysZeroMemory_Flush:		
			ST 	Z+,R16				; ��������� 0 � ������ ������
			CPI	ZH,High(RAMEND+1)	; �������� ����� ����������?
			BRNE SysZeroMemory_Flush; ���? �������� ������!
 
			CPI	ZL,Low(RAMEND+1)	; � ������� ���� ������ �����?
			BRNE	SysZeroMemory_Flush
 
			CLR	ZL			; ������� ������
			CLR	ZH
							; ������� ��� �������� �� ������� �� ����������
			LDI	ZL, 30		; ����� ������ �������� ��������	
			CLR	ZH		; � ��� � ��� ����� ����
			DEC	ZL		; �������� �����
			ST	Z, ZH		; ���������� � ������� 0
			BRNE	PC-2		; ���� �� ��������� ��� �� �����������
		.ENDM
