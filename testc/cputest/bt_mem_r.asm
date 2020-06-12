						.386p
						ASSUME	CS:CODE

						PUBLIC	BT_MEM_R32
						PUBLIC	BT_MEM_R16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



EFLAGS_CF   	   		EQU		00001H
EFLAGS_PF   	  		EQU		00004H
EFLAGS_AF  				EQU		00010H
EFLAGS_ZF       		EQU		00040H
EFLAGS_SF       		EQU		00080H
EFLAGS_TRAP       		EQU		00100H
EFLAGS_IF 				EQU		00200H
EFLAGS_DF  				EQU		00400H
EFLAGS_OF  	 			EQU		00800H
EFLAGS_IOPL       		EQU		03000H
EFLAGS_NF   	  		EQU		04000H
EFLAGS_RF   	  		EQU		10000H
EFLAGS_VF		  		EQU		20000H
EFLAGS_ALIGN_CHECK		EQU		40000H



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					SEGMENT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; void BT_MEM_R32(unsigned char res[512],const unsigned char data[64]);

BT_MEM_R32				PROC
						PUSH	EBP		; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=DataPtr,  [EIP+16]=EAX
						MOV		EBP,ESP
						PUSHAD
						PUSH	ES

						PUSH	DS
						POP		ES

						MOV		EDI,[EBP+8]   ; Result Ptr
						MOV		ESI,[EBP+12]  ; Data Ptr
						MOV		ECX,0

BT_MEM_R32_LOOP:		BT		[ESI],ECX
						SETB	AL
						STOSB
						INC		ECX
						CMP		ECX,512
						JNE		BT_MEM_R32_LOOP

						POP		ES
						POPAD
						POP		EBP
						RET
BT_MEM_R32				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; void BT_MEM_R16(unsigned char res[512],const unsigned char data[64]);

BT_MEM_R16				PROC
						PUSH	EBP		; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=DataPtr,  [EIP+16]=EAX
						MOV		EBP,ESP
						PUSHAD
						PUSH	ES

						PUSH	DS
						POP		ES

						MOV		EDI,[EBP+8]   ; Result Ptr
						MOV		ESI,[EBP+12]  ; Data Ptr
						MOV		ECX,0

BT_MEM_R16_LOOP:		BT		[ESI],CX
						SETB	AL
						STOSB
						INC		CX
						CMP		CX,512
						JNE		BT_MEM_R16_LOOP

						POP		ES
						POPAD
						POP		EBP
						RET
BT_MEM_R16				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					ENDS
						END
