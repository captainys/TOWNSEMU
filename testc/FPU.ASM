						ASSUME	CS:CODE

						PUBLIC	FLD_m64real
						PUBLIC	FLD_m80real
						PUBLIC	FSTP_m64real
						PUBLIC	FSTP_m80real
						PUBLIC	FDIV

CODE					SEGMENT


; void FLD_m64real(const double *ptr64)
; ESP+4  ptr64
; ESP    EIP
FLD_m64real				PROC

						MOV		EAX,[ESP+4]
						DB		09Bh,0DDh,000h	; FWAIT FLD m64real [EAX]
						RET

FLD_m64real				ENDP



; void FLD_m80real(const double *ptr80)
; ESP+4  ptr80
; ESP    EIP
FLD_m80real				PROC

						MOV		EAX,[ESP+4]
						DB		09Bh,0DBh,028h	; FWAIT FLD m80real [EAX]
						RET

FLD_m80real				ENDP



; void FSTP_m64real(void *buf8bytes)
; ESP+4  Buffer
; ESP    EIP
FSTP_m64real			PROC

						MOV		EAX,[ESP+4]
						DB		09Bh,0DDh,018h	; FWAIT FSTP m64real [EAX]
						RET

FSTP_m64real			ENDP



; void FSTP_m80real(void *buf10bytes)
; ESP+4  Buffer
; ESP    EIP
FSTP_m80real			PROC

						MOV		EAX,[ESP+4]
						DB		09Bh,0DBh,038h	; FWAIT FSTP m80real [EAX]
						RET

FSTP_m80real			ENDP



FDIV					PROC

						DB		09Bh,0DEh,0F9h  ; FDIV
						RET

FDIV					ENDP




CODE					ENDS
						END
