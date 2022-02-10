						.386p
						ASSUME	CS:MAINSEG,DS:MAINSEG



						INCLUDE TGDRVDEF.ASM



MAINSEG					SEGMENT	USE16




;;;;;;;; Entrance for .COM ;;;;;;;;

						ORG		0100H
MAIN					PROC
						JMP		COM_MAIN
MAIN					ENDP

						ORG		0108H
NUM_TGDRV				DB		0                                    ; CS:108H
ASSIGNED_TGDRV_LETTERS	DB		TOWNS_TGDRV_MAX_NUM_DRIVES dup(0FFh) ; CS:109H to CS:110H
ERRMSG_BUFFER			DB		256 dup('$')                         ; CS:111H to CS:210H




;////////////////////////////////////////////////////////////


COM_MAIN				PROC
						PUSH	DS
						POP		ES
						MOV		SI,0080h

						MOVZX	BX,ES:[SI]
						INC		SI
						MOV		BYTE PTR ES:[SI+BX],0 ; Make it Zero-Terminated

						CALL	INSTALL
						JC		COM_MAIN_ERR

						MOV		DX,OFFSET END_OF_TSR
						ADD		DX,0FH
						SHR		DX,4

						MOV		AH,31H
						INT		21H

COM_MAIN_ERR:
						MOV		AH,4CH
						INT 	21H

COM_MAIN				ENDP



						INCLUDE	TGDRVMAI.ASM



MAINSEG					ENDS

						END		MAIN
