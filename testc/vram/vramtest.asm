						.386p
						ASSUME	CS:CODE

						PUBLIC	CLEAR_VRAM
						PUBLIC	WRITE_TO_VRAM1
						PUBLIC	SEARCH_VRAM3

; GDT
SEG_PSP					EQU		0004H
SEG_CODE				EQU		000CH
SEG_DATA				EQU		0014H
SEG_PSP_MUTABLE			EQU		0024H
SEG_DOSENV				EQU		002CH
SEG_LOWMEM				EQU		0034H

; LDT
SEG_VRAM1				EQU		001CH	; 80000000H
SEG_VRAM2				EQU		0104H	; 80000000H
SEG_VRAM3				EQU		010CH	; 80100000H
SEG_SPRITERAM			EQU		0114H



VRAM_LENGTH				EQU		80000H



CODE					SEGMENT



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; void CLEAR_VRAM(void);
CLEAR_VRAM		PROC
				PUSHAD
				PUSH	ES

				MOV		AX,SEG_VRAM1
				MOV		ES,AX

				MOV		ECX,VRAM_LENGTH
				SUB		EDI,EDI
				SUB		AL,AL
				REP		STOSB

				POP		ES
				POPAD
				RET
CLEAR_VRAM		ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; void WRITE_TO_VRAM1(unsigned int offset);
WRITE_TO_VRAM1	PROC
				PUSH	EBP
				MOV		EBP,ESP
				PUSH	EAX
				PUSH	EDI
				PUSH	ES

				; [EBP+4] EIP  [EBP+8] 1st Param

				MOV		AX,SEG_VRAM1
				MOV		ES,AX
				MOV		EDI,[EBP+8]
				MOV		AL,0FFH
				MOV		ES:[EDI],AL

				POP		ES
				POP		EDI
				POP		EAX
				POP		EBP
				RET
WRITE_TO_VRAM1	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; unsigned int SEARCH_VRAM3(void);
SEARCH_VRAM3	PROC
				PUSH	EBP
				MOV		EBP,ESP
				PUSH	ESI
				PUSH	ECX
				PUSH	DS
				PUSHF

				CLI

				; [EBP+4] EIP  [EBP+8] 1st Param

				MOV		AX,SEG_VRAM3
				MOV		DS,AX
				SUB		ESI,ESI
				MOV		ECX,VRAM_LENGTH
SEARCH_VRAM3_LOOP:
				LODSB
				CMP		AL,0FFH
				JE		SEARCH_VRAM3_BREAK
				LOOP	SEARCH_VRAM3_LOOP

SEARCH_VRAM3_BREAK:
				LEA		EAX,[ESI-1]

				POPF
				POP		DS
				POP		ECX
				POP		ESI
				POP		EBP
				RET
SEARCH_VRAM3	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					ENDS
						END
