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

						INCLUDE TGDRVBUF.ASM


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
