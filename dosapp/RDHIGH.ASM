; Accidentally found difference between 486 and Pentium.
;
; LGDT instruction only updates GDT, but linear base address, segment limit, and other properties cached in
; each segment selectors do not change.  Therefore, unless I don't use a far jump/call, it is safe to
; first transition to the protected mode, and then update GDT with LGDT.
;
; In Pentium, segment properties cached in each segment selectors apparently reset upon LGDT.
; Therefore, if I first transition to the protected mode, it is too late to update GDT with LGDT.
; As soon as LGDT is called, CS cache is reset, and the value of CS from the real mode is most likely
; not a valid code segment, and the program crashes.
;
; Thanks WINDY and Type.D for testing and confirming this finding with FM TOWNS II HC.



; Need .386p

; Input
;   ESI      Source Physical Address
;   ES:DI    Destination
;   CX       Transfer size

READ_HIGHB				PROC

; https://wiki.osdev.org/Unreal_Mode

						PUSH	ES
						PUSH	DS
						PUSH	EDI
						PUSH	ESI
						PUSH	EDX
						PUSH	ECX
						PUSH	EBX
						PUSH	EAX
						PUSHF

						;   ESI -> DX:AX
						MOV		AX,SI
						SHR		ESI,16
						MOV		DX,SI

						MOV		CS:[GDT_BASE_LOW+1],AH
						MOV		CS:[GDT_BASE_LOW+2],DL
						MOV		CS:[GDT_BASE_HIGH],DH
						MOV		AH,AL
						AND		AH,0F0H					; Align with 16-byte boundary.
						MOV		CS:[GDT_BASE_LOW],AH	; I don't know if segment boundary needs to be 4-byte alignment.
						AND		AX,000FH				; Let AL carry the low 4 bits.
						MOV		SI,AX
						; Now Source is 0008:SI


						MOV		DX,CS
						SHR		DX,12
						MOV		AX,CS
						SHL		AX,4
						ADD		AX,OFFSET GDT_BEGIN
						ADC		DX,0
						MOV		WORD PTR CS:[GDT_ADDR  ],AX
						MOV		WORD PTR CS:[GDT_ADDR+2],DX


						CLI

						SGDT	FWORD PTR CS:[GDT_BACKUP]
						LGDT	FWORD PTR CS:[GDT_INFO]

						MOV		EDX,CR0
						OR		DL,1
						MOV		CR0,EDX

						; https://wiki.osdev.org/Unreal_Mode suggest one jump "to tell 386/486 not to crash".
						JMP		@f
@@:

						AND		ESI,0000FFFFH
						AND		EDI,0000FFFFH
						AND		ECX,0000FFFFH


						; Quote from https://www.felixcloutier.com/x86/lgdt:lidt
						; They are the only instructions that directly load a linear address 
						; (that is, not a segment-relative address) and a limit in protected mode.
						MOV		AX,8	; Descriptor 1
						MOV		DS,AX
						REP		MOVSB


						AND		DL,0FEH
						MOV		CR0,EDX

						JMP		@f
@@:

						LGDT	FWORD PTR CS:[GDT_BACKUP]

						POPF
						POP		EAX
						POP		EBX
						POP		ECX
						POP		EDX
						POP		ESI
						POP		EDI
						POP		DS
						POP		ES
						RET

READ_HIGHB				ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
