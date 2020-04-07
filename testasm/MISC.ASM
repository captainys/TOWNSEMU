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


						ADC     AX,0064H
						SBB     EAX,00008F00H
						TEST    AX,0000007FH
						XOR     EAX,00001234H


						ADC     AL,70H
						ADD     AL,71H
						AND     AL,72H
						CMP     AL,73H
						OR      AL,74H
						SBB     AL,75H
						SUB     AL,76H
						TEST    AL,77H
						XOR     AL,78H

						ADC     AX,0170H
						ADD     AX,0171H
						AND     AX,0172H
						CMP     AX,0173H
						OR      AX,0174H
						SBB     AX,0175H
						SUB     AX,0176H
						TEST    AX,0177H
						XOR     AX,0178H

						ADC     EAX,00000170H
						ADD     EAX,00000171H
						AND     EAX,00000172H
						CMP     EAX,00000173H
						OR      EAX,00000174H
						SBB     EAX,00000175H
						SUB     EAX,00000176H
						TEST    EAX,00000177H
						XOR     EAX,00000178H


						RET     0164H
						; RETF    0164H

						PUSH    DWORD PTR 00000164H
						PUSH    64H

						OUT     10H,AL
						OUT     12H,AX
						OUT     14H,EAX

						MOV     BYTE PTR [ESI],77H
						MOV     WORD PTR [ESI],7777H
						MOV     DWORD PTR [ESI],77777777H

						ADD     BYTE PTR [EDI],64H
						OR      DI,0084H
						ADC     SI,8801H
						SBB     DX,-10H
						AND     ECX,-20H
						SUB     BL,10H
						XOR     BH,20H
						CMP     BYTE PTR [EDI],98H

						PUSH    68H
						PUSH    DWORD PTR 00006809H
						PUSH    WORD PTR 6809H

CODE					ENDS

						END
