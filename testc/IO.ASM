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



IOWriteWord				PROC
						PUSH	EDX

						MOV		EDX,[ESP+08H]
						MOV		EAX,[ESP+0CH]
						OUT		DX,AX

						POP		EDX
						RET
IOWriteWord				ENDP



IOReadByte				PROC
						PUSH	EDX

						MOV		EDX,[ESP+08H]
						XOR		EAX,EAX
						IN		AL,DX

						POP		EDX
						RET
IOReadByte				ENDP



IOReadWord				PROC
						PUSH	EDX

						MOV		EDX,[ESP+08H]
						XOR		EAX,EAX
						IN		AX,DX

						POP		EDX
						RET
IOReadWord				ENDP


CODE					ENDS
						END
