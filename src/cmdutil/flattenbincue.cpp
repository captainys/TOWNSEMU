#include <stdio.h>
#include <string>
#include <vector>
#include "iostream"
#include "fstream"
#include "discimg.h"
#include "cpputil.h"



int main(int ac,char *av[])
{
	if(3!=ac)
	{
		fprintf(stderr,"FLATTENCUE by CaptainYS\n");
		fprintf(stderr,"Make data track 2048 bytes/sector\n");
		fprintf(stderr,"Usage:\n");
		fprintf(stderr,"  flattenbincue input.CUE output.CUE\n");
		return 1;
	}

	DiscImage input;
	if(DiscImage::ERROR_NOERROR!=input.OpenCUE(av[1]))
	{
		std::cout << "Cannot open .CUE file." << std::endl;
		return 1;
	}
	if(1!=input.binaries.size())
	{
		std::cout << "Not supporting a multi-bin CUE file." << std::endl;
		return 1;
	}

	bool first=true;
	for(auto &trk : input.GetTracks())
	{
		if(true==first && DiscImage::TRACK_MODE1_DATA!=trk.trackType)
		{
			std::cout << "First track is not a MODE1 data track." << std::endl;
			return 1;
		}
		else if(true!=first && DiscImage::TRACK_AUDIO!=trk.trackType)
		{
			std::cout << "Non-Audio track in 2nd or later track." << std::endl;
			return 1;
		}
		first=false;
	}

	if(true==first)
	{
		std::cout << "No track." << std::endl;
		return 1;
	}

	std::cout << "Input CUE: " << input.fName << std::endl;
	std::cout << "Input BIN: " << input.binaries[0].fName << std::endl;

	std::string outFNameBase=cpputil::RemoveExtension(av[2]);
	std::string outCUEFName=outFNameBase+".CUE";
	std::string outBINFName=outFNameBase+".BIN";

	std::cout << "Output CUE: " << outCUEFName << std::endl;
	std::cout << "Output BIN: " << outBINFName << std::endl;



	std::ifstream inputCUE,inputBIN;
	inputCUE.open(input.fName);
	inputBIN.open(input.binaries[0].fName,std::ios::binary);

	std::ofstream outCUE,outBIN;
	outCUE.open(outCUEFName);
	outBIN.open(outBINFName,std::ios::binary);

	inputCUE.close();
	inputBIN.close();



	if(true!=outCUE.is_open() || true!=outBIN.is_open())
	{
		std::cout << "Cannot open output file." << std::endl;
		return 1;
	}



	// first flag has been checked above.  Safe to access GetTracks()[0].
	auto dataTrack=input.GetTracks()[0];
	auto LBA0=dataTrack.start.ToHSG();
	auto LBA1=dataTrack.end.ToHSG();

	auto sectorData=input.ReadSectorMODE1(LBA0,(LBA1-LBA0)+1);
	std::cout << "Track 1 From " << LBA0 << " to " << LBA1 << std::endl;
	std::cout << "Track 1 Size: " << sectorData.size() << std::endl;
	outBIN.write((char *)sectorData.data(),sectorData.size());

	if(2<=input.GetTracks().size())
	{
		auto preGapSectors=input.GetTracks()[1].preGap.ToHSG();
		auto preGapVerify=input.GetTracks()[1].start.ToHSG()-(LBA1+1);
		std::cout << "PreGap " << preGapSectors << " sectors." << std::endl;
		if(preGapSectors!=preGapVerify)
		{
			std::cout << "Something is wrong." << std::endl;
		}

		std::vector <char> preGap;
		preGap.resize(preGapSectors*2048);
		for(auto &b : preGap)
		{
			b=0;
		}
		outBIN.write(preGap.data(),preGap.size());

		inputBIN.open(input.binaries[0].fName,std::ios::binary);
		// The twist of the PREGAP in BIN/CUE:
		// PREGAP of the track belongs to the track.  Not to the previous track.
		// However, the sector length of the PREGAP seems to be of the previous track.
		// What's the ****?
		// Since locationInFile points to the beginning of the PREGAP, 
		// number of bytes of the PREGAP must be added to skip it.
		inputBIN.seekg(input.GetTracks()[1].locationInFile+preGapSectors*input.GetTracks()[0].sectorLength,inputBIN.beg);

		std::vector <char> dataBuf;
		dataBuf.resize(64*1024*1024);
		unsigned int progress=0;
		while(true!=inputBIN.eof())
		{
			inputBIN.read(dataBuf.data(),dataBuf.size());
			auto readSize=inputBIN.gcount();
			outBIN.write(dataBuf.data(),readSize);
			progress+=readSize;
			std::cout << progress/(1024*1024) << "MB" << std::endl;
		}

		std::cout << "Audio Track Size: " << progress << std::endl;

		inputBIN.close();
	}
	outBIN.close();


	std::string outBINPath,outBINFile;
	cpputil::SeparatePathFile(outBINPath,outBINFile,outBINFName);

	outCUE << "FILE \"" << outBINFile << "\" BINARY" << std::endl;
	int trackNum=1;
	for(auto trk : input.GetTracks())
	{
		outCUE << "  TRACK ";
		outCUE << cpputil::UitoaZeroPad(trackNum,2) << " ";
		if(DiscImage::TRACK_MODE1_DATA==trk.trackType)
		{
			outCUE << "MODE1/2048" << std::endl;
		}
		else
		{
			outCUE << "AUDIO" << std::endl;
		}

		if(DiscImage::MinSecFrm::Zero()!=trk.preGap)
		{
			outCUE << "    PREGAP ";
			outCUE << cpputil::UitoaZeroPad(trk.preGap.min,2) << ":";
			outCUE << cpputil::UitoaZeroPad(trk.preGap.sec,2) << ":";
			outCUE << cpputil::UitoaZeroPad(trk.preGap.frm,2) << std::endl;
		}
		if(DiscImage::MinSecFrm::Zero()!=trk.index00)
		{
			outCUE << "    INDEX 00 ";
			outCUE << cpputil::UitoaZeroPad(trk.index00.min,2) << ":";
			outCUE << cpputil::UitoaZeroPad(trk.index00.sec,2) << ":";
			outCUE << cpputil::UitoaZeroPad(trk.index00.frm,2) << std::endl;
		}
		outCUE << "    INDEX 01 ";
		outCUE << cpputil::UitoaZeroPad(trk.start.min,2) << ":";
		outCUE << cpputil::UitoaZeroPad(trk.start.sec,2) << ":";
		outCUE << cpputil::UitoaZeroPad(trk.start.frm,2) << std::endl;
		if(DiscImage::MinSecFrm::Zero()!=trk.postGap)
		{
			outCUE << "    INDEX 00 ";
			outCUE << cpputil::UitoaZeroPad(trk.postGap.min,2) << ":";
			outCUE << cpputil::UitoaZeroPad(trk.postGap.sec,2) << ":";
			outCUE << cpputil::UitoaZeroPad(trk.postGap.frm,2) << std::endl;
		}

		trackNum++;
	}
	return 0;
}
