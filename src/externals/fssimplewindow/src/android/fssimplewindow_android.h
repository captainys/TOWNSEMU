#ifndef FSSIMPLEWINDOW_ANDROID_IS_INCLUDED
#define FSSIMPLEWINDOW_ANDROID_IS_INCLUDED
/* { */

extern "C" const char *FsSimpleWindowConnector_GetFilesDir(void);
extern "C" const char *FsSimpleWindowConnector_GetCacheDir(void);

/*! Called from ysgles2view.cpp */
extern "C" void FsSimpleWindowConnector_NotifyFilesDirAndCacheDir(const char filesDir[],const char cacheDir[]);

/* } */
#endif
