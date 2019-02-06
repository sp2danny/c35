#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Common.h"

struct SDL_Surface;

namespace C35
{
	typedef unsigned char UC;

	struct HSVA { UC h,s,v,a; };
	struct RGBA { UC r,g,b,a; RGBA(); RGBA(UC,UC,UC); RGBA(UC,UC,UC,UC); };

	RGBA HSVA_2_RGBA( const HSVA& );
	HSVA RGBA_2_HSVA( const RGBA& );

	struct CIS;

	struct Surface 
	{
		void Overlay( SDL_Surface*, int,int ) const;
		void Overlay( Surface&, int,int ) const;
		void FromCIS(CIS&);
		void FromCIS(CIS&,UC);
		void FromBMP(char*);
		void FromBMP(char*,RGBA);
		void FromSDL( SDL_Surface* s, int hotx=0,int hoty=0) { surface=s; hx=hotx; hy=hoty; }
		void Free();
		
		static Surface Screen();

		int Width()  const;
		int Height() const;

		Pos Hot() const;
		void Hot(const Pos&);

		void Create(int,int);

		SDL_Surface* SDL() const { return surface; }

		Surface() : surface(0)/*, owner(true)*/ {}

		//~Surface() { if(owner) Free(); }
		//Surface(const Surface& s) { (*this)=s; }
		//Surface& operator=(const Surface& s) { owner=false; surface=s.surface; hx=s.hx; hy=s.hy; }

	private:
		SDL_Surface* surface;
		signed short hx,hy;
		//bool owner;
	};

	SDL_Surface* MakeTransMaskFromImage(SDL_Surface*);
	SDL_Surface* MakeSurface(int,int,SDL_PixelFormat*,int);
	SDL_Surface* MakeSurface(int,int,SDL_PixelFormat*);

	struct BasicAnim;

	struct AnimReflection
	{
		bool Next();
		bool Update(int);
		void Start();
		bool Update();
		void Overlay( SDL_Surface*, int,int );
		void Overlay( SDL_Surface* s, Pos p ) { Overlay(s,p.x,p.y); }
		void Set(BasicAnim*,UC);
		void Set(CIS*,UC);
		void Set(AnimReflection& ar) { (*this)=ar; }
		AnimReflection();
		AnimReflection(BasicAnim*,UC);
		AnimReflection(CIS*,UC);
		int Loopcnt() { return loopcnt; }
	private:
		void clr();
		BasicAnim* ba;
		CIS* cis;
		int current;
		int time,last;
		int loopcnt;
		UC hue;
	};

	struct CIS
	{
		enum PixelType { normal=0, alpha, trans, colimp };

	//private:
		unsigned short w,h;
		signed short hx,hy;
		bool has_dither;
		bool has_trans;
		bool has_colimp;
		UC depth;

		std::vector<PixelType> pixeltypes;
		std::vector<HSVA> pixels;

	public:
		void Load( std::istream& );
		void Save( std::ostream& , int=0 );
		void FromImg( SDL_Surface* image, SDL_Surface* transmask, SDL_Surface* dithermask, SDL_Surface* colimpmask );

		void LoadOld( std::istream& );
		void SaveOld( std::ostream& );

		bool LoadExt(std::string);

		CIS CutOut( int,int,int,int );

		AnimReflection Refl(UC hue);

		void Scale150w(), Scale150h();
		void Scale150() { Scale150w(); Scale150h(); }
		CIS HalfSize();
		void HalfSizeMe() { (*this)=HalfSize(); }
		void Skip3();
		void Crop();
		void MakeWhite();
		void MakeDark();
		void Scale50w(), Scale50h();
		void Scale50() { HalfSizeMe(); }
		void Unimport();

		SDL_Surface* MakeSurface();
		SDL_Surface* MakeSurface(UC hue); // dithered
		SDL_Surface* MakeSurface(UC alpha,UC hue); // blended

		CIS Flip(bool,bool=false,bool=false);

		Surface& Get(UC hue);

