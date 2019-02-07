
#pragma once

#include <vector>

struct SDL_Surface;

#include "Anim.h"

class Font
{
public:
	Font();
	Font(char*);
	~Font();

	void Clear();
	void Load(char*);

	void Print( SDL_Surface*, const char*, int,int, bool=false );
	void Lines( SDL_Surface*, const char*, int,int, int );

	int Height();
	int Width(const char*);

private:

	std::vector<SDL_Surface*> font;

friend
	class RawFont;

};

class RawFont
{
public:
	RawFont();

	void FromFolder(const char*);
	void SaveFile(const char*);
	void LoadFile(const char*);

	void Instansiate(Font&, unsigned long int);
	void MakeAD(C35::AnimDir& ad,bool shadow=false);

private:

	struct Char {
		unsigned short offset;
		unsigned char length; // in bytes, padded
		unsigned char width;
		unsigned char height;
		std::vector<bool> bits;
		std::vector<unsigned char> bytes;

		void FromBits();
		void FromBytes();
		SDL_Surface* MakeImg(unsigned long int);
		void MakeCIS(C35::CIS& cis,bool shadow);
	};

	unsigned char height;

	Char chars[256];

};