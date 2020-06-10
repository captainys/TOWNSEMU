#ifndef YSGAMEPAD_MACOS_OBJC_IS_INCLUDED
#define YSGAMEPAD_MACOS_OBJC_IS_INCLUDED
/* { */

#include "../ysgamepad.h"
void YsJoyReader_MacOS_InitializeController(void);
int YsJoyReader_MacOS_NumControllers(void);
void YsJoyReader_MacOS_ReadController(struct YsGamePadReading *reading,int controllerID);

/* } */
#endif
