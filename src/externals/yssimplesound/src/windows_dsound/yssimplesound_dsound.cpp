#include <algorithm>

#include <stdio.h>

#define _WINSOCKAPI_
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#include "yssimplesound.h"


#pragma comment(lib,"user32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"dsound.lib")


#ifdef min
#undef min // ****ing windows.h namespace contamination.
#endif


class YsSoundPlayer::APISpecificData
{
public:
	/*!
	Why do I have to find a window handle?  It is because DirectSound API is so badly designed that it requires a window handle.
	What's even more stupid is that the window needs to be active and forward to play sound.  I cannot get away with creating a
	dummy hidden window.  After wasting days of google search, I concluded that DirectSound API is designed by an incompetent
	programmer.

	Window and sound need to be independent.  Sound can optionally be associated with a window, but the association must not be mandatory.
	Unrelated modules must be independent.  It is such a basic.

	I mostly write a single-window application.  Therefore I can live with it.  But, if there are multiple windows, the sound
	stops when a window that is associated with DirectSound becomes inactive.

	DirectSound is like an opps library.  It might really be designed by an inexperienced graduate student.  Who knows.
	*/
	class MainWindowFinder
	{
	public:
		/*! This function finds a handle of the largest window that is visible and is associated with the current process Id.
		    This window may not be an application main window.  But, what else can I do?
		    This function can be used for giving a window handle to a badly-designed APIs such as Direct Sound API.
		*/
		static HWND Find(HWND hExcludeWnd=nullptr);
	private:
		static void SearchTopLevelWindow(HWND &hWndLargest,int &largestWndArea,HWND hWnd,DWORD procId,HWND hWndExclude);
	};


	HWND hWndMain;
	HWND hOwnWnd;
	LPDIRECTSOUND8 dSound8;

	APISpecificData();
	~APISpecificData();
	void CleanUp(void);
	void Start(void);
	void End(void);

	void RefetchMainWindowHandle(void);

private:
	void OpenDummyWindow(void);
	void DestroyDummyWindow(void);
	static LRESULT WINAPI OwnWindowFunc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp);
};

class YsSoundPlayer::SoundData::APISpecificDataPerSoundData
{
public:
	LPDIRECTSOUNDBUFFER dSoundBuf;

	APISpecificDataPerSoundData();
	~APISpecificDataPerSoundData();
	void CleanUp(void);

	void CreateBuffer(LPDIRECTSOUND8 dSound8,SoundData &dat);
};



////////////////////////////////////////////////////////////



// Source: http://stackoverflow.com/questions/6202547/win32-get-main-wnd-handle-of-application
HWND YsSoundPlayer::APISpecificData::MainWindowFinder::Find(HWND hExcludeWnd)
{
	HWND hWndLargest=NULL;
	int wndArea=0;
	SearchTopLevelWindow(hWndLargest,wndArea,NULL,GetCurrentProcessId(),hExcludeWnd);
	return hWndLargest;
}
void YsSoundPlayer::APISpecificData::MainWindowFinder::SearchTopLevelWindow(HWND &hWndLargest,int &largestWndArea,HWND hWnd,DWORD procId,HWND hWndExclude)
{
	if(nullptr!=hWndExclude && hWnd==hWndExclude)
	{
		return;
	}

	DWORD windowProcId;
	GetWindowThreadProcessId(hWnd,&windowProcId);
	if(windowProcId==procId)
	{
		char str[256];
		GetWindowTextA(hWnd,str,255);
		printf("hWnd=%08x Title=%s\n",(int)hWnd,str);
		printf("IsVisible=%d\n",IsWindowVisible(hWnd));

		RECT rc;
		GetWindowRect(hWnd,&rc);

		int area=((rc.right-rc.left)*(rc.bottom-rc.top));
		if(0>area)
		{
			area=-area;
		}
		printf("Area=%d square pixels. (%d x %d)\n",area,(rc.right-rc.left),(rc.bottom-rc.top));

		if(TRUE==IsWindowVisible(hWnd) && (NULL==hWndLargest || largestWndArea<area))
		{
			hWndLargest=hWnd;
			largestWndArea=area;
		}
		return;
	}

	HWND hWndChild=NULL;
	while(NULL!=(hWndChild=FindWindowEx(hWnd,hWndChild,NULL,NULL))!=NULL)
	{
		SearchTopLevelWindow(hWndLargest,largestWndArea,hWndChild,procId,hWndExclude);
	}
}

