#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

#define TOWNSIO_VM_HOST_IF_CMD_STATUS 0x2386
#define TOWNSIO_VM_HOST_IF_DATA       0x2387

#define TOWNS_VMIF_CMD_NOTIFY_MOUSE   0x0A

static LRESULT WINAPI WindowFunc(HWND wnd,UINT msg,WPARAM wp,LPARAM lp);
static HWND hWnd=NULL;

#define WINSTYLE WS_OVERLAPPED|WS_CAPTION|WS_VISIBLE|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN

static const char *WINCLASS="TSUGARU";
static const char *WINNAME="TSUGARU Mouse Integrator for Windows 3.1";

void OpenWindow(HINSTANCE inst)
{
	int x0=0;
	int y0=0;
	int wid=100;
	int hei=100;

	// int useMultiSampleBuffer==(int)opt.useMultiSampleBuffer;
	const char *windowName="TGMOUSE";

	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc=(WNDPROC)WindowFunc;
	wc.hInstance=(HINSTANCE)inst;
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hIcon=LoadIcon(inst,IDI_APPLICATION);
	wc.lpszMenuName="Menu";
	wc.hbrBackground=GetStockObject(WHITE_BRUSH);
	wc.style=0;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.lpszClassName=WINCLASS;
	if(0!=RegisterClass(&wc))
	{
		RECT rc;
		rc.left  =x0;
		rc.top   =y0;
		rc.right =(unsigned long)(x0+wid-1);
		rc.bottom=(unsigned long)(y0+hei-1);
		AdjustWindowRect(&rc,WINSTYLE,FALSE);
		wid  =rc.right-rc.left+1;
		hei  =rc.bottom-rc.top+1;

		const char *windowNameUsed=(const char *)(NULL!=windowName ? windowName : WINNAME);

		hWnd=CreateWindow(WINCLASS,windowNameUsed,WINSTYLE,x0,y0,wid,hei,NULL,NULL,inst,NULL);
		if(NULL!=hWnd)
		{
			ShowWindow(hWnd,SW_SHOWNORMAL);
			UpdateWindow(hWnd);
		}
		else
		{
			MessageBox(NULL,"Error! Could not open a window.","Error!",MB_OK);
			exit(1);
		}
	}
	else
	{
		MessageBox(NULL,"Error! Could not register class.","Error!",MB_OK);
		exit(1);
	}
}

static LRESULT WINAPI WindowFunc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp)
{
	switch(msg)
	{
	case WM_QUERYNEWPALETTE:
	case WM_PALETTECHANGED:
		return DefWindowProc(hWnd,msg,wp,lp);
	case WM_CREATE:
		break;
	case WM_CLOSE:
		return DefWindowProc(hWnd,msg,wp,lp);
	case WM_SIZE:
		break;
	case WM_PAINT:
		return DefWindowProc(hWnd,msg,wp,lp);
	case WM_COMMAND:
		break;
	case WM_DESTROY:
		hWnd=NULL;
		exit(1);
		break;
	// case WM_MOUSEWHEEL:  OMG  Mousewheel didn't exist days of Windows 3.1
		break;
	case WM_SYSCOMMAND:
		return DefWindowProc(hWnd,msg,wp,lp);
	case WM_SYSKEYDOWN:
		return DefWindowProc(hWnd,msg,wp,lp);
	case WM_SYSKEYUP:
		return 0;
	case WM_KEYDOWN:
		if(VK_ESCAPE==wp)
		{
			DestroyWindow(hWnd);
		}
		break;
	case WM_CHAR:
		break;
	case WM_ERASEBKGND:
		return DefWindowProc(hWnd,msg,wp,lp);

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		break;

	case WM_TIMER:
		POINT pos;
		GetCursorPos(&pos);

		_outp(TOWNSIO_VM_HOST_IF_DATA,pos.x);
		_outp(TOWNSIO_VM_HOST_IF_DATA,pos.x>>8);
		_outp(TOWNSIO_VM_HOST_IF_DATA,pos.y);
		_outp(TOWNSIO_VM_HOST_IF_DATA,pos.y>>8);
		_outp(TOWNSIO_VM_HOST_IF_CMD_STATUS,TOWNS_VMIF_CMD_NOTIFY_MOUSE);
		break;

	default:
		return DefWindowProc(hWnd,msg,wp,lp);
	}
	return 1;
}

/* int main(void)
{
	HINSTANCE inst=GetModuleHandle(NULL);
	OpenWindow(inst);
	for(;;)
	{
	}
	return 0;
} */

int PASCAL WinMain(HINSTANCE inst,HINSTANCE,LPSTR param,int)
{
	OpenWindow(inst);
	SetTimer(hWnd,NULL,10,NULL);
	while(NULL!=hWnd)
	{
		MSG msg;
		PeekMessage(&msg,NULL,0,0,1);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
