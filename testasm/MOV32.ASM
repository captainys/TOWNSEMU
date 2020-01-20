						ASSUME	CS:CODE

						.386p
CODE					SEGMENT

						MOV     EAX,EDX
						MOV     EDX,EAX
						MOV     AL,[ESI+EDX*4+00001FFFH]
						MOV     AL,[EBP+EDX*4+00002FFFH]
						MOV     ECX,12345678H
						MOV     ESP,0ABABABABH

CODE					ENDS

						END
