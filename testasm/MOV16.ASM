						ASSUME	CS:CODE

						.8086
CODE					SEGMENT

						MOV     AX,DX
						MOV     DX,AX
						MOV     AL,[BX+SI+1FFFH]
						MOV     AL,[BP+DI+2FFFH]
						MOV     CX,5678H
						MOV     SP,9876H

CODE					ENDS

						END
