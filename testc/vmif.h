#ifndef VMIF_IS_INCLUDED
#define VMIF_IS_INCLUDED
/* { */

enum
{
	TOWNSIO_VM_HOST_IF_CMD_STATUS=0x2386,
	TOWNSIO_VM_HOST_IF_DATA=      0x2387,
};
enum
{
	TOWNS_VMIF_CMD_NOP=           0x00, 
	TOWNS_VMIF_CMD_CAPTURE_CRTC=  0x01, // Capture CRTC. Followed by two bytes mode0, and mode1.
	TOWNS_VMIF_CMD_PAUSE=         0x02, // Pause VM
	TOWNS_VMIF_CMD_EXIT_VM=       0x03, // Exit the VM.  Return code taken from the data queu.
};

extern void TEST_FAILED(void);
extern void TEST_SUCCEEDED(void);

/* } */
#endif
