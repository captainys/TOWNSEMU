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


CODE					ENDS

						END
