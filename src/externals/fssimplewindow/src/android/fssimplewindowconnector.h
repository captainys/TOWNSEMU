#ifndef FSSIMPLEWINDOWCONNECTOR_IS_INCLUDED
#define FSSIMPLEWINDOWCONNECTOR_IS_INCLUDED
/* { */

void FsSimpleWindowConnector_BeforeBigBang(void);
void FsSimpleWindowConnector_Initialize(void);
void FsSimpleWindowConnector_NotifyWindowSize(int wid,int hei);
void FsSimpleWindowConnector_NotifyMouseEvent(int evt,int lb,int mb,int rb,int mx,int my);
void FsSimpleWindowConnector_NotifyCharIn(int unicode);
void FsSimpleWindowConnector_NotifyTouchState(int nTouch,const int touchXY[]);

/* } */
#endif
