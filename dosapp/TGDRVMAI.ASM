						; INCLUDED FROM TGDRVCOM.ASM and TGDRVSYS.ASM

						ORG		0200H
INT2FH					PROC

						PUSH	BX
						PUSH	AX
						; SS:SP     AX
						; SS:SP+2   BX
						; SS:SP+4   IP
						; SS:SP+6   CS
						; SS:SP+8   FLAGS
						; SS:SP+0Ah Param0
						; SS:SP+0Ch Param1
						; SS:SP+0Eh Param2
						; SS:SP+10h Param3

						MOV		AH,51H ; GetPSP
						INT		21H
						MOV		CS:[CURRENT_PSP],BX

						MOV		BX,SP
						MOV		AX,SS:[BX+0AH]
						MOV		CS:[STACKPARAM0],AX
						MOV		AX,SS:[BX+0CH]
						MOV		CS:[STACKPARAM1],AX
						MOV		AX,SS:[BX+0EH]
						MOV		CS:[STACKPARAM2],AX
						MOV		AX,SS:[BX+10H]
						MOV		CS:[STACKPARAM3],AX

						POP		AX
						POP		BX

						PUSH	WORD PTR CS:[STACKPARAM3]
						PUSH	WORD PTR CS:[STACKPARAM2]
						PUSH	WORD PTR CS:[STACKPARAM1]
						PUSH	WORD PTR CS:[STACKPARAM0]
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
						POP		WORD PTR CS:[STACKPARAM0]
						POP		WORD PTR CS:[STACKPARAM1]
						POP		WORD PTR CS:[STACKPARAM2]
						POP		WORD PTR CS:[STACKPARAM3]

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
STACKPARAM0				DW		0
STACKPARAM1				DW		0
STACKPARAM2				DW		0
STACKPARAM3				DW		0

END_OF_TSR:



;////////////////////////////////////////////////////////////



INSTALL					PROC

						PUSH	ES	; Save command parameter pointer
						PUSH	SI


						PUSH	CS
						POP		DS

						LEA		DX,MSG_INTRO
						MOV		AH,09H
						INT		21H

						MOV		AX,6809h ; TGDRV Installation Check
						INT		2FH
						JC		INSTALL_CHECKED

						LEA		DX,MSG_ALREADY_INSTALLED
						CMP		AX,4754H ; "TG"
						JE		ERRMSG_AND_EXIT

INSTALL_CHECKED:
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


						POP		SI	; Restore command patemter
						POP		ES	; ES:SI needs to be command parameter

						MOV		DI,OFFSET ERRMSG_BUFFER  ; DS:DI Error Message Buffer.

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

						LEA		DX,MSG_INSTALLED
						MOV		AH,09H
						INT		21H

						MOVZX	CX,BYTE PTR CS:[NUM_TGDRV]
						LEA		SI,ASSIGNED_TGDRV_LETTERS
DRIVE_MSG_LOOP:
						PUSH	CX
						PUSH	SI

						LODSB
						MOV		CS:[MSG_DRIVES+6],AL

						LEA		DX,MSG_DRIVES
						MOV		AH,09H
						INT		21H

						POP		SI
						POP		CX
						LOOP	DRIVE_MSG_LOOP


						CLC
						RET


FAILED_INSTALL:
						LEA		DX,MSG_NOT_INSTALLED
ERRMSG_AND_EXIT:
						MOV		AH,09H
						INT		21H

						STC
						RET


INSTALL					ENDP


MSG_INTRO				DB		"TSUGARU HOST-VM File Sharing Driver TGDRV",0Dh,0Ah
						DB		"By CaptainYS  http://www.ysflight.com",0Dh,0Ah
						DB		"Version 20220209",0Dh,0Ah,0,'$'
MSG_ALREADY_INSTALLED	DB		"TGDRV has already beein installed.",0Dh,0Ah,0,'$'
MSG_INSTALLED			DB		"TGDRV has been installed.",0Dh,0Ah,0,'$'
MSG_NOT_INSTALLED		DB		"TGDRV has not been installed.",0Dh,0Ah,0,'$'
MSG_DRIVES				DB		"Drive  ",0Dh,0Ah,0,'$'