		void Instance(UC hue);
		void FreeData();
		void UnInstance();

		int Width() const { return w; } int Height() const { return h; }
		Pos Hot() const { return Pos(hx,hy); }
		void Hot(const Pos& p) { hx=p.x; hy=p.y; }

		std::map<UC,Surface> instance;

		CIS() : w(0), h(0), hx(0), hy(0), depth(4) { instance.clear(); has_dither=has_trans=has_colimp=false; } 

		void LoadPCX(const char* fn);

	private:
		void LoadInternal( std::istream& );
		void SaveInternal( std::ostream& );
	friend
		struct Surface;

	};
	typedef void (CIS::*ClosureCIS)();

	typedef struct BasicAnim
	{
		BasicAnim() : delay(0), repeating(false), jbf(0) {}
		short delay;
		bool repeating;
		short jbf; // jump back frame
		std::vector<CIS> anim;
		void Load( std::istream& );
		void Save( std::ostream& );
		void MakeMirror( BasicAnim& , bool, bool, bool );

		template<ClosureCIS exe> void DoAll() { for(CIS& cis:anim) (cis.*exe)(); }

		bool LoadExt(std::string);

		void Instance(UC hue);
		void FreeData();
		void UnInstance();

		int Size() { return anim.size(); }

		AnimReflection Refl(UC);

		Surface& Get( int, UC );

		void LoadOld(std::istream&),SaveOld(std::ostream&);
	private:
		void SaveInternal(std::ostream&);
		void LoadInternal(std::istream&,bool=false);
	} BA;
	typedef void (BasicAnim::*ClosureBA)();

	typedef struct AnimDir
	{
		struct BAD : BasicAnim {
			short dir,mirrorof;
			bool mirror,flipx,flipy,rot90;
			BAD() : mirror(false) {}
		};
		std::vector<BAD> bad;

		template<ClosureBA exe> void DoAll() { for(BAD& b:bad) (b.*exe)(); }

		bool LoadExt(std::string);
		void Load( std::istream& );
		void Save( std::ostream& );
		BasicAnim& Closest(short);
		void Mirror();
		AnimReflection Refl(short,UC);

		int UseAsFont( SDL_Surface*, Pos, UC, std::string );

		void Instance(UC hue);
		void FreeData();
		void UnInstance();

		void LoadOld(std::istream&),SaveOld(std::ostream&);
	private:
		void SaveInternal(std::ostream&);
		void LoadInternal(std::istream&,bool=false);

		BAD* findexact(short);

	} AD;
	typedef void (AnimDir::*ClosureAD)();

	struct NAV
	{
		std::vector<AnimDir> variants;
		std::string name;
		bool LoadExt(std::string);
		void Load( std::istream& );
		void Save( std::ostream& );
		AnimReflection Refl(short,UC);

		void Instance(UC hue);
		void FreeData();
		void UnInstance();

		template<ClosureAD exe> void DoAll() { for(AnimDir& ad:variants) (ad.*exe)(); }

		void LoadOld(std::istream&),SaveOld(std::ostream&);
	private:
		void SaveInternal(std::ostream&);
		void LoadInternal(std::istream&,bool=false);

	};
	typedef void (NAV::*ClosureNAV)();

	typedef struct AnimCollection
	{
		void AddVariant(std::string,AnimDir);
		std::vector<NAV*> core;
		std::map<std::string,NAV*> mappings;
		bool LoadExt(std::string);
		void Load( std::istream& );
		void Save( std::ostream& );
		AnimReflection Refl(std::string,short,UC);

		std::string default_anim;

		void Instance(UC hue);
		void UnInstance();
		void FreeData();

		std::vector<std::string> CoreNames();
		std::vector<std::string> AllNames();

		~AnimCollection();

		template<ClosureNAV exe> void DoAll() { for(NAV* nav:core) (nav->*exe)(); }

		void LoadOld(std::istream&),SaveOld(std::ostream&);
	private:
		void SaveInternal(std::ostream&);
		void LoadInternal(std::istream&,bool=false);
	} AC;

}
