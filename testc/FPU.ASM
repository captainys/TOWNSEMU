						ASSUME	CS:CODE

						PUBLIC	FLD_m64real
						PUBLIC	FLD_m80real

						PUBLIC	FSTP_m64real
						PUBLIC	FSTP_m80real

						PUBLIC	FDIVP_STi_ST
						PUBLIC	FDIVRP_STi_ST
						PUBLIC	FDIV_STi_ST
						PUBLIC	FDIVR_STi_ST
						PUBLIC	FDIV_ST_STi
						PUBLIC	FDIVR_ST_STi
						PUBLIC	FDIV_m64real
						PUBLIC	FDIVR_m64real

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



FDIVP_STi_ST				PROC

						DB		09Bh,0DEh,0F9h  ; FDIVP
						RET

FDIVP_STi_ST				ENDP




FDIVRP_STi_ST			PROC

						DB		09Bh,0DEh,0F1h  ; FDIVRP
						RET

FDIVRP_STi_ST			ENDP



FDIV_STi_ST				PROC

						DB		09Bh,0DCh,0F9h  ; FDIV
						RET

FDIV_STi_ST				ENDP




FDIVR_STi_ST			PROC

						DB		09Bh,0DCh,0F1h  ; FDIVR
						RET

FDIVR_STi_ST			ENDP



FDIV_ST_STi				PROC

						DB		09Bh,0D8h,0F1h  ; FDIV
						RET

FDIV_ST_STi				ENDP




FDIVR_ST_STi			PROC

						DB		09Bh,0D8h,0F9h  ; FDIVR
						RET

FDIVR_ST_STi			ENDP



FDIV_m64real			PROC

						MOV		EAX,[ESP+4]
						DB		09Bh,0DCh,030h	; FDIV [EAX]
						RET

FDIV_m64real			ENDP



FDIVR_m64real			PROC

						MOV		EAX,[ESP+4]
						DB		09Bh,0DCh,038h	; FDIVR [EAX]
						RET

FDIVR_m64real			ENDP





; For LST file
YSJUNK					PROC
						FDIV
						FDIVR
YSJUNK					ENDP



CODE					ENDS
						END
