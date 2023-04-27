						.386p
						ASSUME	CS:CODE


DEBUG_BREAK_TOWNS_VM	MACRO
						MOV		DX,2386H
						MOV		AL,0002H
						OUT		DX,AL
						ENDM


						PUBLIC	GET_DS_BASE_LINEAR
						PUBLIC	SET_UP_EXPDOWN_SEG
						PUBLIC	READ_FROM_SEG

LDT_SEG					EQU		0028H	; Set by DOS-Extender
LDT_WRITE_SEG			EQU		0030H	; Set by DOS-Extender
TEST_SEG				EQU		00A4H
TEST_SEG_OFFSET			EQU		00A0H


CODE					SEGMENT


; unsigned int GET_DS_BASE_LINEAR(void);
GET_DS_BASE_LINEAR		PROC

						PUSH	EBX
						PUSH	ECX

						MOV		BX,DS
						MOV		AX,2508H
						INT		21H
						MOV		EAX,ECX

						POP		ECX
						POP		EBX
						RET

GET_DS_BASE_LINEAR		ENDP



; Returns a Expand-Down Segment Selector.
; unsigned int SET_UP_EXPDOWN_SEG(unsigned int DorB,unsigned int Gbit,unsigned int baseLinear,unsigned int limit)
; [EBP+14H] Limit
; [EBP+10H] BaseLinear
; [EBP+0CH] Gbit
; [EBP+08H] DorB
; [EBP+04H] Return EIP
; [EBP]     PrevEBP
; [EBP-04H] Selector
SET_UP_EXPDOWN_SEG		PROC
						PUSH	EBP
						MOV		EBP,ESP
						PUSH	EAX		; Place holder for EAX
						PUSHAD
						PUSH	GS

						MOV		DWORD PTR [EBP-4],TEST_SEG

						MOV		AX,LDT_WRITE_SEG
						MOV		GS,AX


						MOV		EAX,[EBP+10H] ; BaseLinear
						MOV		GS:[TEST_SEG_OFFSET+2],AX
						SHR		EAX,16
						MOV		GS:[TEST_SEG_OFFSET+4],AL
						MOV		GS:[TEST_SEG_OFFSET+7],AH


						MOV		EAX,[EBP+14H] ; Limit
						MOV		GS:[TEST_SEG_OFFSET],AX
						SHR		EAX,16

						MOV		AH,[EBP+08H] ; D-bit
						AND		AH,1
						SHL		AH,6
						OR		AL,AH

						MOV		AH,[EBP+0CH] ; G-bit
						AND		AH,1
						SHL		AH,7
						OR		Al,AH

						MOV		GS:[TEST_SEG_OFFSET+6],AL


						MOV		BYTE PTR GS:[TEST_SEG_OFFSET+5],096H ; Present + Type 16H


						MOV		AX,LDT_SEG
						LLDT	AX


						DEBUG_BREAK_TOWNS_VM


						POP		GS
						POPAD
						POP		EAX
						POP		EBP
						RET

SET_UP_EXPDOWN_SEG		ENDP



; unsigned int READ_FROM_SEG(unsigned int selector,unsigned int offset)
READ_FROM_SEG			PROC
						PUSH	DS

						MOV		EAX,[ESP+8]
						MOV		DS,AX
						MOV		EAX,[ESP+0CH]
						MOV		EAX,[EAX]

						POP		DS
						RET
READ_FROM_SEG			ENDP


CODE					ENDS

						END
