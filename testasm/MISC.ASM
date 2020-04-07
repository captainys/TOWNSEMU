						ASSUME	CS:CODE

						.386p
CODE					SEGMENT

					  ; MOV     AL,[ESI+EDX*4+00001FFFH]
						ROL     CH,05H
						ROL     BL,03H
						SAR     EBX,07H
						RCL     EDX,0BH

						ROR     AH,1
						SHR     DH,1
						ROR     AL,CL
						SHR     CH,CL

						AAD
						AAM

						TEST    DH,0CH
						TEST    CX,000EH
						TEST    EBX,00000008H

						MOV     AL,10H
						MOV     BH,20H
						MOV     CL,33H
						MOV     DL,44H

						MOV     BYTE PTR [ESI],55H
						MOV     DWORD PTR [EDI],00001234H

						MOV     EAX,12345678H
						MOV     BX,5678H

						IN      AL,10H
						IN      AX,12H
						IN      EAX,77H

						INT     21H


CODE					ENDS

						END
