								; INCLUDED FROM TGDRVCOM.ASM and TGDRVSYS.ASM

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

DPB_LENGTH_DOS6					EQU		21h
