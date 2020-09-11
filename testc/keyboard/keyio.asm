					ASSUME	CS:CODE

					PUBLIC	CLEAR_IF
					PUBLIC	SET_IF
					PUBLIC	RESETKEYBIOS
					PUBLIC	A0A1
					PUBLIC	A1A1
					PUBLIC	A1
					PUBLIC	A0



CODE					SEGMENT



CLEAR_IF				PROC
						CLI
						RET
CLEAR_IF				ENDP



SET_IF					PROC
						STI
						RET
SET_IF					ENDP



RESETKEYBIOS			PROC
						XOR		AH,AH
						INT		90H
						RET
RESETKEYBIOS			ENDP



A0A1					PROC
						PUSH	EBP
						MOV		EBP,ESP
						PUSH	ECX
						PUSH	EBX
						PUSH	EDX
						PUSH	EDI
						CLI

						MOV		DX,0602H
						MOV		AL,0A0H
						OUT		DX,AL
						CALL	DELAY
						MOV		AL,0A1H
						OUT		DX,AL

						MOV		EDI,[EBP+8]
						CALL	READ

						STI
						POP		EDI
						POP		EDX
						POP		EBX
						POP		ECX
						POP		EBP
						RET
A0A1					ENDP


A1A1					PROC
						PUSH	EBP
						MOV		EBP,ESP
						PUSH	ECX
						PUSH	EBX
						PUSH	EDX
						PUSH	EDI
						CLI

						MOV		DX,0602H
						MOV		AL,0A1H
						OUT		DX,AL
						CALL	DELAY
						MOV		AL,0A1H
						OUT		DX,AL

						MOV		EDI,[EBP+8]
						CALL	READ

						STI
						POP		EDI
						POP		EDX
						POP		EBX
						POP		ECX
						POP		EBP
						RET
A1A1					ENDP


A1						PROC
						PUSH	EBP
						MOV		EBP,ESP
						PUSH	ECX
						PUSH	EBX
						PUSH	EDX
						PUSH	EDI
						CLI

						MOV		DX,0602H
						MOV		AL,0A1H
						OUT		DX,AL

						MOV		EDI,[EBP+8]
						CALL	READ

						STI
						POP		EDI
						POP		EDX
						POP		EBX
						POP		ECX
						POP		EBP
						RET
A1						ENDP


A0						PROC
						PUSH	EBP
						MOV		EBP,ESP
						PUSH	ECX
						PUSH	EBX
						PUSH	EDX
						PUSH	EDI
						CLI

						MOV		DX,0602H
						MOV		AL,0A0H
						OUT		DX,AL

						MOV		EDI,[EBP+8]
						CALL	READ

						STI
						POP		EDI
						POP		EDX
						POP		EBX
						POP		ECX
						POP		EBP
						RET
A0						ENDP



DELAY					PROC
						PUSH	ECX
						MOV		ECX,10000
DELAY_LOOP:				LOOP	DELAY_LOOP
						POP		ECX
						RET
DELAY					ENDP



DELAY_WAIT_CODE			PROC
						PUSH	ECX
						MOV		ECX,10000
DELAY_LOOP2:
						MOV		DX,0602H
						IN		AL,DX
						AND		AL,1
						JNE		DELAY_LOOP2_BREAK
						LOOP	DELAY_LOOP2
DELAY_LOOP2_BREAK:
						POP		ECX
						RET
DELAY_WAIT_CODE			ENDP



READ					PROC

						XOR		EBX,EBX
						MOV		ECX,40*10000
READ_LOOP:
						MOV		DX,0602H
						IN		AL,DX
						AND		AL,1
						JE		READ_LOOP_NEXT

						MOV		DX,0600H
						IN		AL,DX
						MOV		BYTE PTR [EDI+EBX],AL
						ADD		EBX,1
						CMP		EBX,40
						JGE		READ_LOOP_BREAK
READ_LOOP_NEXT:
						LOOP	READ_LOOP

READ_LOOP_BREAK:
						MOV		EAX,EBX

						RET
READ					ENDP


CODE					ENDS
						END
