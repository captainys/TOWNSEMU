#include <stdio.h>
#include <string>
#include <vector>
#include <cstdint>
#include "iostream"
#include "fstream"
#include "discimg.h"
#include "cpputil.h"



int main(int ac,char *av[])
{
	if(3!=ac)
	{
		fprintf(stderr,"CHOPBINCUE by CaptainYS\n");
		fprintf(stderr,"Convert BIN/CUE file from single-BIN to multi-BIN.\n");
		fprintf(stderr,"Usage:\n");
		fprintf(stderr,"  chopbincue input.CUE output.CUE\n");
		return 1;
	}

	std::cout << "CAUTION: TSUGARU does not support multi-BIN disc image at this time." << std::endl;
	std::cout << "         This program is for write-back purpose only." << std::endl;

	DiscImage input;
	if(DiscImage::ERROR_NOERROR!=input.OpenCUE(av[1]))
	{
		std::cout << "Cannot open .CUE file." << std::endl;
		return 1;
	}
	if(1!=input.binaries.size())
	{
		std::cout << "Already a multi-bin CUE file." << std::endl;
		return 1;
	}

	std::cout << "Input CUE: " << input.fName << std::endl;
	std::cout << "Input BIN: " << input.binaries[0].fName << std::endl;

	std::string outFNameBase=cpputil::RemoveExtension(av[2]);
	std::string outCUEFName=outFNameBase+".CUE";

	std::cout << "Output CUE: " << outCUEFName << std::endl;



	std::ifstream inputCUE,inputBIN;
	inputCUE.open(input.fName);
	inputBIN.open(input.binaries[0].fName,std::ios::binary);

	std::ofstream outCUE;
	outCUE.open(outCUEFName);

	inputCUE.close();
	inputBIN.close();



	{
		std::ifstream ifp;
		ifp.open(input.binaries[0].fName,std::ios::binary);

		if(true==ifp.is_open())
		{
			int track=1;
			for(auto &t : input.GetTracks())
			{
				std::string outBINFName=outFNameBase+cpputil::UitoaZeroPad(track,3)+".BIN";
				std::cout << "Output BIN: " << outBINFName << std::endl;
				std::cout << "Location in Input Binary: " << t.locationInFile << std::endl;

				uint64_t length;
				if(track<input.GetTracks().size())
				{
					length=input.GetTracks()[track].locationInFile-input.GetTracks()[track-1].locationInFile;
				}
				else
				{
					length=input.totalBinLength-input.GetTracks()[track-1].locationInFile;
				}
				std::cout << "Length in Input Binary:   " << length << std::endl;

				std::vector <char> databuf;
				databuf.resize(length);

				ifp.read(databuf.data(),length);
				auto actuallyRead=ifp.gcount();
				if(actuallyRead<length)
				{
					std::cout << "Error while reading input binary." << std::endl;
					return 1;
				}

				std::ofstream ofp;
				ofp.open(outBINFName,std::ios::binary);
				ofp.write(databuf.data(),length);
				ofp.close();

				++track;
			}
		}
		else
		{
			std::cout << "Failed to open input binary file." << std::endl;
			return 1;
		}
	}



	std::string outBINPath,outBINFileBase;
	cpputil::SeparatePathFile(outBINPath,outBINFileBase,outFNameBase);

	int trackNum=1;
	for(auto trk : input.GetTracks())
	{
		std::string outBINFile=outBINFileBase+cpputil::UitoaZeroPad(trackNum,3)+".BIN";
		outCUE << "FILE \"" << outBINFile << "\" BINARY" << std::endl;


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
