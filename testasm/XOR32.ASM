						ASSUME	CS:CODE

						.386p
CODE					SEGMENT

						XOR		AL,68H
						XOR		EAX,01234567H
						XOR		BL,77H
						XOR		BYTE PTR [EDI+EDX*4+01234567H],86H
						XOR		EBX,01234567H
						XOR		DWORD PTR [ESI+EAX*2+01234567H],89ABCDEFH

						XOR		ECX,0FFFFFF80H
						XOR		DWORD PTR [EBP+77777777H],0FFFFFF80H

						XOR		ECX,11H
						XOR		DWORD PTR [ESP+77777777H],11H

						XOR		BL,BH
						XOR		CH,CL
						XOR		EBX,ECX
						XOR		ECX,EBX

						XOR		BL,[EDI+11111111H]
						XOR		EDX,[ESI+11111111H]

CODE					ENDS

						END
