#include "osinit.h"
#include <windows.h>
int OSInit(void)
{
    HMODULE h = LoadLibraryA("vcruntime140_1.dll");
    if (!h) {
        MessageBoxA(NULL,
            "vcruntime140_1.dll not found.\n"
            "Please install Visual C++ 2022 Redistributable Package and try again.\n"
            "Tsugaru tries to continue, but most likely stop or crash.",
            "Missing Runtime DLL", MB_ICONERROR);
        return 0;
    }

    FARPROC p = GetProcAddress(h, "__CxxFrameHandler4");
    if (!p) {
        MessageBoxA(NULL,
            "vcruntime140_1.dll is installed but does not have functions of VS2022.\n"
            "The version of the DLL VC++ is outdated.\n"
            "Please install the newest-version Visual C++ 2022 Redistributable Package and try again.\n"
            "Tsugaru tries to continue, but most likely stop or crash.",
            "Outdated Runtime DLL", MB_ICONERROR);
        FreeLibrary(h);
        return 0;
    }

    FreeLibrary(h);
    return 1;
}