						.386p
						ASSUME	CS:CODE,DS:CODE,SS:STACK


CODE					SEGMENT	USE16

MAIN					PROC
						PUSH	CS
						POP		DS
						MOV		DI,OFFSET BUF

						MOV		AH,5
						INT		0AFH

						MOV		AX,WORD PTR CS:[BUF+5]
						CALL	PRINT_AX

						MOV		AH,4CH
						INT		21H

MAIN					ENDP


						INCLUDE PRINT.ASM

BUF						DB	16 dup(0)


CODE					ENDS


STACK					SEGMENT
						DB	256 dup(0)
STACK					ENDS


						END		MAIN




