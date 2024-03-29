/*	AzbukaMorze.inc
	Ausor: Irbis#2012-12-09
	About: 
	Comments:
	  *����� ����� ������ ��������� � ������������� ������� - ���������� ADCI.
*/

.IFDEF AzbukaMorze
	.ERROR ">>>������ AzbukaMorze ��������� �����!<<<"
.ENDIF
.IFNDEF RTOS
	.ERROR ">>>RTOS required<<<"
.ENDIF
.equ AzbukaMorze = 1

.equ MzTimeDot    = 400;ms
.equ MzTimeDash   = MzTimeDot*2;ms
.equ MzSignPause  = MzTimeDot;ms
.equ MzLeterPause = MzTimeDash;ms


.dseg
MzLetterCounter: .byte 1
MzBitCounter: .byte 1
MzBitLetter: .byte 1

.cseg
MzString: .db $11, $FE, $0D, $0E, $02, $1B, $0C, $FE, $03, $0E, $04, $0E, $0C, $FF
MzAzbuka:
MzAzbukaEN:
.db/*A*/	2,	65,  /*B*/	4,	136, /*C*/	4,	138, /*D*/	3,	100, /*E*/	1,	32,  /*F*/	4,	130, /*G*/	3,	102, /*H*/	4,	128
.db/*I*/	2,	64,  /*J*/	4,	135, /*K*/	3,	101, /*L*/	4,	132, /*M*/	2,	67,  /*N*/	2,	66,  /*O*/	3,	103, /*P*/	4,	134
.db/*Q*/	4,	141, /*R*/	3,	98,  /*S*/	3,	96,  /*T*/	1,	33,  /*U*/	3,	97,  /*V*/	4,	129, /*W*/	3,	99,  /*X*/	4,	137
.db/*Y*/	4,	139, /*Z*/	4,	140
MzAzbukaRU:
.db/*�*/	2,	65,  /*�*/	4,	136, /*�*/	3,	99,  /*�*/	3,	102, /*�*/	3,	100, /*�*/	1,	32
.db/*�*/	4,	129, /*�*/	4,	140, /*�*/	2,	64,  /*�*/	4,	135, /*�*/	3,	101, /*�*/	4,	132, /*�*/	2,	67,  /*�*/	2,	66
.db/*�*/	3,	103, /*�*/	4,	134, /*�*/	3,	98,  /*�*/	3,	96,  /*�*/	1,	33,  /*�*/	3,	97,  /*�*/	4,	130, /*�*/	4,	128
.db/*�*/	4,	138, /*�*/	4,	142, /*�*/	4,	143, /*�*/	4,	141, /*�*/	0,	0,	 /*�*/	4,	139, /*�*/	4,	137, /*�*/	5,	164
.db/*�*/	4,	131, /*�*/	4,	133

.MACRO MzCheckLineDefs
	.IFNDEF MzLinePort
		.ERROR ">>>MzLinePort not defined<<<"
	.ENDIF
	.IFNDEF MzLinePin
		.ERROR ">>>MzLinePin not defined<<<"
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

RTOS_NewTask(MzLoadLetter)
;.MACRO MzGetLetter;(MzLeterCounter; out Tmp2) �������� �� ������ � �������
	LDS	OSRG, MzLetterCounter
	LoadAddr OSRG, Z, MzString*2, 1, Forward
	INC OSRG
	STS	MzLetterCounter, OSRG
	; ����� ����� � OSRG
	LPM OSRG, Z
	;����� � OSRG
	CPI OSRG, $FE; ������?
	BREQ MzLoadNeedNextLetter
	CPI	OSRG, $FF; ����� ������? - ���������� �������.
	BRNE MzLoadKeepLoading
	
	CLR OSRG
	STS	MzLetterCounter, OSRG
MzLoadNeedNextLetter:
	RTOS_SetTimerTask TSMzLoadLetter, MzLeterPause
	RJMP MzLoadExit

MzLoadKeepLoading:
	LoadAddr OSRG, Z, MzAzbukaRU*2, 2, Forward
	LPM OSRG, Z+
	TST OSRG	;���-�� ��� � ����� = 0 -> ������ �����- ���������.
	BREQ MzLoadNeedNextLetter
	STS MzBitCounter, OSRG
	LPM OSRG, Z
	STS MzBitLetter, Tmp2
	RTOS_SetTask TSMzPlayBit

MzLoadExit:
	ret

RTOS_NewTask(MzPlayBit)
	LDS	OSRG, MzBitCounter
	DEC OSRG
	BREQ MzPlayLatterEOF	;���� ����� ����������� - ������� ���������.
	; � ����� ����� =): RJMP MzLoadNeedNextLetter
	STS MzBitCounter, OSRG

	MzLineUP
	LDS	OSRG, MzBitLetter
	ROR	OSRG	;�� C �������� Dot-Dash
	STS MzBitLetter, OSRG
	BRCC MzPlayDot
	;MzPlayDash:
	RTOS_SetTimerTask TSMzStopBit, MzTimeDash
	RJMP MzPlayEndChoos
	MzPlayDot:
	RTOS_SetTimerTask TSMzStopBit, MzTimeDot
	MzPlayEndChoos:
	RJMP MzPlayExit

MzPlayLatterEOF:
	RTOS_SetTimerTask TSMzLoadLetter, MzLeterPause
MzPlayExit:
	ret

RTOS_NewTask(MzStopBit)
	MzLineDown
	RTOS_SetTimerTask TSMzPlayBit, MzSignPause
	ret