////////////////////////////////////////////////////////////



YsSoundPlayer::APISpecificData::APISpecificData()
{
	hWndMain=nullptr;
	hOwnWnd=nullptr;
	dSound8=nullptr;
	CleanUp();
}
YsSoundPlayer::APISpecificData::~APISpecificData()
{
	CleanUp();
}

void YsSoundPlayer::APISpecificData::CleanUp(void)
{
	hWndMain=nullptr;
	if(nullptr!=dSound8)
	{
		dSound8->Release();
		dSound8=nullptr;
	}

	DestroyDummyWindow();
}

void YsSoundPlayer::APISpecificData::Start(void)
{
	CleanUp();

	MainWindowFinder mainWindowFinder;
	hWndMain=mainWindowFinder.Find();
	if(nullptr==hWndMain && nullptr==hOwnWnd)
	{
		OpenDummyWindow();
	}
	if(DS_OK==DirectSoundCreate8(NULL,&dSound8,NULL))
	{
		if(nullptr!=hWndMain)
		{
			dSound8->SetCooperativeLevel(hWndMain,DSSCL_PRIORITY);
		}
		else if(nullptr!=hOwnWnd)
		{
			dSound8->SetCooperativeLevel(hOwnWnd,DSSCL_PRIORITY);
		}
	}
}
void YsSoundPlayer::APISpecificData::End(void)
{
	CleanUp();
}

void YsSoundPlayer::APISpecificData::RefetchMainWindowHandle(void)
{
	if(nullptr!=hOwnWnd && nullptr==hWndMain)
	{
		MainWindowFinder mainWindowFinder;
		hWndMain=mainWindowFinder.Find(hOwnWnd);
		if(nullptr!=hWndMain)
		{
			dSound8->SetCooperativeLevel(hWndMain,DSSCL_PRIORITY);
			DestroyDummyWindow();
		}
	}
}

////////////////////////////////////////////////////////////

YsSoundPlayer::APISpecificData *YsSoundPlayer::CreateAPISpecificData(void)
{
	return new APISpecificData;
}
void YsSoundPlayer::DeleteAPISpecificData(APISpecificData *ptr)
{
	delete ptr;
}

YSRESULT YsSoundPlayer::StartAPISpecific(void)
{
	api->Start();
	return YSOK;
}
YSRESULT YsSoundPlayer::EndAPISpecific(void)
{
	api->End();
	return YSOK;
}

void YsSoundPlayer::SetVolumeAPISpecific(SoundData &dat,float leftVol,float rightVol)
{
	if(nullptr!=dat.api->dSoundBuf)
	{
		float vol=(leftVol+rightVol)/2.0;

		// left=1, right=0 -> DSBPAN_LEFT
		// left=0, right=1 -> DSBPAN_RIGHT
		LONG pan=0;
		if(vol<0.001)
		{
		}
		else if(leftVol>rightVol)
		{
			pan=(LONG)((float)DSBPAN_LEFT*(1.0-rightVol/leftVol));
		}
		else if(rightVol>leftVol)
		{
			pan=(LONG)((float)DSBPAN_RIGHT*(1.0-leftVol/rightVol));
		}

		vol=sqrt(vol);

		float dB=(float)DSBVOLUME_MAX*vol+(float)DSBVOLUME_MIN*(1.0-vol);
		long atten=(long)dB;
		if(DSBVOLUME_MAX<atten)
		{
			atten=DSBVOLUME_MAX;
		}
		if(atten<DSBVOLUME_MIN)
		{
			atten=DSBVOLUME_MIN;
		}
		// printf("%d\n",atten);
		dat.api->dSoundBuf->SetVolume(atten);
		dat.api->dSoundBuf->SetPan(pan);
	}
}

