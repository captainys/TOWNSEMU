;/* ////////////////////////////////////////////////////////////
;
;File Name: PHYSDUMP.ASM
;Copyright (c) 2019 Soji Yamakawa.  All rights reserved.
;http://www.ysflight.com
;
;Redistribution and use in source and binary forms, with or without modification, 
;are permitted provided that the following conditions are met:
;
;1. Redistributions of source code must retain the above copyright notice, 
;   this list of conditions and the following disclaimer.
;
;2. Redistributions in binary form must reproduce the above copyright notice, 
;   this list of conditions and the following disclaimer in the documentation 
;   and/or other materials provided with the distribution.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
;AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
;THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
;PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
;BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
;CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
;GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
;HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
;LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
;OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;//////////////////////////////////////////////////////////// */


						.386p
						ASSUME	CS:CODE,DS:DATA



CODE					SEGMENT		USE16


MAIN					PROC

						MOV		AX,DATA
						MOV		DS,AX
						MOV		ES,AX
						MOV		SI,OFFSET TITLE_LOGO
						CALL	PRINT_CSTR


						MOV		AH,62H	; GET PSP
						INT		21H

						PUSH	DS

						MOV		DS,BX
						MOV		SI,81H
						CALL	SKIP_SPACE_TAB
						CALL	XTOI

						CALL	PRINT_REGISTER
						CALL	PRINT_CRLF

						POP		DS



						XCHG	AX,DX
						CALL	PRINT_AX
						XCHG	AX,DX
						CALL	PRINT_AX
						CALL	PRINT_CRLF


						MOV		SI,DX			;ESI Phys addr
						SHL		ESI,16
						MOV		SI,AX
						LEA		DI,[DATABUF]	; ES:DI Read buffer
						MOV		CX,256			; Number of bytes
						CALL	READ_HIGHD_WITH_WAIT



						LEA		SI,[DATABUF]
						XOR		BX,BX

DUMP_OUTER_LOOP:
						LODSB
						CALL	PRINT_AL
						MOV		AL,' '
						CALL	PUTC
						INC		BL
						CMP		BL,16
						JB		DUMP_OUTER_LOOP

						MOV		AL,' '
						CALL	PUTC
						MOV		AL,'|'
						CALL	PUTC
						MOV		AL,' '
						CALL	PUTC


						SUB		SI,16
						MOV		BL,0
DUMP_ASCII_LOOP:		LODSB
						CALL	MAKEPRINTABLE
						CALL	PUTC
						INC		BL
						CMP		BL,16
						JB		DUMP_ASCII_LOOP


						CALL	PRINT_CRLF
						INC		BH
						MOV		BL,0
						CMP		BH,16
						JB		DUMP_OUTER_LOOP


						MOV		AH,4CH
						INT		21H
MAIN					ENDP



MAKEPRINTABLE			PROC
						CMP		AL,32
						JB		@f
						CMP		AL,126
						JA		@f
						RET
@@:
						MOV		AL,'.'
						RET
MAKEPRINTABLE			ENDP




SKIP_SPACE_TAB			PROC
						CLD
@@:						LODSB
						CMP		AL,' '
						JE		@b
						CMP		AL,9
						JE		@b
						SUB		SI,1
						RET
SKIP_SPACE_TAB			ENDP



; DS:SI  In   Pointer to the hexadecimal
; DX:AX  Out  32-bit value
; SI          Destroyed
XTOI					PROC
						XOR		DX,DX
						XOR		BX,BX

@@:						LODSB
						CALL	XCTOI
						JB		@f

						SHLD	DX,BX,4
						SHL		BX,4
						OR		BL,AL
						JMP		@b
@@:
						MOV		AX,BX
						RET
XTOI					ENDP



; AL In   ASCII code
; AL Out  0 to F
; CF Out  1 if not a hexadecimal
XCTOI					PROC
						CMP		AL,'0'
						JB		@f
						CMP		AL,'9'
						JA		@f
						SUB		AL,'0'
						CLC
						RET
@@:
						CMP		AL,'A'
						JB		@f
						CMP		AL,'F'
						JA		@f
						SUB		AL,'A'-10
						CLC
						RET
@@:
						CMP		AL,'a'
						JB		@f
						CMP		AL,'f'
						JA		@f
						SUB		AL,'a'-10
						CLC
						RET
@@:
						STC
						RET

XCTOI					ENDP



						INCLUDE		PRINT.ASM
						INCLUDE		RDHIGH.ASM
						INCLUDE		WRHIGH.ASM
						INCLUDE		GDT.ASM
CODE					ENDS



DATA					SEGMENT	USE16
TITLE_LOGO				DB		"PHYSDUMP by CaptainYS http://www.ysflight.com",0DH,0AH,0

DATABUF					DB		256 dup(0)

DATA					ENDS



STACK					SEGMENT	STACK USE16

						DB	1024 DUP(0)

STACK					ENDS


						END		MAIN
