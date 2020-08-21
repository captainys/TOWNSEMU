#ifndef DISKIMG_IS_INCLUDED
#define DISKIMG_IS_INCLUDED
/* { */

#ifdef __cplusplus
#include <vector>
std::vector <unsigned char> Get1440KBFloppyDiskImage(void);
std::vector <unsigned char> Get1232KBFloppyDiskImage(void);
std::vector <unsigned char> Get720KBFloppyDiskImage(void);
std::vector <unsigned char> Get640KBFloppyDiskImage(void);

extern "C" {
#endif

extern const unsigned long long sizeof_FDIMG640KB;
extern const unsigned char FDIMG640KB[];

extern const unsigned long long sizeof_FDIMG720KB;
extern const unsigned char FDIMG720KB[];

extern const unsigned long long sizeof_FDIMG1232KB;
extern const unsigned char FDIMG1232KB[];

extern const unsigned long long sizeof_FDIMG1440KB;
extern const unsigned char FDIMG1440KB[];

#ifdef __cplusplus
} // extern "C"
#endif

/* } */
#endif
