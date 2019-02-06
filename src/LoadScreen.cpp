
#include "stdafx.h"

// --------------------------------------------

#include "FrameSystem.h"
#include "FrameForwards.h"
#include "sdlGuiColl.h"
#include "Anim.h"
#include "GameEngine.h"
#include "AssetRepository.h"
#include "Font.h"

using namespace std;

namespace C35
{
	class LoadScreen : public Frame
	{
	public:
		LoadScreen();
		~LoadScreen();
		virtual void Update(int);
		virtual void Display();
		virtual void ParseInput(SDL_Event&);
		virtual bool Done();
		virtual bool Unload();
	private:

		void RandomMap();
		void LoadMap();
		void LoadGame();

		SDL_Surface* bg;
		bool done;

		AnimReflection refl_n;

	};

}


	int mx,my;
	bool lmd;
	Font f_w;
	Font f_b;
	int ox,oy;

C35::Frame* GetLoadScreen()
{
	static C35::LoadScreen* loadscreen =0;

	if(!loadscreen) loadscreen = new C35::LoadScreen();

	return loadscreen;
}

C35::LoadScreen::LoadScreen()
{
	bg = SDL_LoadBMP( "gfx/AdvGenBG.bmp" );
	lmd = done = false;
	mx=my=0;
	RawFont rf;
	rf.LoadFile("gfx/arial.sdlf");
	SDL_PixelFormat* pf = SDL_GetVideoSurface()->format;
	rf.Instansiate( f_w, SDL_MapRGB( pf, 255,255,255 ) );
	rf.Instansiate( f_b, SDL_MapRGB( pf, 0,0,0 ) );

	AD& night = Asset<AD>("gfx/knight/walk.ad");

	night.Instance(70);
	refl_n = night.Refl(270,70);
	night.FreeData();

	ox = ( screen->w - bg->w ) / 2;
	oy = ( screen->h - bg->h ) / 2;

}

C35::LoadScreen::~LoadScreen()
{
	SDL_FreeSurface(bg);
}

void C35::LoadScreen::Update(int ms)
{
	refl_n.Update( ms );
}

void C35::LoadScreen::RandomMap()
{
	//GameEngine::Init();
	//GameEngine::RandomMap();
	//Push( GameEngine::GUI() );
}

void C35::LoadScreen::LoadMap()
{
	GameEngine::Init();
	GameEngine::LoadMap("map-01.m35");
	Push( GameEngine::GUI() );
}

void C35::LoadScreen::LoadGame()
{
	//GameEngine::Init();
	//GameEngine::LoadGame("save-01.s35");
	//Push( GameEngine::GUI() );
}

int wrap_dist( int a, int b, int sz )
{
	int m1 = a-b;
	int m2 = a-b-sz;
	int m3 = a-b+sz;

	if( abs(m1) < abs(m2) )
	{
		if( abs(m1) < abs(m3) )
			return m1;
	}
	if( abs(m2) < abs(m3) )
		return m2;
	else
		return m3;
}

int wrap( int a, int sz )
{
	while( a <   0 ) a += sz;
	while( a >= sz ) a -= sz;
	return a;
}
/*
const int NUM_PASS = 4;

void HuePress()
{
	int i,j,k;

	for(j=0;j<C35::NUM_NIG;++j) ha[j] = 17*(j+1);

	bool did;
	for(i=0; i<NUM_PASS; ++i) // max 4 passes
	{
		did=false;
		int press[C35::NUM_NIG] = {0};
		for(j=0;j<4;++j) // for all
		{
			for(k=0;k<C35::NUM_NIG;++k) // find all pressures
			{
				if(j==k) continue;
				int wd = wrap_dist(ha[j],ha[k],256) ;
				if( abs(wd) < 16 )
				{
					// heres press
					if(wd<0)
						press[j] -= (16+wd) / 4;
					else
						press[j] += (16-wd) / 4;
				}

			}
		}
		// now we have all pressures, move em
		for(j=0;j<C35::NUM_NIG;++j)
		{
			if(press[j])
			{
				ha[j] += press[j];
				ha[j] = wrap( ha[j], 256 );
				did = true;
			}
		}

		// abort if nothing
		if(!did) break;
	}

	message = ": " + IntToStr(ha[0]);
	for(j=1;j<C35::NUM_NIG;++j)
		message += ", " + IntToStr(ha[j]);
} 

void MakeBarb()
{
	using namespace C35;
	static AC barb;
	barb.LoadExt("gfx/units/warrior.ac");
	barb.DoAll<&NAV::DoAll<&AD::DoAll<&BA::DoAll<&CIS::MakeWhite>>>>();
	barb.DoAll<&NAV::DoAll<&AD::DoAll<&BA::DoAll<&CIS::MakeWhite>>>>();
	barb.Instance(30);
	barb.DoAll<&NAV::DoAll<&AD::DoAll<&BA::DoAll<&CIS::Unimport>>>>();
	ofstream ofs("gfx/units/barbarian.ac",ios::out|ios::binary|ios::trunc);
	barb.Save(ofs);
} */

void C35::LoadScreen::Display()
{
	SDL_Rect r = { ox,oy };
	SDL_BlitSurface( bg,0,screen,&r);

	refl_n.Overlay(screen,300+ox,500+oy);

	int y = 150+oy;
	if( Button(850+ox,y+=80,"Random Map") ) if(lmd) { lmd=false; RandomMap(); }
	if( Button(850+ox,y+=80,"Load Map")   ) if(lmd) { lmd=false; LoadMap();   }
	if( Button(850+ox,y+=80,"Load Game")  ) if(lmd) { lmd=false; LoadGame();  }
	//if( Button(850+ox,y+=80,"Make Barb")  ) if(lmd) { lmd=false; MakeBarb();  }
	if( Button(850+ox,y+=80,"Quit")       ) if(lmd) { done=true; }

	//f_b.Print(screen,message.c_str(), 11+ox,701+oy );
	//f_w.Print(screen,message.c_str(), 10+ox,700+oy );
}

void C35::LoadScreen::ParseInput(SDL_Event& e)
{
	if( e.type == SDL_KEYDOWN )
	{
		if( e.key.keysym.sym == SDLK_ESCAPE )
			done=true;
	}

	if( e.type == SDL_MOUSEBUTTONDOWN )
		if( e.button.button == SDL_BUTTON_LEFT )
			lmd = true;
	if( e.type == SDL_MOUSEBUTTONUP )
		if( e.button.button == SDL_BUTTON_LEFT )
			lmd = false;

	if( e.type == SDL_MOUSEMOTION )
	{
		mx = e.motion.x;
		my = e.motion.y;
	}
}

bool  C35::LoadScreen::Done()
{
	return done ;
}

bool C35::LoadScreen::Unload()
{
	return false;
}

// ----------------------------------------------------------------------------------------------------------



