						.386p
						ASSUME	CS:CODE


DEBUG_BREAK_TOWNS_VM	MACRO
						MOV		DX,2386H
						MOV		AL,0002H
						OUT		DX,AL
						ENDM


						PUBLIC	TEST_SCSI_ERR


CODE					SEGMENT

; void TEST_SCSI_ERR(int *numStatus,unsigned char status[1024*1024],unsigned int time[1024*1024]);
; EBP+16	time[]
; EBP+12	status[]
; EBP+ 8	*numStatus
; EBP+ 4	returnEIP
; EBP   	prevEBP
TEST_SCSI_ERR			PROC

						PUSH	EBP
						MOV		EBP,ESP
						PUSHAD
						PUSH	ES

						MOV		AX,DS
						MOV		ES,AX

						MOV		EBX,[EBP+8]
						MOV		DWORD PTR [EBX],0

						MOV		EDI,[EBP+0CH]
						MOV		ESI,[EBP+10H]

						MOV		ECX,1024*1024

						CLI
						MOV		DX,0C30H
						MOV		AL,81H
						OUT		DX,AL

						MOV		DX,0C32H
						MOV		AL,0C6H
						OUT		DX,AL

						XOR		AH,AH
IOLOOP:
						OUT		6CH,AL
						OUT		6CH,AL
						IN		AL,DX
						CMP		AL,AH
						JE		LOOPNEXT

						INC		DWORD PTR [EBX]
						STOSB
						MOV		AH,AL
						MOV		DWORD PTR [ESI],1024*1024
						SUB		DWORD PTR [ESI],ECX
LOOPNEXT:
						LOOP	IOLOOP

LOOPBREAK:
						STI

						POP		ES
						POPAD
						POP		EBP
						RET

TEST_SCSI_ERR			ENDP

CODE					ENDS

						END
