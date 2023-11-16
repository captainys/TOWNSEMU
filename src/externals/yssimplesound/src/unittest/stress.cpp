#include <cstdio>
#include <vector>
#include <chrono>
#include <math.h>
#include <thread>
#include <mutex>
#include <algorithm>
#include <string.h>

#include <fssimplewindow.h>
#include <yssimplesound.h>


const int samplePerChannel=44100;


void MakeSilence(std::vector <unsigned char> &wave)
{
	wave.resize(samplePerChannel*4);  // 4-bytes per sample * 44.1KHz = 1 second
	memset((char *)wave.data(),0,wave.size());
}

bool MakeData(YsSoundPlayer::SoundData &data)
{
	std::vector <unsigned char> wave;
	MakeSilence(wave);

	data.CreateFromSigned16bitStereo(44100,wave);

	if(data.GetNumSamplePerChannel()!=samplePerChannel)
	{
		fprintf(stderr,"Sample count error.\n");
		return false;
	}
	return true;
}


bool RecyclePlayer(void)
{
	YsSoundPlayer::SoundData data;
	if(true!=MakeData(data))
	{
		return false;
	}

	YsSoundPlayer sndPlayer;
	for(int i=0; i<5; ++i)
	{
		printf("%d\n",i);
		sndPlayer.Start();
		sndPlayer.PlayOneShot(data);
		while(YSTRUE==sndPlayer.IsPlaying(data))
		{
			FsPollDevice();
			sndPlayer.KeepPlaying();
		}
		sndPlayer.End();
	}
	return true;
}

bool Position(void)
{
	YsSoundPlayer::SoundData data;
	if(true!=MakeData(data))
	{
		return false;
	}

	bool result=true;
	YsSoundPlayer sndPlayer;
	sndPlayer.Start();

	auto t0=std::chrono::high_resolution_clock::now();
	sndPlayer.PlayOneShot(data);
	double nextPrint=0.0;

	std::vector <double> realTime,waveTime;
	while(YSTRUE==sndPlayer.IsPlaying(data))
	{
		auto playPos=sndPlayer.GetCurrentPosition(data);

		FsPollDevice();
		sndPlayer.KeepPlaying();

		auto dt=std::chrono::high_resolution_clock::now()-t0;
		auto millisec=std::chrono::duration_cast<std::chrono::milliseconds>(dt).count();
		double sec=(double)millisec/1000.0;

		// Very likely the data is done after last checking IsPlaying before reaching this point.
		if(YSTRUE==sndPlayer.IsPlaying(data))
		{
			if(nextPrint<sec)
			{
				realTime.push_back(sec);
				waveTime.push_back(playPos);
				printf("Player says %lf  Timer says %lf\n",playPos,sec);
				nextPrint+=0.05;
			}
		}
	}
	sndPlayer.End();


	double a=0,b=0,c=0;
	double d=0,e=0,f=0;
	for(int i=0; i<realTime.size(); ++i)
	{
		double xi=realTime[i];
		double yi=waveTime[i];
		a+=2.0*xi*xi;
		b+=2.0*xi;
		c+=2.0*xi*yi;

		d+=2.0*xi;
		e+=2.0;
		f+=2.0*yi;
	}

	double D=a*e-b*d;
	if(0.0==D)
	{
		// Failed to do least square.
		printf("Failed least square.\n");
		return false;
	}
	std::swap(a,e);
	b=-b;
	d=-d;

	a/=D;
	b/=D;
	d/=D;
	e/=D;

	double A=a*c+b*f;
	double B=d*c+e*f;
	printf("Fitted  y=%lfx+%lf\n",A,B);
	if(0.15<fabs(A-1.0))
	{
		return false;
	}

	return true;
}

std::mutex mtx;
bool done=false;
bool mtResult=true;
void ThreadFunc(void)
{
	YsSoundPlayer::SoundData data;
	if(true!=MakeData(data))
	{
		mtx.lock();
		done=true;
		mtResult=false;
		mtx.unlock();
		return;
	}

	YsSoundPlayer sndPlayer;
	for(int i=0; i<3; ++i)
	{
		printf("%d\n",i);
		sndPlayer.Start();
		sndPlayer.PlayOneShot(data);
		while(YSTRUE==sndPlayer.IsPlaying(data))
		{
			sndPlayer.KeepPlaying();
		}
		sndPlayer.End();
	}

	mtx.lock();
	done=true;
	mtResult=true;
	mtx.unlock();
}

int main(void)
{
	FsOpenWindow(0,0,100,100,0);

	printf("Position vs Timer\n");
	if(true!=Position())
	{
		fprintf(stderr,"Current Position Error (more than 0.1 seconds).\n");
		return 1;
	}

	printf("Multi Threading\n");
	{
		std::thread t(ThreadFunc);
		for(;;)
		{
			FsPollDevice();
			mtx.lock();
			auto doneCopy=done;
			mtx.unlock();

			if(true==doneCopy)
			{
				break;
			}
		}
		t.join();
	}

	printf("Recycling\n");
	if(true!=RecyclePlayer())
	{
		fprintf(stderr,"Player Recycling failed.\n");
		return 1;
	}

	printf("End Test.\n");
	return 0;
}
