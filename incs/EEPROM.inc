EEPROMRead:
ERL01:	sbic	EECR,EEWE	;if EEWE not clear
		rjmp	ERL01		; wait more 

		out		EEARH, ZH	;output address 
		out		EEARL, ZL	;output address 

		sbi		EECR, EERE	;set EEPROM Read strobe
		in		OSRG, EEDR	;get data
		ret
	
EEPROMWrite:
EWL01:	sbic	EECR,EEWE	;if EEWE not clear
		rjmp	EWL01		;    wait more
	
		out		EEARH, ZH	
		out		EEARL, ZL	

		out		EEDR,OSRG	;output data
		sbi		EECR,EEMWE	;set EEPROM Master Write Enable
		sbi		EECR,EEWE	;set EEPROM Write strobe
		ret