YSRESULT YsSoundPlayer::PlayOneShotAPISpecific(SoundData &dat)
{
	if(nullptr==api->hWndMain && nullptr!=api->hOwnWnd)
	{
		api->RefetchMainWindowHandle();
	}

	if(nullptr!=dat.api->dSoundBuf)
	{
		SetVolumeAPISpecific(dat,dat.playBackVolumeLeft,dat.playBackVolumeRight);
		dat.api->dSoundBuf->SetCurrentPosition(0);
		dat.api->dSoundBuf->Play(0,0xc0000000,0);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::PlayBackgroundAPISpecific(SoundData &dat)
{
	if(nullptr==api->hWndMain && nullptr!=api->hOwnWnd)
	{
		api->RefetchMainWindowHandle();
	}

	if(nullptr!=dat.api->dSoundBuf)
	{
		DWORD sta;
		dat.api->dSoundBuf->GetStatus(&sta);
		if(0==(sta&DSBSTATUS_PLAYING))
		{
			SetVolumeAPISpecific(dat,dat.playBackVolumeLeft,dat.playBackVolumeRight);
			dat.api->dSoundBuf->SetCurrentPosition(0);
			dat.api->dSoundBuf->Play(0,0xc0000000,DSBPLAY_LOOPING);
		}
		return YSOK;
	}
	return YSERR;
}

YSBOOL YsSoundPlayer::IsPlayingAPISpecific(const SoundData &dat) const
{
	if(nullptr!=dat.api->dSoundBuf)
	{
		DWORD sta;
		dat.api->dSoundBuf->GetStatus(&sta);
		if(0!=(sta&DSBSTATUS_PLAYING))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

void YsSoundPlayer::PauseAPISpecific(SoundData &dat)
{
	if(nullptr!=dat.api->dSoundBuf)
	{
		dat.api->dSoundBuf->Stop();
	}
}
void YsSoundPlayer::ResumeAPISpecific(SoundData &dat)
{
	if(nullptr==api->hWndMain && nullptr!=api->hOwnWnd)
	{
		api->RefetchMainWindowHandle();
	}
	if(nullptr!=dat.api->dSoundBuf)
	{
		dat.api->dSoundBuf->Play(0,0xc0000000,0);
	}
}

double YsSoundPlayer::GetCurrentPositionAPISpecific(const SoundData &dat) const
{
	DWORD playCursor,writeCursor;
	if(nullptr!=dat.api->dSoundBuf && DS_OK==dat.api->dSoundBuf->GetCurrentPosition(&playCursor,&writeCursor))
	{
		playCursor/=(dat.BytePerSample()*dat.GetNumChannel());
		return ((double)playCursor)/(double)dat.PlayBackRate();
	}
	return 0.0;
}

void YsSoundPlayer::StopAPISpecific(SoundData &dat)
{
	if(nullptr!=dat.api->dSoundBuf)
	{
		dat.api->dSoundBuf->Stop();
		// Memo: IDirectSoundBuffer::Stop() actually pauses playing.
		//       It does not rewind.  Therefore, to play from the head of the wave,
		//       IDirectSoundBuffer::SetCurrentPosition(0) must be called before playing.
	}
}

void YsSoundPlayer::KeepPlayingAPISpecific(void)
{
}

////////////////////////////////////////////////////////////

YsSoundPlayer::SoundData::APISpecificDataPerSoundData::APISpecificDataPerSoundData()
{
	dSoundBuf=nullptr;
	CleanUp();
}
YsSoundPlayer::SoundData::APISpecificDataPerSoundData::~APISpecificDataPerSoundData()
{
	CleanUp();
}
void YsSoundPlayer::SoundData::APISpecificDataPerSoundData::CleanUp(void)
{
	if(nullptr!=dSoundBuf)
	{
		dSoundBuf->Release();
		dSoundBuf=nullptr;
	}
}

void YsSoundPlayer::SoundData::APISpecificDataPerSoundData::CreateBuffer(LPDIRECTSOUND8 dSound8,SoundData &dat)
{
	CleanUp();

	const int nChannels=dat.GetNumChannel();
	const int nBlockAlign=nChannels*dat.BitPerSample()/8;
	const int nAvgBytesPerSec=dat.PlayBackRate()*nBlockAlign;

	WAVEFORMATEX fmt;
	fmt.cbSize=sizeof(fmt);
	fmt.wFormatTag=WAVE_FORMAT_PCM;

	fmt.nChannels=nChannels;
	fmt.nSamplesPerSec=dat.PlayBackRate();
	fmt.wBitsPerSample=dat.BitPerSample();

	fmt.nBlockAlign=nBlockAlign;
	fmt.nAvgBytesPerSec=nAvgBytesPerSec;


	DSBUFFERDESC desc;
	desc.dwSize=sizeof(desc);
	// Finally!  I found it!
	// https://stackoverflow.com/questions/25829935/play-background-music-with-directsound
	// I can play sound when the window loses focus!
	desc.dwFlags=DSBCAPS_CTRLVOLUME|DSBCAPS_LOCDEFER|DSBCAPS_GLOBALFOCUS;
	desc.dwBufferBytes=dat.SizeInByte();
	if(DSBSIZE_MAX<desc.dwBufferBytes)
	{
		desc.dwBufferBytes=DSBSIZE_MAX;
	}
	desc.dwReserved=0;
	desc.lpwfxFormat=&fmt;
	desc.guid3DAlgorithm=GUID_NULL;
	if(DS_OK==dSound8->CreateSoundBuffer(&desc,&dSoundBuf,NULL))
	{
		auto datPtr=dat.DataPointer();

		DWORD writeBufSize1,writeBufSize2;
		unsigned char *writeBuf1,*writeBuf2;
		if(dSoundBuf->Lock(0,0,(LPVOID *)&writeBuf1,&writeBufSize1,(LPVOID *)&writeBuf2,&writeBufSize2,DSBLOCK_ENTIREBUFFER)==DS_OK &&
		   NULL!=writeBuf1)
		{
			// printf("Buffer Locked\n");

			for(int i=0; i<(int)dat.SizeInByte() && i<(int)writeBufSize1; i++)
			{
				writeBuf1[i]=datPtr[i];
			}

			dSoundBuf->Unlock(writeBuf1,writeBufSize1,writeBuf2,writeBufSize2);
		}
		else
		{
			printf("Failed to Lock Buffer.\n");
			CleanUp();
		}
	}
	
}

////////////////////////////////////////////////////////////

YsSoundPlayer::SoundData::APISpecificDataPerSoundData *YsSoundPlayer::SoundData::CreateAPISpecificData(void)
{
	return new APISpecificDataPerSoundData;
}
void YsSoundPlayer::SoundData::DeleteAPISpecificData(APISpecificDataPerSoundData *ptr)
{
	delete ptr;
}

YSRESULT YsSoundPlayer::SoundData::PreparePlay(YsSoundPlayer &player)
{
	if(nullptr!=api->dSoundBuf)
	{
		return YSOK;
	}
	if(nullptr==player.api->dSound8)
	{
		return YSERR;
	}

	api->CreateBuffer(player.api->dSound8,*this);
	if(nullptr!=api->dSoundBuf)
	{
		return YSOK;
	}
	return YSERR;
}

void YsSoundPlayer::SoundData::CleanUpAPISpecific(void)
{
	if(nullptr!=playerStatePtr && YsSoundPlayer::STATE_ENDED==*playerStatePtr)
	{
		// In this case, DirectSoundBuffer is gone with the player.
		api->dSoundBuf=nullptr;
	}
	api->CleanUp();
}



////////////////////////////////////////////////////////////

void YsSoundPlayer::APISpecificData::OpenDummyWindow(void)
{
	#define WINSTYLE WS_OVERLAPPED|WS_CAPTION|WS_VISIBLE|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN
	#define WINCLASS L"YsSimpleSound_DummyWindowClass"
	HINSTANCE inst=GetModuleHandleA(NULL);

	WNDCLASSW wc;
	wc.style=CS_OWNDC|CS_BYTEALIGNWINDOW;
	wc.lpfnWndProc=(WNDPROC)OwnWindowFunc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=(HINSTANCE)inst;
	wc.hIcon=nullptr;
	wc.hCursor=nullptr;
	wc.hbrBackground=nullptr;
	wc.lpszMenuName=NULL;
	wc.lpszClassName=WINCLASS;

	if(0!=RegisterClassW(&wc))
	{
		RECT rc;
		rc.left  =0;
		rc.top   =0;
		rc.right =127;
		rc.bottom=127;
		AdjustWindowRect(&rc,WINSTYLE,FALSE);
		int wid  =rc.right-rc.left+1;
		int hei  =rc.bottom-rc.top+1;

		const wchar_t *WINTITLE=L"DummyWindowForFailedAPIDesignOfDirectSound";
		hOwnWnd=CreateWindowW(WINCLASS,WINTITLE,WINSTYLE,0,0,wid,hei,NULL,NULL,inst,NULL);

		ShowWindow(hOwnWnd,SW_SHOWNORMAL);
		SetForegroundWindow(hOwnWnd);
	}
}

void YsSoundPlayer::APISpecificData::DestroyDummyWindow(void)
{
	if(nullptr!=hOwnWnd)
	{
		DestroyWindow(hOwnWnd);
	}
	hOwnWnd=nullptr;
}

LRESULT WINAPI YsSoundPlayer::APISpecificData::OwnWindowFunc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp)
{
	return DefWindowProc(hWnd,msg,wp,lp);
}

////////////////////////////////////////////////////////////

enum
{
	RINGBUFFER_MILLI=1000,

	RINGBUFFER_CHANNELS=2,
	RINGBUFFER_SAMPLING_RATE=44100,
	RINGBUFFER_BITS_PER_SAMPLE=16,
};

class YsSoundPlayer::Stream::APISpecificData
{
public:
	LPDIRECTSOUNDBUFFER dSoundBuf=nullptr;

	uint64_t bufferLengthInBytes;
	int64_t seg0Start,seg0End;
	int64_t seg1Start,seg1End;
};

YsSoundPlayer::Stream::APISpecificData *YsSoundPlayer::Stream::CreateAPISpecificData(void)
{
	YsSoundPlayer::Stream::APISpecificData *api=new YsSoundPlayer::Stream::APISpecificData;
	api->dSoundBuf=nullptr;
	return api;
}
void YsSoundPlayer::Stream::DeleteAPISpecificData(APISpecificData *api)
{
	if(nullptr!=api)
	{
		if(nullptr!=api->dSoundBuf)
		{
		}
		delete api;
	}
}

YSRESULT YsSoundPlayer::StartStreamingAPISpecific(Stream &stream,StreamingOption opt)
{
	auto api=stream.api;
	if(nullptr!=api)
	{
		const int nChannels=RINGBUFFER_CHANNELS;
		const int nBlockAlign=nChannels*RINGBUFFER_BITS_PER_SAMPLE/8;
		const int nAvgBytesPerSec=RINGBUFFER_SAMPLING_RATE*nBlockAlign;

		uint64_t sizeInBytes=RINGBUFFER_SAMPLING_RATE;
		sizeInBytes*=RINGBUFFER_BITS_PER_SAMPLE/8;
		sizeInBytes*=RINGBUFFER_CHANNELS;
		sizeInBytes*=opt.ringBufferLengthMillisec;
		sizeInBytes/=RINGBUFFER_MILLI;

		stream.api->bufferLengthInBytes=sizeInBytes;

		WAVEFORMATEX fmt;
		fmt.cbSize=sizeof(fmt);
		fmt.wFormatTag=WAVE_FORMAT_PCM;

		fmt.nChannels=nChannels;
		fmt.nSamplesPerSec=RINGBUFFER_SAMPLING_RATE;
		fmt.wBitsPerSample=RINGBUFFER_BITS_PER_SAMPLE;

		fmt.nBlockAlign=nBlockAlign;
		fmt.nAvgBytesPerSec=nAvgBytesPerSec;


		DSBUFFERDESC desc;
		desc.dwSize=sizeof(desc);
		// Finally!  I found it!
		// https://stackoverflow.com/questions/25829935/play-background-music-with-directsound
		// I can play sound when the window loses focus!
		desc.dwFlags=DSBCAPS_CTRLVOLUME|DSBCAPS_LOCDEFER|DSBCAPS_GLOBALFOCUS|DSBCAPS_GETCURRENTPOSITION2;
		desc.dwBufferBytes=sizeInBytes;
		if(DSBSIZE_MAX<desc.dwBufferBytes)
		{
			desc.dwBufferBytes=DSBSIZE_MAX;
		}
		desc.dwReserved=0;
		desc.lpwfxFormat=&fmt;
		desc.guid3DAlgorithm=GUID_NULL;
		if(DS_OK==this->api->dSound8->CreateSoundBuffer(&desc,&stream.api->dSoundBuf,NULL))
		{
			DWORD writeBufSize1,writeBufSize2;
			unsigned char *writeBuf1,*writeBuf2;
			if(stream.api->dSoundBuf->Lock(0,0,(LPVOID *)&writeBuf1,&writeBufSize1,(LPVOID *)&writeBuf2,&writeBufSize2,DSBLOCK_ENTIREBUFFER)==DS_OK)
			{
				if(NULL!=writeBuf1)
				{
					for(int i=0; i<(int)writeBufSize1; i++)
					{
						writeBuf1[i]=0;
					}
				}
				if(NULL!=writeBuf2)
				{
					for(int i=0; i<(int)writeBufSize2; i++)
					{
						writeBuf2[i]=0;
					}
				}

				stream.api->dSoundBuf->Unlock(writeBuf1,writeBufSize1,writeBuf2,writeBufSize2);

				stream.api->dSoundBuf->SetCurrentPosition(0);
				stream.api->dSoundBuf->Play(0,0xc0000000,DSBPLAY_LOOPING);

				stream.api->seg0Start=0;
				stream.api->seg0End=0;
				stream.api->seg1Start=0;
				stream.api->seg1End=0;
			}
			else
			{
				printf("Failed to Lock Buffer.\n");
				return YSERR;
			}
		}
		return YSOK;
	}
	return YSERR;
}
void YsSoundPlayer::StopStreamingAPISpecific(Stream &stream)
{
	if(nullptr!=stream.api && nullptr!=stream.api->dSoundBuf)
	{
		DWORD writeBufSize1,writeBufSize2;
		unsigned char *writeBuf1,*writeBuf2;

		stream.api->dSoundBuf->Stop();
		if(stream.api->dSoundBuf->Lock(0,0,(LPVOID *)&writeBuf1,&writeBufSize1,(LPVOID *)&writeBuf2,&writeBufSize2,DSBLOCK_ENTIREBUFFER)==DS_OK)
		{
			if(NULL!=writeBuf1)
			{
				for(int i=0; i<(int)writeBufSize1; i++)
				{
					writeBuf1[i]=0;
				}
			}
			if(NULL!=writeBuf2)
			{
				for(int i=0; i<(int)writeBufSize2; i++)
				{
					writeBuf2[i]=0;
				}
			}

			stream.api->dSoundBuf->Unlock(writeBuf1,writeBufSize1,writeBuf2,writeBufSize2);

			stream.api->dSoundBuf->SetCurrentPosition(0);

			stream.api->seg0Start=0;
			stream.api->seg0End=0;
			stream.api->seg1Start=0;
			stream.api->seg1End=0;
		}
	}
}
YSBOOL YsSoundPlayer::StreamPlayerReadyToAcceptNextSegmentAPISpecific(const Stream &stream,const SoundData &dat) const
{
	if(nullptr!=stream.api && nullptr!=stream.api->dSoundBuf)
	{
		DWORD playCursor,writeCursor;
		stream.api->dSoundBuf->GetCurrentPosition(&playCursor,&writeCursor);
		// Two segments: Seg0 -> Seg1.
		// Next data should be written when Seg0 is done.
		if(stream.api->seg0Start<=stream.api->seg0End)
		{
			//   0   seg0Start       seg0End
			//   |     |---------------|        |
			//                SEG0
			if(playCursor<stream.api->seg0Start || stream.api->seg0End<=playCursor)
			{
				return YSTRUE;
			}
		}
		else // means segment 0 wrapped around.
		{
			//   0   seg0End         seg0Start
			//   |----|                |--------|
			//    SEG0                    SEG0
			if(stream.api->seg0End<=playCursor && playCursor<stream.api->seg0Start)
			{
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}
YSRESULT YsSoundPlayer::AddNextStreamingSegmentAPISpecific(Stream &stream,const SoundData &dat)
{
	// How can I tell both segments lapsed?
	// If playing segment1, add it to the end of segment 1, and erase segment 0.
	// If not, lapsed or it is the first segment.  Just write it to writeCursor.
	if(nullptr!=stream.api && nullptr!=stream.api->dSoundBuf)
	{
		int64_t numSamplesIn=dat.GetNumSamplePerChannel();
		int64_t numSamplesOut=numSamplesIn;
		numSamplesOut*=RINGBUFFER_SAMPLING_RATE;
		numSamplesOut/=dat.PlayBackRate();



		DWORD playCursor,writeCursor;
		stream.api->dSoundBuf->GetCurrentPosition(&playCursor,&writeCursor);

		bool isPlayingSegment1=false;
		if(stream.api->seg1Start<=stream.api->seg1End)
		{
			//   0   seg1Start       seg1End
			//   |     |---------------|        |
			//                SEG1
			isPlayingSegment1=(stream.api->seg1Start<=playCursor && playCursor<stream.api->seg1End);
		}
		else // means segment 0 wrapped around.
		{
			//   0   seg1End         seg1Start
			//   |----|                |--------|
			//    SEG1                    SEG1
			isPlayingSegment1=(playCursor<stream.api->seg1End || stream.api->seg1Start<=playCursor);
		}

		DWORD bytesCanWrite=0;
		if(playCursor<writeCursor)
		{
			//   0   playCursor     writeCursor
			//   |    |----------------|        |bufferLength
			bytesCanWrite=stream.api->bufferLengthInBytes-(writeCursor-playCursor);
		}
		else
		{
			//   0   writeCursor     playCursor
			//   |----|                |--------|
			bytesCanWrite=playCursor-writeCursor;
		}

		if(true==isPlayingSegment1)
		{
			writeCursor=(DWORD)stream.api->seg1End;
		}

		DWORD bytesWrite=std::min<DWORD>(numSamplesOut*2*RINGBUFFER_CHANNELS,bytesCanWrite);


		DWORD writeBufSize1,writeBufSize2;
		unsigned char *writeBuf1,*writeBuf2;
		if(stream.api->dSoundBuf->Lock(writeCursor,bytesWrite,(LPVOID *)&writeBuf1,&writeBufSize1,(LPVOID *)&writeBuf2,&writeBufSize2,0)==DS_OK)
		{
			const unsigned char *readPtr=dat.DataPointer();

			int64_t balance=RINGBUFFER_SAMPLING_RATE;
			if(NULL!=writeBuf1)
			{
				unsigned char *writePtr=(unsigned char *)writeBuf1;
				uint64_t sizeLeft=writeBufSize1;
				while(0<sizeLeft)
				{
					if(1==dat.GetNumChannel())
					{
						*writePtr++=readPtr[0];
						*writePtr++=readPtr[1];
						*writePtr++=readPtr[0];
						*writePtr++=readPtr[1];
					}
					else
					{
						*writePtr++=readPtr[0];
						*writePtr++=readPtr[1];
						*writePtr++=readPtr[2];
						*writePtr++=readPtr[3];
					}

					sizeLeft-=4;

					balance-=dat.PlayBackRate();
					while(balance<0)
					{
						balance+=RINGBUFFER_SAMPLING_RATE;
						readPtr+=2*dat.GetNumChannel();
					}
				}
			}

			if(NULL!=writeBuf2)
			{
				unsigned char *writePtr=(unsigned char *)writeBuf2;
				uint64_t sizeLeft=writeBufSize2;
				while(0<sizeLeft)
				{
					if(1==dat.GetNumChannel())
					{
						*writePtr++=readPtr[0];
						*writePtr++=readPtr[1];
						*writePtr++=readPtr[0];
						*writePtr++=readPtr[1];
					}
					else
					{
						*writePtr++=readPtr[0];
						*writePtr++=readPtr[1];
						*writePtr++=readPtr[2];
						*writePtr++=readPtr[3];
					}

					sizeLeft-=4;

					balance-=dat.PlayBackRate();
					while(balance<0)
					{
						balance+=RINGBUFFER_SAMPLING_RATE;
						readPtr+=2*dat.GetNumChannel();
					}
				}
			}
			stream.api->dSoundBuf->Unlock(writeBuf1,writeBufSize1,writeBuf2,writeBufSize2);
		}


		if(true==isPlayingSegment1)
		{
			stream.api->seg0Start=stream.api->seg1Start;
			stream.api->seg0End=stream.api->seg1End;
		}
		else
		{
			// Lapsed: Make entire rest of the buffer as segment 0.
			stream.api->seg0Start=(writeCursor+bytesWrite)%stream.api->bufferLengthInBytes;
			stream.api->seg0End=writeCursor;
		}
		stream.api->seg1Start=writeCursor;
		stream.api->seg1End=(writeCursor+bytesWrite)%stream.api->bufferLengthInBytes;
		return YSOK;
	}
	return YSERR;
}
