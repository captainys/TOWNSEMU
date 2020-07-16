						ASSUME	CS:CODE,DS:DATA
						

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CODE					SEGMENT

MAIN					PROC

						MOV		AX,00C0H
						XOR		CX,CX
						MOV		DL,4
						INT 	93H


						MOV		AX,DATA
						MOV		DS,AX

						MOV		AX,54C0H		; Get TOC
						XOR		CX,CX
						LEA		DI,[TOC_BUF]
						INT		93H

						CMP		AH,AH
						JNE		EXIT

						LEA		DI,[TOC_BUF+6]

						MOV		CL,1
FIND_FIRST_LOOP:		CMP		CL,BYTE PTR [TOC_BUF+2]
						JG		EXIT

						TEST	BYTE PTR [DI],80H
						JE		FIRST_AUDIO_TRACK

						LEA		DI,[DI+3]
						INC		CL
						JMP		FIND_FIRST_LOOP

FIRST_AUDIO_TRACK:
						MOV		AX,[DI]
						MOV		CL,[DI+2]
						MOV		WORD PTR [CMD_BUF],AX
						MOV		[CMD_BUF+2],CL

						ADD		AH,5
						CMP		AH,60

						JL		MINUTES_ADJUSTED
						SUB		AH,60
						INC		AL
MINUTES_ADJUSTED:		MOV		WORD PTR [CMD_BUF+3],AX
						MOV		[CMD_BUF+5],CL


						MOV		AX, 50C0H			; CDDA Play
						MOV		CX,0FF01H			; With repeat
						LEA		DI,[CMD_BUF]
						INT		93H


						MOV		AH,01H
						INT 	21H


EXIT:
						MOV		AH,4CH
						INT		21H

MAIN					ENDP

CODE					ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DATA					SEGMENT

CMD_BUF					DB		256 dup (0)
TOC_BUF					DB		4096 dup (0)

DATA					ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

						END		MAIN
