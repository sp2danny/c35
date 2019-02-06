
#include "stdafx.h"

// --------------------------------------------

using namespace std;

#include "SDL.h"

#include "Anim.h"

typedef unsigned char UC;
typedef unsigned short US;

struct pcxHeader
{
	UC manuf,verinf,encoding,bpp;
	US left_marg,upper_marg,right_marg,lower_marg;
	US hor_dpi,ver_dpi;
	UC pal16[48];
	UC reserved,NCP,NBS;
	US palinfo,hor_scr_sz,ver_scr_sz;
	UC reserver2[54];
};

typedef UC* UCP;

bool pal_is_pl(UC p)
{
	int row = p / 16;
	if(row==0) return true;
	if(row>3) return false;
	return (p%2)==0;
}

bool pal_is_dith(UC p)
{
	return p >= 224;
}

void DBG_LOG(char*)
{
}

struct TRIP { UC r,g,b; };
TRIP palette[256];

extern C35::RGBA GetPixel( SDL_Surface* surf, int x, int y);

void MakePal()
{
	SDL_Surface* pal = SDL_LoadBMP("gfx/conv/pal3.bmp");
	int i=0;
	int dx = pal->w / 16;
	int dy = pal->h / 16;
	SDL_LockSurface(pal);
	for( int x=0; x<16; ++x )
		for( int y=0; y<16; ++y )
		{
			C35::RGBA pix = GetPixel( pal, x*dx + 2, y*dy + 2 );
			palette[i].r = pix.r;
			palette[i].g = pix.g;
			palette[i].b = pix.b;
			++i;
		}
	SDL_UnlockSurface(pal);
}

void C35::CIS::LoadPCX(const char* fn)
{
	FILE* fp = fopen(fn,"rb");

	if(!fp) {
		pixels.clear(); pixeltypes.clear();
		w=h=0;
		return;
	}

	pcxHeader hdr;
	fread(&hdr,128,1,fp);

	int height = 1 + hdr.lower_marg - hdr.upper_marg;
	int width  = 1 + hdr.right_marg - hdr.left_marg;
	//if(!hdr.left_marg) --width;
	if(width==469) width=470;
	using std::vector;
	vector<vector<UC>> scanlines;
	scanlines.reserve(height);
	int x,y;
	int nbs = width * (hdr.bpp/8);
	if(nbs%2) ++nbs;

	if(hdr.NBS!=nbs)
		 DBG_LOG("error in header");

	for(y=0;y<height;++y)
	{
		scanlines.emplace_back();
		scanlines.back().reserve(nbs+1);
		x=0;
		while(x<nbs)
		{
			UC c1 = getc(fp);

			if( (c1 & 0xC0) == 0xC0 )
			{
				UC c2 = getc(fp);
				c1 &= 0x3F;
				while(c1)
				{
					if(x++>=nbs) DBG_LOG("error in image");
					scanlines[y].push_back(c2);
					--c1;
				}
			} else {
				if(x++>=nbs) DBG_LOG("error in image");
				scanlines[y].push_back(c1);
			}

			if(feof(fp)) { 
				DBG_LOG("unexpected eof"); break;
			}
		}
	}

	if(hdr.verinf>=5)
	{
		int c = getc(fp);
		//if(c==0x0c)
		{
			for(int i=0;i<256;++i)
			{
				palette[i].r = getc(fp);
				palette[i].g = getc(fp);
				palette[i].b = getc(fp);
				assert(!feof(fp));
				//int r = fread(palette+i,1,3,fp);
				//assert(r==3);
			}
		}
		/*else if(c==0)
		{
			MakePal();
		}*/
	}

	fclose(fp);

	//UC SHADOW[] = { 191, 195, 200, 204, 208, 212, 217, 221, 225, 229, 234, 238, 242, 246, 251, 255 };
	//UC SMOKE[]  = {   0,  13,  26,  38,  51,  64,  77,  90, 102, 115, 128, 141, 154, 116, 179, 192 };

	// UC SHADOW[] = { 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 21 };
	// UC SMOKE[]  = {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 };

	UC SMOKE[]  = { 191, 195, 200, 204, 208, 212, 217, 221, 225, 229, 234, 238, 242, 246, 251, 255 };
	UC SHADOW[]	= {   0,  13,  26,  38,  51,  64,  77,  90, 102, 115, 128, 141, 154, 166, 179, 192 };

	// the latest part of palette is altered
	/*for(int i=0;i<16;++i)
	{
		palette[i+224].r = SMOKE[i];
		palette[i+224].g = SMOKE[i];
		palette[i+224].b = SMOKE[i];
		palette[i+240].r = SHADOW[i];
		palette[i+240].g = SHADOW[i];
		palette[i+240].b = SHADOW[i];
	}*/


	C35::CIS& cis = (*this);

	cis.w = 1 + hdr.right_marg - hdr.left_marg;
	cis.h = height;
	cis.hx = hdr.left_marg;
	cis.hy = hdr.upper_marg;

	int sz = cis.w * cis.h ;
	cis.pixels.clear();
	cis.pixels.reserve(sz);
	cis.pixeltypes.clear();
	cis.pixeltypes.reserve(sz);

	C35::RGBA tr;
	C35::RGBA px;

	cis.has_dither = false;
	cis.has_colimp = false;
	cis.has_trans = false;

	for( y=0; y<cis.h; ++y )
	{
		for( x=0; x<cis.w; ++x )
		{
			UC pix = scanlines[y][x];
			if(pix==255)
			{
				cis.pixeltypes.push_back(C35::CIS::trans);
				cis.pixels.push_back(RGBA_2_HSVA(tr));
				cis.has_trans = true;
			}
			else
			{
				px.r = palette[pix].r;
				px.g = palette[pix].g;
				px.b = palette[pix].b;
				/*if(pal_is_pl(pix))
				{
					cis.pixeltypes.push_back(C35::CIS::colimp);
					cis.has_colimp = true;
				} else if(pal_is_dith(pix))
				{
					cis.pixeltypes.push_back(C35::CIS::alpha);
					cis.has_dither = true;
				} else*/
					cis.pixeltypes.push_back(C35::CIS::normal);

				cis.pixels.push_back(RGBA_2_HSVA(px));
			}
		}

	}

	return;// cis;
}

C35::BasicAnim LoadSequence( const char* fn )
{
	char buff[256];
	int i = 0;
	C35::BasicAnim ba;
	ba.delay = 87;
	ba.repeating = true;
	ba.anim.clear();
	while(true)
	{
		sprintf(buff,"%s_%03d.pcx",fn,i++);
		C35::CIS cis; cis.LoadPCX(buff);
		if(!cis.Width()) break;
		ba.anim.push_back(cis);
	}
	return ba;
}