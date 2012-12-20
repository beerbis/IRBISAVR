/*	AzbukaMorze.inc
	Ausor: Irbis#2012-12-09
	About: 
	Comments:
	  *����� ����� ������ ��������� � ������������� ������� - ���������� ADCI.
*/

.IFDEF AzbukaMorze
	.ALERT "������ AzbukaMorze ��������� �����!"
.ENDIF
.IFNDEF RTOS_TimeService
	.ALERT "������ �������� �� ����������."
.ENDM
.equ AzbukaMorze = 1

.equ TimeDot    = 400;ms
.equ TireTime   = TimeDot*2;ms
.equ SignPause  = TimeDot;ms
.equ LeterPause = TireTime;ms

.IFDEF Tmp2
	.ALERT
.ENDIF
.def Tmp2 = R17;��������� �����������, �� ����������� RTOS_macro

.dseg
MzLeterCounter: .byte 1
MzBitCounter: .byte 1
MzBitLeter: .byte 1

.cseg
MzAzbuka:
.db/*A*/	2,	65,  /*B*/	4,	136, /*C*/	4,	138, /*D*/	3,	100, /*E*/	1,	32,  /*F*/	4,	130, /*G*/	3,	102, /*H*/	4,	128
.db/*I*/	2,	64,  /*J*/	4,	135, /*K*/	3,	101, /*L*/	4,	132, /*M*/	2,	67,  /*N*/	2,	66,  /*O*/	3,	103, /*P*/	4,	134
.db/*Q*/	4,	141, /*R*/	3,	98,  /*S*/	3,	96,  /*T*/	1,	33,  /*U*/	3,	97,  /*V*/	4,	129, /*W*/	3,	99,  /*X*/	4,	137
.db/*Y*/	4,	139, /*Z*/	4,	140, /*�*/	2,	65,  /*�*/	4,	136, /*�*/	3,	99,  /*�*/	3,	102, /*�*/	3,	100, /*�*/	1,	32
.db/*�*/	4,	129, /*�*/	4,	140, /*�*/	2,	64,  /*�*/	4,	135, /*�*/	3,	101, /*�*/	4,	132, /*�*/	2,	67,  /*�*/	2,	66
.db/*�*/	3,	103, /*�*/	4,	134, /*�*/	3,	98,  /*�*/	3,	96,  /*�*/	1,	33,  /*�*/	3,	97,  /*�*/	4,	130, /*�*/	4,	128
.db/*�*/	4,	138, /*�*/	4,	142, /*�*/	4,	143, /*�*/	4,	141, /*�*/	0,	0,	 /*�*/	4,	139, /*�*/	4,	137, /*�*/	5,	164
.db/*�*/	4,	131, /*�*/	4,	133

.MACRO MzCheckLineDefs
	.IFNDEF MzLinePort
		.ALERT "�� �������� ���� ����� �������� ������"
	.ENDIF
	.IFDEF MzLinePin
		.ALERT "�� �������� ��� ����� �������� ������"
	.ENDIF
.ENDM

.MACRO MzLineUP
	MzCheckLineDefs
	SBI MzLinePort, MzLinePin
.ENDM

.MACRO MzLineDown
	MzCheckLineDefs
	CBI MzLinePort, MzLinePin
.ENDM

.MACRO MzGetLeter;(MzLeterCounter; out Tmp2) �������� �� ������ � �������
	LDI Tmp2, 'f'
.ENDM

.MACRO MzLoadLeter;(Tmp2; out MzBitCounter, MzBitLeter) �������� �� ������ � ������.
	;����� ���������
	;����� � ������
	LDI ZL, Low(MzAzbuka)		;������� �������� ������� ��������
	LDI	ZH, High(MzAzbuka)
	;����� ����� � ��������; ����� ������������.
	SUB ZL, Tmp2		;--����� �����.
	SBCI ZL, 0			;������ �������.
	LSL ZL					;��������� �� ���� ������;*2
	ROL ZH
	LPM Tmp2, Z+			;������ ���-�� ���.
	STS MzBitCounter, Tmp2
	LPM Tmp2, Z
	STS MzBitLeter, Tmp2
.ENDM

.MACRO MzPlayBit_Start;(MzBitLeter; var MzBitCounter)
	MzLineUp
	LDS Tmp2, MzBitLeter
	LSR Tmp2	;Flasg[C] := Tmp2[0]; Tmp2[n]:= Tmp2[n+1]
	BRCC MzDoPlayBit_Dot
	;Do Tire
	RJMP MzDoPlayBit_Start_ChoosEnd
MzDoPlayBit_Start_Dot:
	
MzDoPlayBit_Start_ChoosEnd:
	LDS Tmp2, MzBitCounter
	DEC Tmp2
	BRNE MzDoPlayBit_Start_Exit
	;����� ��������� - ���������?
MzDoPlayBit_Start_Exit:
	STS MzBitCounter, Tmp2
.ENDM

.MACRO 