#include "fssimplewindow.h"
#include <stdio.h>

int main(void)
{
    int rgb=0;

    FsOpenWindow(0,0,800,600,1);
    for(;;)
    {
        FsPollDevice();
        auto key=FsInkey();
        if(FSKEY_F==key)
        {
            FsMakeFullScreen();
        }
        else if(FSKEY_R==key)
        {
            FsUnmaximizeWindow();
        }
        else if(FSKEY_M==key)
        {
            FsMaximizeWindow();
        }
        else if(FSKEY_ESC==key)
        {
            break;
        }

        rgb+=10;
        float r=float((rgb>>16)&255)/256.0;
        float g=float((rgb>>8)&255)/256.0;
        float b=float(rgb&255)/256.0;
        glClearColor(r,g,b,1);

        int wid,hei;
        FsGetWindowSize(wid,hei);
        glViewport(0,0,wid,hei);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        FsSwapBuffers();
    }
    return 0;
}