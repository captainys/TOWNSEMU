						.386p
						ASSUME	CS:CODE


DEBUG_BREAK_TOWNS_VM	MACRO
						MOV		DX,2386H
						MOV		AL,0002H
						OUT		DX,AL
						ENDM


						PUBLIC	TEST_LAR_LSL

LDT_SEG					EQU		0028H	; Set by DOS-Extender
LDT_WRITE_SEG			EQU		0030H	; Set by DOS-Extender
TEST_SEG				EQU		00A4H
TEST_SEG_OFFSET			EQU		00A0H


CODE					SEGMENT

;16/32 operand size
;0 to 1FH  descriptor type
;0 or 1 P flag
;RPL 0,1,2,3


; void TEST_LAR_LSL(
;     unsigned int res[4],int opSize,unsigned int descType,unsigned int GFlag,unsigned int Pflag,unsigned int DPL,unsigned int RPL);
; opSize 16 or 32
; descType 0 to 1FH
; GFlag 1 or 0
; PFlag 1 or 0
; RPL 0 to 3
; DPL 0 to 3
; res[0] ZF from LSL
; res[1] Returned 32-bit reg from LSL
; res[0] ZF from LAR
; res[1] Returned 32-bit reg from LAR

; [EBP+32] RPL given to LSL and LAR
; [EBP+28] DPL of the descriptor
; [EBP+24] Present flag
; [EBP+20] Granularity flag
; [EBP+16] descriptor Type
; [EBP+12] opSize 16 or 32
; [EBP+8]  Return Buffer
; [EBP+4]  Return EIP
; [EBP]  PrevEBP
TEST_LAR_LSL			PROC
						PUSH	EBP
						MOV		EBP,ESP
						PUSHAD
						PUSH	GS

						MOV		AX,LDT_WRITE_SEG
						MOV		GS,AX

						MOV		WORD PTR GS:[TEST_SEG_OFFSET],2345H
						MOV		BYTE PTR GS:[TEST_SEG_OFFSET+6],1
						CMP		BYTE PTR [EBP+20],0
						JE		NO_G_FLAG
						OR		BYTE PTR GS:[TEST_SEG_OFFSET+6],80H
NO_G_FLAG:

						MOV		AL,[EBP+28]            ; DPL
						AND		AL,3
						SHL		AL,5

						CMP		BYTE PTR [EBP+24],0   ; P-Flag
						JE		NO_P_FLAG
						OR		AL,80H
NO_P_FLAG:

						MOV		AH,[EBP+16]		; Type
						AND		AH,1FH
						OR		AL,AH
						MOV		GS:[TEST_SEG_OFFSET+5],AL


						MOV		BYTE PTR GS:[TEST_SEG_OFFSET+7],11H
						MOV		BYTE PTR GS:[TEST_SEG_OFFSET+4],22H
						MOV		WORD PTR GS:[TEST_SEG_OFFSET+2],3344H

						MOV		AX,LDT_SEG
						LLDT	AX

						; DEBUG_BREAK_TOWNS_VM



						MOVZX	EBX,BYTE PTR [EBP+32];
						AND		EBX,3
						OR		EBX,TEST_SEG

						MOV		ESI,[EBP+8]
						CMP		DWORD PTR [EBP+12],16
						JE		OPSIZE_16

						MOV		EAX,77777777H
						LSL		EAX,EBX
						MOV		[ESI],EAX
						SETZ	AL
						MOVZX	EAX,AL
						MOV		[ESI+4],EAX

						MOV		EAX,77777777H
						LAR		EAX,EBX
						MOV		[ESI+8],EAX
						SETZ	AL
						MOVZX	EAX,AL
						MOV		[ESI+12],EAX

						JMP		EXIT
OPSIZE_16:
						MOV		EAX,77777777H
						MOV		EAX,77777777H
						LSL		AX,BX
						MOV		[ESI],EAX
						SETZ	AL
						MOVZX	EAX,AL
						MOV		[ESI+4],EAX

						MOV		EAX,77777777H
						LAR		AX,BX
						MOV		[ESI+8],EAX
						SETZ	AL
						MOVZX	EAX,AL
						MOV		[ESI+12],EAX

EXIT:
						POP		GS
						POPAD
						POP		EBP
						RET
TEST_LAR_LSL			ENDP

CODE					ENDS

						END
