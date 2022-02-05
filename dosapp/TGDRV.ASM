						.386p
						ASSUME	CS:MAINSEG,DS:MAINSEG


TSUGARUIO_VM_HOST_IF_CMD_STATUS	EQU		02386h
TSUGARUIO_VM_HOST_IF_DATA		EQU		02387h


TOWNS_VMIF_CMD_PAUSE			EQU		02h
TOWNS_VMIF_CMD_NOTIFY_DOSSEG	EQU		07h
TOWNS_VMIF_CMD_NOTIFY_DOSVER	EQU		08h ; Capture DOS Version.  Use it immediately after INT 21H AH=30H.
TOWNS_VMIF_CMD_NOTIFY_DOSLOL	EQU		09h ; Capture DOS List of Lists.  Use it immediately after INT 21H AH=52H


TOWNSIO_VM_TGDRV				EQU		2388h
TOWNS_VM_TGDRV_INSTALL			EQU		01h
TOWNS_VM_TGDRV_INT2FH			EQU		02h

TOWNS_TGDRV_MAX_NUM_DRIVES		EQU		08h


MAINSEG					SEGMENT	USE16


						ORG		0100H
MAIN					PROC
						JMP		INSTALL

; Question: If someone calls TGDRV.COM twice, or TGDRV.SYS in CONFIG.SYS and then TGDRV.COM
; How can I tell it is already installed?

						ORG		0108H
NUM_TGDRV				DB		0                                 ; CS:108H
ASSIGNED_TGDRV_LETTERS	DB		TOWNS_TGDRV_MAX_NUM_DRIVES dup(0) ; CS:109H to CS:110H

MAIN					ENDP



;////////////////////////////////////////////////////////////

						ORG		0200H
INT2FH					PROC

						PUSH	BX
						PUSH	AX
						MOV		AH,51H ; GetPSP
						INT		21H
						MOV		CS:[CURRENT_PSP],BX
						POP		AX
						POP		BX

						PUSH	WORD PTR CS:[CURRENT_PSP]
						PUSH	DX
						PUSH	CX
						PUSH	BX
						PUSH	AX	; Tsugaru expects SS:SP is AX on write TOWNS_VM_TGDRV_INT2FH to TOWNSIO_VM_TGDRV.

						MOV		BX,AX
						MOV		DX,TOWNSIO_VM_TGDRV
						MOV		AL,TOWNS_VM_TGDRV_INT2FH
						OUT		DX,AL

						POP		AX
						POP		BX
						POP		CX
						POP		DX
						POP		WORD PTR CS:[CURRENT_PSP]

						JP		INT2FH_NOT_MY_DRIVE
						JC		INT2FH_IS_MY_DRIVE_BUT_ERROR

						PUSH	BP
						; SS:BP   BP
						; SS:BP+2 IP
						; SS:BP+4 CS
						; SS:BP+6 FLAGS
						MOV		BP,SP
						AND		WORD PTR SS:[BP+6],0FFFEh
						POP		BP
						IRET

INT2FH_IS_MY_DRIVE_BUT_ERROR:
						PUSH	BP
						MOV		BP,SP
						OR		WORD PTR SS:[BP+6],1
						POP		BP
						IRET

INT2FH_NOT_MY_DRIVE:
;						JMP		FAR [CS:ORIGINAL_2FH]		; --USE_IN_NASM--
						JMP		DWORD PTR CS:[ORIGINAL_2FH]	; --NOT_IN_NASM--

INT2FH					ENDP

CURRENT_PSP				DW		0
ORIGINAL_2FH			DD		0

END_OF_TSR:



;////////////////////////////////////////////////////////////



INSTALL					PROC
						MOV		AH,30H	; GET DOS VERSION
						INT		21H
						MOV		BX,AX

						MOV		DX,TSUGARUIO_VM_HOST_IF_CMD_STATUS
						MOV		AL,TOWNS_VMIF_CMD_NOTIFY_DOSVER
						OUT		DX,AL

						MOV		AH,52H
						INT		21H

						MOV		DX,TSUGARUIO_VM_HOST_IF_CMD_STATUS
						MOV		AL,TOWNS_VMIF_CMD_NOTIFY_DOSLOL
						OUT		DX,AL

						PUSH	CS
						POP		DS


						MOV		DX,TOWNSIO_VM_TGDRV
						MOV		AL,TOWNS_VM_TGDRV_INSTALL
						OUT		DX,AL

						JC		FAILED_INSTALL

						MOV		AX,352FH
						INT		21H
						MOV		WORD PTR CS:[ORIGINAL_2FH],BX
						MOV		WORD PTR CS:[ORIGINAL_2FH+2],ES

						MOV		DX,OFFSET INT2FH
						MOV		AX,252FH
						INT		21H

						MOV		DX,OFFSET END_OF_TSR
						ADD		DX,0FH
						SHR		DX,4

						LEA		DX,MSG_INSTALLED
						MOV		AH,09H
						INT		21H

						MOV		AH,31H
						INT		21H

FAILED_INSTALL:
						LEA		DX,MSG_NOT_INSTALLED
						MOV		AH,09H
						INT		21H

						MOV		AH,4CH
						INT 	21H

INSTALL					ENDP


MSG_INSTALLED			DB		"TGDRV has been installed.",0Dh,0Ah,0,'$'
MSG_NOT_INSTALLED		DB		"TGDRV has not been installed.",0Dh,0Ah,0,'$'



MAINSEG					ENDS

						END		MAIN
