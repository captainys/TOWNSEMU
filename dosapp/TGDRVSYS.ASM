						.386p
						ASSUME	CS:MAINSEG,DS:MAINSEG



						INCLUDE TGDRVDEF.ASM



MAINSEG					SEGMENT	USE16
						ORG		0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DEVHDR_LINKINFO			DD		-1
DEVHDR_ATTRIB			DW		0C800H	; I am not sure if it is good for this driver.
DEVHDR_STRATEGY			DW		STRATEGY
DEVHDR_INTERRUPT		DW		INTERRUPT0
; Careful about DEVHDR_NAME
; If it is same as the basename of the driver file, driver file gets write-protected.
; Or, this DEVHDR_NAME will become a virtual file in the DOS.
DEVHDR_NAME				DB		'TGDRIVE_'
DEVHDR_NOT_USED			DW		0
DEVHDR_DRIVE_LETTER		DB		0
DEVHDR_NUM_DRIVE		DB		0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PACKET					DD		0
NOT_USED_AT_THIS_TIME	DD		0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

						ORG		0108H
NUM_TGDRV				DB		0                                    ; CS:108H
ASSIGNED_TGDRV_LETTERS	DB		TOWNS_TGDRV_MAX_NUM_DRIVES dup(0FFh) ; CS:109H to CS:110H
ERRMSG_BUFFER			DB		256 dup('$')                         ; CS:111H to CS:210H


STRATEGY				PROC	FAR
						MOV		WORD PTR CS:[PACKET],BX
						MOV		WORD PTR CS:[PACKET+2],ES
						RET
STRATEGY				ENDP


INTERRUPT0				PROC	FAR

						PUSH	ES
						PUSH	DS
						PUSHA

						LES		SI,CS:[PACKET]	; ES:SI=Pointer to Command Struct
						LES		SI,ES:[SI+18]	; ES:SI=Command String
						CALL	INSTALL

						LES		DI,CS:[PACKET]

						MOV		WORD PTR ES:[DI+3],0100H		; Status
						MOV		BYTE PTR ES:[DI+13],0			; Number of Drives
						MOV		WORD PTR ES:[DI+14],OFFSET END_OF_TSR
						MOV		WORD PTR ES:[DI+16],CS

						POPA
						POP		DS
						POP		ES

						RET

INTERRUPT0				ENDP



						INCLUDE	TGDRVMAI.ASM



MAINSEG					ENDS

						END
