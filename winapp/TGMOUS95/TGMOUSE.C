/*
 * TGMOUSE for Windows 95
 *
 *
 * DESCRIPTION:
 *   Background Win32 executable for Windows 95 that notifies the current mouse
 *   position to the emulator for smooth mouse integration.
 *
 * ----------------------------------------------------------------------------
 * COMPILATION INSTRUCTIONS (Microsoft Visual C++ 2.0):
 * ----------------------------------------------------------------------------
 * 1. Open the Visual C++ 2.0 Command Prompt (or DOS prompt with environment
 *	variables set via VCVARS32.BAT).
 * 2. Run the following command:
 *
 *	   cl /W3 tgmouse.c user32.lib
 *
 * ----------------------------------------------------------------------------
 * INSTALLATION INSTRUCTIONS (Auto-Start at Boot):
 * ----------------------------------------------------------------------------
 * To make Windows 95 start this process automatically at system boot
 * (before user login):
 *
 * 1. Open Registry Editor (Start -> Run -> regedit.exe).
 * 2. Navigate to:
 *	   HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\RunServices
 *
 * 3. Create a new String Value (REG_SZ):
 *	   Value Name: TGMOUSE95
 *	   Value Data: C:\PATH\TO\TGMOUSE.EXE   (replace with actual file path)
 *
 * ============================================================================
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h> /* For _inp and _outp */

#define TOWNSIO_VM_HOST_IF_CMD_STATUS 0x2386
#define TOWNSIO_VM_HOST_IF_DATA	   0x2387

#define TOWNS_VMIF_CMD_NOTIFY_MOUSE   0x0A

/* Flag to control the monitoring loop */
static BOOL g_bRunning=TRUE;

/* Function pointer definition for Win95-specific API */
typedef DWORD(WINAPI *pfnRegisterServiceProcess)(DWORD dwProcessId, DWORD dwType);

/* Hidden Window Procedure to catch OS Shutdown messages */
LRESULT CALLBACK HiddenWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_QUERYENDSESSION:
			/* Return TRUE to tell Windows 95: "Yes, safe to shut down or restart" */
			return TRUE;

		case WM_ENDSESSION:
			/* If wParam is TRUE, Windows 95 is actively shutting down right now */
			if(wParam)
			{
				g_bRunning=FALSE; // Signal loop to exit
				PostQuitMessage(0);
			}
			return 0;

		case WM_DESTROY:
			g_bRunning=FALSE;
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	POINT pos;
	MSG msg;
	WNDCLASS wc;
	HWND hwndHidden;
	HMODULE hKernel;
	pfnRegisterServiceProcess rsp;

	/* 1. Register and create a completely hidden 0x0 window to handle OS messages */
	ZeroMemory(&wc,sizeof(wc));
	wc.lpfnWndProc  =HiddenWndProc;
	wc.hInstance    =hInstance;
	wc.lpszClassName="TGMOUSEHiddenClass";
	RegisterClass(&wc);

	hwndHidden=CreateWindow("TGMOUSEHiddenClass","",0,0,0,0,0,NULL,NULL,hInstance,NULL);

	/* 2. Hide process from Win95 Ctrl+Alt+Del Task List */
	hKernel=GetModuleHandle("KERNEL32.DLL");
	if(hKernel)
	{
		rsp =(pfnRegisterServiceProcess)GetProcAddress(hKernel,"RegisterServiceProcess");
		if(rsp)
		{
			rsp(0,1); /* RSP_SIMPLE_SERVICE */
		}
	}

	/* 
	 * 3. Priority elevation commented out as requested.
	 * Running at normal priority delivers ~15ms - 25ms timing while keeping
	 * the system balanced.
	 */
	/* SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_HIGHEST); */

	/* 4. Main Loop: Handles Windows messages AND I/O Port Monitoring */
	while(g_bRunning)
	{
		/* Process any pending OS messages(Shutdown,System events,etc.) */
		while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(WM_QUIT==msg.message)
			{
				g_bRunning=FALSE;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(!g_bRunning)
		{
			break;
		}

		/* Safely get mouse cursor position */
		GetCursorPos(&pos);

		_outp(TOWNSIO_VM_HOST_IF_DATA,pos.x);
		_outp(TOWNSIO_VM_HOST_IF_DATA,pos.x>>8);
		_outp(TOWNSIO_VM_HOST_IF_DATA,pos.y);
		_outp(TOWNSIO_VM_HOST_IF_DATA,pos.y>>8);
		_outp(TOWNSIO_VM_HOST_IF_CMD_STATUS,TOWNS_VMIF_CMD_NOTIFY_MOUSE);

		/* Yield CPU for ~10ms(Actual OS scheduling gap gives ~15ms - 25ms) */
		Sleep(20);
	}

	/* Clean up window before exiting */
	if(IsWindow(hwndHidden))
	{
		DestroyWindow(hwndHidden);
	}

	return 0;
}