#define TOWNSIO_VM_HOST_IF_CMD_STATUS        0x2386
#define TOWNSIO_VM_HOST_IF_DATA              0x2387

#define TOWNS_VMIF_CMD_NOP                   0x00
#define TOWNS_VMIF_CMD_CAPTURE_CRTC          0x01
#define TOWNS_VMIF_CMD_PAUSE                 0x02
#define TOWNS_VMIF_CMD_EXIT_VM               0x03

int main(void)
{
	// Writing 02h to I/O 2386h will break Tsugaru.
	// Needs to be before including windows.h because windows.h is
	// stupid enough to define a keyword DX.
	_asm {
		PUSH	AX
		PUSH	DX

		MOV		AL,00H
		MOV		DX,TOWNSIO_VM_HOST_IF_DATA
		OUT		DX,AL

		MOV		AL,TOWNS_VMIF_CMD_EXIT_VM
		MOV		DX,TOWNSIO_VM_HOST_IF_CMD_STATUS
		OUT		DX,AL

		POP		DX
		POP		AX
	}

	return 0;
}
