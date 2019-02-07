
#include "stdafx.h"

// --------------------------------------------

#include "Font.h"

using namespace std;

// ************
// *** Font ***
// ************

Font::Font()
{
	font.clear();
}

Font::Font(char* folder)
{
	Load(folder);
}

void Font::Clear()
{
	for(auto p : font)
	{
		SDL_FreeSurface(p);
	}
	font.clear();
}

namespace
{
	SDL_Surface* load(char* name)
	{
		SDL_Surface* srf  = SDL_LoadBMP(name);
		if(!srf) return 0;
		Uint32 key = SDL_MapRGB(srf->format,255,0,255);
		SDL_SetColorKey(srf,SDL_SRCCOLORKEY,key);
		return srf;
	}
}

static char* MakeHex(unsigned char i)
{
	static char h[] = "0123456789abcdef" ;
	static char res[3] = {0,0,0};
	res[0]=h[i/16];
	res[1]=h[i%16];
	return res;
}

void Font::Load(char* folder)
{
	Clear();
	font.reserve(256);
	for(int i=0;i<255;++i)
	{
		/*if(!isprint(i))
		{
			font.push_back(0);
			continue;
		}*/
		char buff[256];
		sprintf(buff,"gfx/%s/%s.bmp",folder,MakeHex(i));
		font.push_back(load(buff));
	}
}

Font::~Font()
{
	Clear();
}

int Font::Height()
{
	for(int i=0;i<255;++i)
		if(font[i])
			return
				font[i]->h;
	return 0;
}

int Font::Width(const char* text)
{
	char c;
	const char* p;
	int len = 0;
	for(p=text;(c=*p);++p)
	{
		unsigned char uc = static_cast<unsigned char>(c);
		SDL_Surface* s = font[uc];
		if(!s) continue;
		len += s->w;
	}
	return len;
}

void Font::Print( SDL_Surface* dest, const char* text, int x,int y, bool center )
{
	if(center)
		x -= Width(text)/2;

	char c;
	const char* p;
	for(p=text;(c=*p);++p)
	{
		unsigned char uc = static_cast<unsigned char>(c);
		SDL_Surface* s = font[uc];
		if(!s) continue;
		SDL_Rect r = { (short)(x),(short)(y),(unsigned short)(s->w),(unsigned short)(s->h) };
		SDL_BlitSurface(s,0,dest,&r);
		x+=s->w;
	}
}

void Font::Lines( SDL_Surface* , const char*, int,int, int )
{
}

// ***************
// *** RawFont ***
// ***************

/*
	RGBA HSVA_2_RGBA( const HSVA& );
	HSVA RGBA_2_HSVA( const RGBA& );
*/

using namespace C35;

void RawFont::Char::MakeCIS( C35::CIS& cis, bool shadow)
{
	cis.w = width + (shadow?2:0);
	cis.h = height + (shadow?2:0);

	cis.pixels.clear();
	cis.pixeltypes.clear();
	C35::HSVA black = RGBA_2_HSVA( RGBA(   0,   0,   0 ) );
	C35::HSVA red   = RGBA_2_HSVA( RGBA( 255,   0,   0 ) );
	cis.pixels.resize( cis.w * cis.h , black );
	cis.pixeltypes.resize( cis.w * cis.h , C35::CIS::trans );
	cis.has_trans=true;
	cis.has_colimp=true;
	cis.has_dither=shadow;
	if(bits.empty()) FromBytes();
	if(shadow)
	{
		int idx=0;
		for(int y=0;y<height;++y) for(int x=0;x<width;++x)
		{
			if(bits[idx])
			{
				cis.pixeltypes[ (x+1) + (y+2)*cis.w ] = C35::CIS::alpha;
				cis.pixeltypes[ (x+2) + (y+2)*cis.w ] = C35::CIS::alpha;
			}
			++idx;
		}
	}

	int idx=0;
	for(int y=0;y<height;++y) for(int x=0;x<width;++x)
	{
		if(bits[idx])
		{
			cis.pixeltypes[ x + y*cis.w ] = C35::CIS::colimp;
			cis.pixels[ x + y*cis.w ] = red;
		}
		++idx;
	}

}


void RawFont::MakeAD(C35::AnimDir& ad,bool shadow)
{
	ad.bad.clear();
	int cc;
	for(cc=0; cc<256; ++cc)
	{
		Char& c = chars[cc];
		if( (!c.width) || (!c.height) || (c.bits.empty()&&c.bytes.empty()) ) continue;
		ad.bad.emplace_back();
		AD::BAD& bd = ad.bad.back();
		bd.dir = cc;
		bd.mirror = false;
		bd.anim.clear();
		bd.delay=0;
		bd.jbf=0;
		bd.repeating=false;
		bd.anim.emplace_back();
		c.MakeCIS( bd.anim.back(), shadow );
	}
}


RawFont::RawFont()
{
}

