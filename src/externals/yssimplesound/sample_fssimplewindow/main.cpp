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
int main(void)
{
	printf("This program opens a window mainly because DirectSound API requires a window\n");
	printf("to play sound.  The window will be blank, and it is normal.\n");
	printf("\n");

	FsOpenWindow(0,0,800,600,1);

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


	// [Copying Data Files]
	// You need to load a .WAV format sound data to an instance of YsSoundPlayer::SoundData.
	// By calling FsChangeToProgramDir(), the current working directory will be set to:
	//   In Windows and Linux:  The same directory as the executable file.
	//   In MacOSX:             Contents/Resources sub-directory of the application bundle.
	// Make sure to place .WAV files in the current-working directory.
	//
	// In Visual Studio, you can do it by defining a post-build command.  See:
	//   https://msdn.microsoft.com/en-us/library/42x5kfw4(v=vs.140).aspx
	//   https://msdn.microsoft.com/en-us/library/c02as0cs.aspx
	// for more information.
	// If I assume you create a solution in the soundlib directory, the post-build command should look like:
	//   copy $(SolutionDir)..\sample_fssimplewindow\datafiles $(OutDir)
	// 
	// In XCode 7, you can select the project in the "Project Navigator", and then click on the "Build Phase".
	// Then click on the "+" sign, which is probably shown on the right of the project name.
	// Select "New Copy Files Phase".  Then, select files in the "Copy Files" section.
	//
	// If you want to copy a whole folder, see:
	//   http://stackoverflow.com/questions/1401338/include-a-resource-directory-hierarchy-into-app-bundle
	// The point is you first need to add a folder reference, not a group, to your project before adding to
	// the copy files phase.
	FsChangeToProgramDir();

	YsSoundPlayer::SoundData propeller,notice;
	// Use LoadWav function for loading a .WAV file to the variable.
	// Just in case you ended up copying datafiles directory instead of individual files,
	// I try "prop5.wav" and "datafiles/prop5.wav", "notice.wav" and "datafiles/notice.wav".
	if(YSOK!=propeller.LoadWav("prop5.wav") &&
	   YSOK!=propeller.LoadWav("datafiles/prop5.wav"))
	{
		printf("Error!  Cannot load prop5.wav!\n");
	}
	if(YSOK!=notice.LoadWav("notice.wav") &&
	   YSOK!=notice.LoadWav("datafiles/notice.wav"))
	{
		printf("Error!  Cannot load notice.wav!\n");
	}


	printf("Keys:\n");
	printf("  SPACE..........Play a tone (notice)\n");
	printf("  B..............Begin propeller engine noise.\n");
	printf("  E..............End propeller engine noise.\n");
	printf("  1-9............Change Volume.\n");


	for(;;)
	{
		FsPollDevice();

		auto key=FsInkey();
		if(FSKEY_ESC==key)
		{
			break;
		}

		// The player can play two types of sounds effects (1) background (or environmental) or (2) one-shot.
		if(FSKEY_SPACE==key)
		{
			// To play a one-shot sound, use Stop and PlayOneShot function.
			// If you do not call Stop, PlayOneShot does not play what is being played.
			// It does not wait for the sound to finish.  It just ignores the PlayOneShot call.
			player.Stop(notice);
			player.PlayOneShot(notice);
		}
		if(FSKEY_B==key)
		{
			// To play a background sound, use PlayBackground function.
			// MacOSX player has an issue that there may be a gap between loop, but I can do nothing about it.
			player.PlayBackground(propeller);
		}
		if(FSKEY_E==key)
		{
			// To stop a background sound, call Stop function.
			player.Stop(propeller);
		}

		if(FSKEY_0<=key && key<=FSKEY_9)
		{
			float vol=float(key-FSKEY_0)/9.0f;
			player.SetVolume(notice,vol);
			player.SetVolume(propeller,vol);
		}


		// Linux's ALSA API is more like a polling-based sound API.
		// To support ALSA, you need to constantly call KeepPlaying member function.
		// That means, if for some reason, one iteration takes too long, the sound
		// might be interrupted, but again, I can do nothing about it.
		// There is a better API called Pulse Audio, but apparently it is not very popular.
		player.KeepPlaying();


		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		FsSwapBuffers();

		FsSleep(10);
	}

	// It is a good practice to End the player.  If you don't it's automatic in the destructor.
	player.End();

	return 0;
}



