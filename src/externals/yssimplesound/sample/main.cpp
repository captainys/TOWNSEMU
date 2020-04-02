#include "fssimplewindow.h"
#include "yssimplesound.h"


//    To compile without CMake, follow the instruction below:
//     (1) Copy FsSimpleWindow framework source files to the same directory as main.cpp
//     (2) Copy following files to the same directory as main.cpp
//      For MacOSX (Require Cocoa framework)
//        yssimplesound.cpp
//        yssimplesound.h
//        macosx/yssimplesound_macosx_cpp.cpp
//        macosx/yssimplesound_macosx_objc.m      
//
//      For Linux (Require ALSA library, libasound.a)
//        yssimplesound.cpp
//        yssimplesound.h
//        linux_alsa/yssimplesound_linux_alsa.cpp
//
//      For Windows (Direct Sound):
//        yssimplesound.cpp
//        yssimplesound.h
//        windows_dsound/yssimplesound_dsound.cpp
//
//      For No Window (No sound, linking only)
//        yssimplesound.cpp
//        yssimplesound.h
//        nownd/yssimplesound_nownd.cpp
//     (3) Make a project and add all source files in the project.
//     (4) Build.
//
//  To run the program, .WAV data files must be copied to the working directory.
//  See instructions below labeled as [Copying Data Files].
int main(int ac,char *av[])
{
	if(ac<2)
	{
		printf("Usage: sample filename.wav\n");
		return 1;
	}

	// You need to have an instance of YsSoundPlayer.
	YsSoundPlayer player;

	// The sound player object must be made current and started before playing a sound data.
	// Make sure to do this after opening a Window.
	// YsSoundPlayer implementation for Windows uses DirectSound API, which is a design failure.
	// DirectSound API requires a Window to play a sound data.
	// When you design an API, it is one of the most important things to keep modules independent.
	// Unnecessary dependencies are the worst enemy.
	// Anyway, whoever designed DirectSound API made such a basic mistake, and therefore the
	// sound player needs to be started after opening a window.
	player.MakeCurrent();
	player.Start();


	YsSoundPlayer::SoundData wav;
	if(YSOK!=wav.LoadWav(av[1]))
	{
		printf("Error!  Cannot load %s!\n",av[1]);
		return 1;
	}

	player.PlayOneShot(wav);
	while(YSTRUE==player.IsPlaying(wav))
	{
		player.KeepPlaying();
	}

	// It is a good practice to End the player.  If you don't it's automatic in the destructor.
	player.End();

	return 0;
}