void RawFont::FromFolder(const char* fn)
{
	height=0;

	for(int i=0;i<256;++i)
	{
		chars[i].bits.clear();
		chars[i].bytes.clear();
		char hex[] = "0123456789abcdef";
		char buff[256];
		sprintf( buff, "%s\\%c%c.bmp", fn, hex[i/16], hex[i%16] );
		SDL_Surface* surf = SDL_LoadBMP(buff);
		if(!surf)
		{
			chars[i].offset = 0;
			chars[i].length = 0;
			chars[i].width = 0;
		} else {
			SDL_LockSurface(surf);
			int bpp = surf->format->BytesPerPixel;
			int w = chars[i].width = surf->w;
			int h = chars[i].height = surf->h;
			if(!height) height=h;
			assert(height==h);
			int stride = w * bpp;
			while( stride % 4 ) ++stride;
			for(int y=0;y<h;++y) for(int x=0;x<w;++x)
			{
				int offs = stride * y + x*bpp;
				unsigned long int pix = 0;
				unsigned char* p = (unsigned char*) surf->pixels;
				p += offs;
				for(int j=0;j<bpp;++j)
				{
					unsigned int ii = (*p);
					pix |= ( ii << (j*8) );
					++p;
				}
				chars[i].bits.push_back( pix==0 );

			}
			SDL_UnlockSurface(surf);
		}
	}
}

static int len_from( int w, int h )
{
	int l=w*h;
	return (l/8) + ((l%8)?1:0);
}

void RawFont::SaveFile(const char* fn)
{
	FILE* fp = fopen(fn,"wb");

	fwrite( & height, 1, 1, fp );

	for(int i=0;i<256;++i)
	{
		chars[i].FromBits();
		// chars[i].length = chars[i].bytes.size();
		chars[i].length = len_from(chars[i].width,chars[i].height);
		// assert( chars[i].length == len_from(chars[i].width,chars[i].height) );
		assert( chars[i].bytes.size() >= chars[i].length );
		assert( chars[i].bytes.size() == chars[i].length );
	}

	for(int i=0;i<256;++i)
	{
		fwrite( & chars[i].width,  1, 1, fp );
	}

	for(int i=0;i<256;++i)
	{
		int j,n=chars[i].length;
		for(j=0;j<n;++j)
		{
			unsigned char c = chars[i].bytes[j];
			fwrite( &c, 1,1, fp );
		}
	}
	fclose(fp);
}


void RawFont::Char::FromBits()
{
	bytes.clear();
	int i,n = bits.size();
	int j=0;
	unsigned char c = 0;
	for(i=0;i<n;++i)
	{
		c |= bits[i] << j;
		if (++j >= 8 )
		{
			bytes.push_back(c);
			c=0;
			j=0;
		}
	}
	if( j!=0 )
	{
		bytes.push_back(c);
	}
}

void RawFont::Char::FromBytes()
{
	bits.clear();
	int i,n=bytes.size();
	for(i=0;i<n;++i)
	{
		unsigned char c = bytes[i];
		for(int j=0;j<8;++j)
		{
			bits.push_back( c & 1 );
			c = c >> 1;
		}
	}
}

void RawFont::LoadFile(const char* fn)
{
	FILE* fp = fopen(fn,"rb");

	fread( & height, 1, 1, fp );

	for(int i=0;i<256;++i)
		chars[i].height = height;

	for(int i=0;i<256;++i)
	{
		fread( & chars[i].width,  1, 1, fp );
	}

	chars[0].offset = 0;
	chars[0].length = len_from( chars[0].width, height );

	for(int i=1;i<256;++i)
	{
		chars[i].length = len_from( chars[i].width, height );
		chars[i].offset = chars[i-1].offset + chars[i-1].length;
	}

	for(int i=0;i<256;++i)
	{
		chars[i].bytes.clear();
		for(int j=0;j<chars[i].length;++j)
		{
			unsigned char c;
			fread( &c, 1,1, fp );
			chars[i].bytes.push_back(c);
		}
		chars[i].FromBytes();
	}

	fclose(fp);
}

SDL_Surface* RawFont::Char::MakeImg( unsigned long int  )
{
	if(!width) return 0;
	SDL_PixelFormat* pf = SDL_GetVideoSurface()->format;
	SDL_Surface* surf;
	surf=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCCOLORKEY, width, height, pf->BitsPerPixel, pf->Rmask, pf->Gmask, pf->Bmask, pf->Amask);
	unsigned long int trans = SDL_MapRGB(pf,255,0,255);
	SDL_SetColorKey(surf,SDL_SRCCOLORKEY,trans);
	int idx = 0;
	for(int y=0; y<height; ++y)
	{
		for(int x=0; x<width; ++x)
		{
			if( bits[idx++] )
			{
				//Draw_Pixel(surf,x,y,color);
			} else {
				//Draw_Pixel(surf,x,y,trans);
			}
		}
	}
	return surf;
}

void RawFont::Instansiate(Font& f, unsigned long int col)
{
	f.font.clear();

	for(int i=0;i<256;++i)
	{
		f.font.push_back( chars[i].MakeImg(col) );
	}


}