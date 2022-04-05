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
						MOV		DI,OFFSET PHYSADDR_BUF
						CALL	COPY_WORD

						CALL	FIND_SPACE_TAB
						JC		INSUFFICIENT_PARAM

						CALL	SKIP_SPACE_TAB
						MOV		DI,OFFSET FILENAME_BUF
						CALL	COPY_WORD

						CALL	PRINT_REGISTER
						CALL	PRINT_CRLF

						POP		DS


						MOV		SI,OFFSET FILENAME_BUF
						CALL	PRINT_CSTR
						CALL	PRINT_CRLF



						MOV		DX,OFFSET FILENAME_BUF
						MOV		AX,3D00h
						INT		21H
						JC		CANNOT_OPEN_FILE

						; AX=File Handle
						PUSH	AX	; Save File Handle

						CALL	PRINT_AX
						CALL	PRINT_CRLF

						MOV		SI,OFFSET PHYSADDR_BUF
						CALL	XTOI

						XCHG	AX,DX
						CALL	PRINT_AX
						XCHG	AX,DX
						CALL	PRINT_AX
						CALL	PRINT_CRLF

						POP		BX	; File Handle=BX


						MOV		DI,DX
						Shl		EDI,16
						MOV		DI,AX

						; Need to keep BX and EDI
						; BX=File Handle
						; EDI=Phys addr
SEND_PHYSMEM_OUTER_LOOP:
						PUSH	BX
						PUSH	EDI

						MOV		AH,3FH
						MOV		CX,256
						MOV		DX,OFFSET DATABUF
						INT		21H

						JC		FILE_READ_ERROR

						ADD		AX,3
						MOVZX	ECX,AX		; Transfer size 
						PUSH	ECX			; Save transfer size

						SHR		ECX,2		; Make it for MOVSD
						JCXZ	SEND_PHYSMEM_DONE

						MOV		EDX,EDI
						SHR		EDX,16
						MOV		AX,DI

						MOV		SI,OFFSET DATABUF

						;   DX:AX    Physical Address  DX=High_16_bit  AX=Low_16_bit
						;   DS:SI    Source
						;   CX       Transfer size.  It uses MOVSB.

						CALL	WRITE_HIGHD

						POP		ECX
						POP		EDI
						POP		BX
						ADD		EDI,ECX
						JMP		SEND_PHYSMEM_OUTER_LOOP

SEND_PHYSMEM_DONE:
						POP		ECX
						POP		EDI
						POP		BX

						MOV		AH,3EH
						INT		21H


						MOV		SI,OFFSET DONE_TEXT
						CALL	PRINT_CSTR
						CALL	PRINT_CRLF

						MOV		AH,4CH
						INT		21H


INSUFFICIENT_PARAM:
						POP		DS
						MOV		SI,OFFSET USAGE_TEXT
						CALL	PRINT_CSTR

						MOV		AH,4CH
						INT		21H

CANNOT_OPEN_FILE:
						MOV		SI,OFFSET CANNOT_OPEN_FILE_TEXT
						CALL	PRINT_CSTR
						MOV		AH,4CH
						INT		21H


FILE_READ_ERROR:
						MOV		SI,OFFSET READ_ERROR_TEXT
						CALL	PRINT_CSTR
						CALL	PRINT_CRLF
						CALL	PRINT_AX
						CALL	PRINT_CRLF
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



COPY_WORD				PROC
						PUSH	SI
						PUSH	DI
						PUSH	AX

						CLD

COPY_WORD_LOOP:	LODSB
						CMP		AL,020H
						JBE		COPY_WORD_END
						STOSB
						JMP		COPY_WORD_LOOP
COPY_WORD_END:
						SUB		AL,AL
						STOSB

						POP		AX
						POP		DI
						POP		SI
						RET
COPY_WORD				ENDP



FIND_SPACE_TAB			PROC
						CLD
FIND_SPACE_TAB_LOOP:	LODSB
						CMP		AL,' '
						JE		SPACE_TAB_FOUND
						CMP		AL,9
						JE		SPACE_TAB_FOUND
						CMP		AL,0
						JE		SPACE_TAB_NOT_FOUND
						CMP		AL,0Ah
						JE		SPACE_TAB_NOT_FOUND
						CMP		AL,0Dh
						JE		SPACE_TAB_NOT_FOUND
						JMP		FIND_SPACE_TAB_LOOP
SPACE_TAB_FOUND:
						DEC		SI
						CLC
						RET
SPACE_TAB_NOT_FOUND:
						STC
						RET

FIND_SPACE_TAB			ENDP



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
TITLE_LOGO				DB		"PHYSWRIT by CaptainYS http://www.ysflight.com",0DH,0AH,0

USAGE_TEXT				DB		"Usage: PHYSWRIT physAddr file",0Dh,0Ah,0

CANNOT_OPEN_FILE_TEXT	DB		"Cannot Open File.",0Dh,0Ah,0

READ_ERROR_TEXT			DB		"File Read Error.",0Dh,0Ah,0

DONE_TEXT				DB		"Transfer Done.",0Dh,0Ah,0

PHYSADDR_BUF			DB		16 dup(0)
FILENAME_BUF			DB		256 dup(0)

DATABUF					DB		256 dup(0)

DATA					ENDS



STACK					SEGMENT	STACK USE16

						DB	1024 DUP(0)

STACK					ENDS


						END		MAIN
