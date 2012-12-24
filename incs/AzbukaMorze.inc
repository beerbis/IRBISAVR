/*	AzbukaMorze.inc
	Ausor: Irbis#2012-12-09
	About: 
	Comments:
	  *Буквы фразы должны храниться в отрицательном формате - отсутствие ADCI.
*/

.IFDEF AzbukaMorze
	.ERROR ">>>Модуль AzbukaMorze подключен ранее!<<<"
.ENDIF
.IFNDEF RTOS
	.ERROR ">>>RTOS required<<<"
.ENDIF
.equ AzbukaMorze = 1

.equ MzTimeDot    = 400;ms
.equ MzTimeTire   = MzTimeDot*2;ms
.equ MzSignPause  = MzTimeDot;ms
.equ MzLeterPause = MzTimeTire;ms


.dseg
MzLeterCounter: .byte 1
MzBitCounter: .byte 1
MzBitLeter: .byte 1

.cseg
MzString: .db $12, $00, $0E, $0F, $03, $1C, $0D, $00, $04, $0F, $05, $0F, $0D
MzAzbuka:
MzAzbukaEN:
.db/*A*/	2,	65,  /*B*/	4,	136, /*C*/	4,	138, /*D*/	3,	100, /*E*/	1,	32,  /*F*/	4,	130, /*G*/	3,	102, /*H*/	4,	128
.db/*I*/	2,	64,  /*J*/	4,	135, /*K*/	3,	101, /*L*/	4,	132, /*M*/	2,	67,  /*N*/	2,	66,  /*O*/	3,	103, /*P*/	4,	134
.db/*Q*/	4,	141, /*R*/	3,	98,  /*S*/	3,	96,  /*T*/	1,	33,  /*U*/	3,	97,  /*V*/	4,	129, /*W*/	3,	99,  /*X*/	4,	137
.db/*Y*/	4,	139, /*Z*/	4,	140
MzAzbukaRU:
.db/*А*/	2,	65,  /*Б*/	4,	136, /*В*/	3,	99,  /*Г*/	3,	102, /*Д*/	3,	100, /*Е*/	1,	32
.db/*Ж*/	4,	129, /*З*/	4,	140, /*И*/	2,	64,  /*Й*/	4,	135, /*К*/	3,	101, /*Л*/	4,	132, /*М*/	2,	67,  /*Н*/	2,	66
.db/*О*/	3,	103, /*П*/	4,	134, /*Р*/	3,	98,  /*С*/	3,	96,  /*Т*/	1,	33,  /*У*/	3,	97,  /*Ф*/	4,	130, /*Х*/	4,	128
.db/*Ц*/	4,	138, /*Ч*/	4,	142, /*Ш*/	4,	143, /*Щ*/	4,	141, /*Ъ*/	0,	0,	 /*Ы*/	4,	139, /*Ь*/	4,	137, /*Э*/	5,	164
.db/*Ю*/	4,	131, /*Я*/	4,	133

.MACRO MzCheckLineDefs
	.IFNDEF MzLinePort
		.ERROR "MzLinePort not defined"
	.ENDIF
	.IFDEF MzLinePin
		.ERROR "MzLinePin not defined"
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

.MACRO MzGetLeter;(MzLeterCounter; out Tmp2) Загрузка из строки в регистр
	LDI Tmp2, 'f'
.ENDM

.MACRO MzLoadLeter;(Tmp2; out MzBitCounter, MzBitLeter) Загрузка из азбуки в память.
	;Буква сообщения
	;Буква в индекс
	LDI ZL, Low(MzAzbuka)		;Базовое смещение массива морзянки
	LDI	ZH, High(MzAzbuka)
	;номер буквы в алфавите; номер отицательный.
	SUB ZL, Tmp2		;--номер буквы.
	SBCI ZL, 0			;Учесть перенос.
	LSL ZL					;Выровнять на шину данных;*2
	ROL ZH
	LPM Tmp2, Z+			;Читаем кол-во бит.
	STS MzBitCounter, Tmp2
	LPM Tmp2, Z
	STS MzBitLeter, Tmp2
.ENDM

.MACRO MzPlayBit_Start;(MzBitLeter; var MzBitCounter)
	MzLineUp
	LDS Tmp2, MzBitLeter
	LSR Tmp2	;Flasg[C] := Tmp2[0]; Tmp2[n]:= Tmp2[n+1]
	BRCC Dot
	;Do Tire
	RJMP ChoosEnd
Dot:
	
ChoosEnd:
	LDS Tmp2, MzBitCounter
	DEC Tmp2
	BRNE Exit
	;Букву достучали - следующая?
Exit:
	STS MzBitCounter, Tmp2
.ENDM