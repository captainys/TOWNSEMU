#ifndef IO_H_IS_INCLUDED
#define IO_H_IS_INCLUDED

enum
{
	TOWNSIO_VM_HOST_IF_CMD_STATUS=0x2386,
	TOWNSIO_VM_HOST_IF_DATA=      0x2387,
};
enum
{
	TOWNS_VMIF_CMD_NOP=           0x00, 
	TOWNS_VMIF_CMD_CAPTURE_CRTC=  0x01, // Capture CRTC. Followed by two bytes mode0, and mode1.
};

extern void IOWriteByte(unsigned int ioport,unsigned int byteData);
extern unsigned int IOReadByte(unsigned int ioport);

#endif
