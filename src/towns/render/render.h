/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef RENDER_IS_INCLUDED
#define RENDER_IS_INCLUDED
/* { */

#include <vector>

#include "physmem.h"
#include "crtc.h"

/*! Make up an RGBA image for showing.
*/
class TownsRender
{
private:
	unsigned int wid,hei;
	std::vector <unsigned char> rgba;


	bool highResCRTC;
	bool crtcIsSinglePageMode;
	bool crtcShowPage[2];
	TownsCRTC::Layer crtcLayer[2];
	TownsCRTC::HardwareMouseCursor hardwareMouse;
	int crtcPriorityPage;
	Vec2i crtcRenderSize;
	int scanLineCounter=0;
	int frequency=0;

public:
	bool damperWireLine=false;
	bool scanLineEffectIn15KHz=false;

	class Image
	{
	public:
		unsigned int wid=0,hei=0;
		const unsigned char *rgba;
	};
	class ImageCopy
	{
	public:
		unsigned int wid=0,hei=0;
		std::vector <unsigned char> rgba;
	};

	TownsRender();

	/*! Create a bitmap image.
	*/
	void Create(int wid,int hei);

	/*!
	*/
	void Crop(unsigned int x0,unsigned int y0,unsigned int wid,unsigned int hei);

	/*! 
	*/
	void Prepare(const TownsCRTC &crtc);


	/*!
	*/
	void PrepareEntireVRAMLayer(const TownsCRTC &crtc,int layer);


	/*!
	*/
	void OerrideShowPage(bool layer0,bool layer1);

	/*! 
	*/
	void BuildImage(const unsigned char VRAM[],const TownsCRTC::AnalogPalette &palette,const TownsCRTC::ChaseHQPalette &chaseHQ);

	/*!
	*/
	void FlipUpsideDown(void);
private:
	void SetResolution(int wid,int hei);

public:
	/*!
	*/
	void MakeOpaque(void);

	/*! Returns an image.
	    This image is a light-copy of the image, therefore it does not own the bitmap.
	    This image only retains a link to the bitmap of TownsRender class.
	*/
	Image GetImage(void) const;

	/*! Returns an image.
	    This image takes ownership of the bitmap from TownsRender.
	    After this function, this TownsRender loses the bitmap.
	*/
	ImageCopy MoveImage(void);

	class VRAM0Trans // 80000000H to 80080000H
	{
	public:
		inline static void Trans(unsigned int &)
		{
		}
	};
	class VRAM1Trans // 80100000H to 80180000H
	{
	public:
		inline static void Trans(unsigned int &offset)
		{
			offset=((offset&4)<<16)|((offset&0x7fff8)>>1)|(offset&3);
		};
	};

	template <class OFFSETTRANS>
	void Render(unsigned int page,const TownsCRTC::Layer &layer,const TownsCRTC::AnalogPalette &palette,const TownsCRTC::ChaseHQPalette &chaseHQ,const unsigned char VRAM[],bool transparent);
	template <class OFFSETTRANS>
	void Render4Bit(const TownsCRTC::Layer &layer,const Vec3ub palette[16],const TownsCRTC::ChaseHQPalette &chaseHQPalette,const unsigned char VRAM[],bool transparent);
	template <class OFFSETTRANS>
	void Render8Bit(const TownsCRTC::Layer &layer,const Vec3ub palette[256],const unsigned char VRAM[],bool transparent);
	template <class OFFSETTRANS>
	void Render16Bit(const TownsCRTC::Layer &layer,const unsigned char VRAM[],bool transparent);
	template <class OFFSETTRANS>
	void Render24Bit(const TownsCRTC::Layer &layer,const unsigned char VRAM[],bool transparent);
};


/* } */
#endif
