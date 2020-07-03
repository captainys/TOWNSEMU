						ASSUME	CS:CODE

						.386p
CODE					SEGMENT


						ADC     EAX,EBX
						ADD     EAX,EBX
						AND     EAX,EBX
						CMP     EAX,EBX
						OR      EAX,EBX
						SBB     EAX,EBX
						SUB     EAX,EBX
						XOR     EAX,EBX

						ADC     BX,DX
						ADD     BX,DX
						AND     BX,DX
						CMP     BX,DX
						OR      BX,DX
						SBB     BX,DX
						SUB     BX,DX
						XOR     BX,DX

						ADC     BL,DH
						ADD     BL,DH
						AND     BL,DH
						CMP     BL,DH
						OR      BL,DH
						SBB     BL,DH
						SUB     BL,DH
						XOR     BL,DH

						ADC     EAX,[EBP+EDI*2+00000004H]
						ADD     EBX,[EBP+EDI*2+00000004H]
						AND     ECX,[EBP+EDI*2+00000004H]
						CMP     EDX,[EBP+EDI*2+00000004H]
						OR      ESI,[EBP+EDI*2+00000004H]
						SBB     EDI,[EBP+EDI*2+00000004H]
						SUB     EBP,[EBP+EDI*2+00000004H]
						XOR     ESP,[EBP+EDI*2+00000004H]

						ADC     DI,[EBP+EDI*2+00000004H]
						ADD     DI,[EBP+EDI*2+00000004H]
						AND     DI,[EBP+EDI*2+00000004H]
						CMP     DI,[EBP+EDI*2+00000004H]
						OR      DI,[EBP+EDI*2+00000004H]
						SBB     DI,[EBP+EDI*2+00000004H]
						SUB     DI,[EBP+EDI*2+00000004H]
						XOR     DI,[EBP+EDI*2+00000004H]

						ADC     AL,[EBP+EDI*2+00000004H]
						ADD     BH,[EBP+EDI*2+00000004H]
						AND     CL,[EBP+EDI*2+00000004H]
						CMP     DH,[EBP+EDI*2+00000004H]
						OR      AL,[EBP+EDI*2+00000004H]
						SBB     BH,[EBP+EDI*2+00000004H]
						SUB     CL,[EBP+EDI*2+00000004H]
						XOR     DH,[EBP+EDI*2+00000004H]


						ADC     [EBP+EDI*2+12345678H],EAX
						ADD     [EBP+EDI*2+12345678H],EBX
						AND     [EBP+EDI*2+12345678H],ECX
						CMP     [EBP+EDI*2+12345678H],EDX
						OR      [EBP+EDI*2+12345678H],ESI
						SBB     [EBP+EDI*2+12345678H],EDI
						SUB     [EBP+EDI*2+12345678H],EBP
						XOR     [EBP+EDI*2+12345678H],ESP

						ADC     [EBP+EDI*2+12345678H],DI
						ADD     [EBP+EDI*2+12345678H],DI
						AND     [EBP+EDI*2+12345678H],DI
						CMP     [EBP+EDI*2+12345678H],DI
						OR      [EBP+EDI*2+12345678H],DI
						SBB     [EBP+EDI*2+12345678H],DI
						SUB     [EBP+EDI*2+12345678H],DI
						XOR     [EBP+EDI*2+12345678H],DI

						ADC     [EBP+EDI*2+12345678H],AL
						ADD     [EBP+EDI*2+12345678H],BH
						AND     [EBP+EDI*2+12345678H],CL
						CMP     [EBP+EDI*2+12345678H],DH
						OR      [EBP+EDI*2+12345678H],AL
						SBB     [EBP+EDI*2+12345678H],BH
						SUB     [EBP+EDI*2+12345678H],CL
						XOR     [EBP+EDI*2+12345678H],DH


						AND     EAX,EAX
						AND     AX,AX
						AND     AH,AH
						AND     AL,AL

CODE					ENDS

						END
