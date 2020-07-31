						ASSUME	CS:CODE,DS:DATA,SS:STACK
						

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CODE					SEGMENT

MAIN					PROC

						MOV		AX,DATA
						MOV		DS,AX

						MOV		AX,00C0H
						XOR		CX,CX
						MOV		DL,4     ; 2048 bytes per sector
						INT 	93H

						MOV		AX,05C0H
						XOR		CX,CX
						MOV		CL,0
						MOV		DX,150
						MOV		BX,2
						LEA		DI,[DATA_BUF]
						INT 	93H

						MOV		AX,00C0H
						XOR		CX,CX
						MOV		DL,8     ; 2336 bytes per sector
						INT 	93H

						MOV		AX,05C0H
						XOR		CX,CX
						MOV		CL,0
						MOV		DX,151
						MOV		BX,2
						LEA		DI,[DATA_BUF]
						INT 	93H


						MOV		AX,00C0H
						XOR		CX,CX
						MOV		DL,9     ; 2340 bytes per sector
						INT 	93H

						MOV		AX,05C0H
						XOR		CX,CX
						MOV		CL,0
						MOV		DX,152
						MOV		BX,2
						LEA		DI,[DATA_BUF]
						INT 	93H

						MOV		AX,00C0H
						XOR		CX,CX
						MOV		DL,4     ; 2048 bytes per sector
						INT 	93H

						MOV		AX,05C0H
						XOR		CX,CX
						MOV		CL,0
						MOV		DX,153
						MOV		BX,2
						LEA		DI,[DATA_BUF]
						INT 	93H

EXIT:
						MOV		AH,4CH
						INT		21H

MAIN					ENDP

CODE					ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DATA					SEGMENT

DATA_BUF				DB		4096 dup (0)

DATA					ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

STACK					SEGMENT
						DB		128 dup (0)
STACK					ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

						END		MAIN
