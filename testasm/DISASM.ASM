						ASSUME	CS:CODE

						.386p
CODE					SEGMENT

						XCHG	BH,BYTE PTR [EDI+EAX]
						XCHG	DL,BYTE PTR [ESI+EBX]
						XCHG	CH,BYTE PTR [EBP+EAX]
						MOVSX	ECX,BYTE PTR [EDI+EAX]
						MOVZX	EDX,BYTE PTR [ESI+EDX]
						MOVSX	CX,BYTE PTR [EDI+EAX]
						MOVZX	DX,BYTE PTR [ESI+EDX]
						BSF		EBX,[ESI+EAX]
						BSR		EDX,[EDI+EBX]
						LAR		EAX,[ESI+EAX]
						IMUL	EAX,DWORD PTR [EBP]
						LEA		EDI,[EDI+EAX]
						LDS		ESI,[ECX]
						LDS		EDI,[ECX]
						LFS		EDI,[ECX]
						LGS		EDI,[ECX]
						LSS		EDI,[ECX]

						BT		[ESI+EAX],ECX
						BTC		[ESI+EAX],ECX
						BTS		[ESI+EAX],ECX
						BTR		[ESI+EAX],ECX
						SHLD	EAX,EBX,CL
						SHRD	EAX,EBX,CL



CODE					ENDS

						END
