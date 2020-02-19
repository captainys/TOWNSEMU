						ASSUME	CS:CODE

						PUBLIC	IOWriteByte
						PUBLIC	IOReadByte

CODE					SEGMENT

IOWriteByte				PROC
						PUSH	EDX

						MOV		EDX,[ESP+08H]
						MOV		EAX,[ESP+0CH]
						OUT		DX,AL

						POP		EDX
						RET
IOWriteByte				ENDP



IOReadByte				PROC
						PUSH	EDX

						MOV		EDX,[ESP+08H]
						XOR		EAX,EAX
						IN		AL,DX

						POP		EDX
						RET
IOReadByte				ENDP


CODE					ENDS
						END
