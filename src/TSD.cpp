
#include "stdafx.h"

// --------------------------------------------

#include "resize++.h"

#include "Tech.h"

#include "Unit.h"
#include "City.h"
#include "Player.h"
#include "FrameSystem.h"
#include "Resource.h"

using namespace std;

// Tech Screen Display
// or
// Terminate Stay Dead
// ?

extern C35::Frame* GetTSD();

namespace C35
{
	class TSD : public Frame
	{
	public:
		TSD();

		virtual void Update(int);
		virtual void Display();
		virtual void ParseInput(SDL_Event&);
		virtual bool Done();
		virtual bool Unload();
	private:
		C35::TechTree* ttp;
		SDL_Surface* bg;
		//TTF_Font* fnt_big;
		//TTF_Font* fnt_sml;
		int mx,my;
		C35::Player* pp;
		float scale;
		bool inited;
		bool done;

		SDL_Surface* box;
		SDL_Surface* box2;
		SDL_Surface* box3;
		SDL_Surface* arrow_wake;

		struct TechBox
		{
			int id;
			int x,y;
			int w,h;
			IdList in,out;
			SDL_Surface* box;
			string name;
			vector<C35::CIS*> icons;
		};

		map<int,TechBox> boxes;

		typedef map<int,TechBox>::iterator MITBI;

		void AddArrows(SDL_Surface* target);
		void TechScreenStart(TechTree* tt,Player* p);
		void Init();
		SDL_Surface* BoxMaker( SDL_Surface* src, int x1,int x2, int y1,int y2, int w,int h );

	friend
		C35::Frame* ::GetTSD();
	};
}

C35::TSD::TSD()
{
	inited=false;
	done=false;
}

void C35::TSD::Init()
{
	if(inited) return;

	bg = SDL_LoadBMP("gfx/AdvGenBG.bmp");
	box = SDL_LoadBMP("gfx/BoxTemplate.bmp");
	arrow_wake = SDL_LoadBMP("gfx/ArrowWake.bmp");
	SDL_SetColorKey(arrow_wake,SDL_SRCCOLORKEY,SDL_MapRGB(arrow_wake->format,255,0,255));

	//auto screen = SDL_GetVideoSurface();

	float scale1 = screen->w / (float)bg->w;
	float scale2 = screen->h / (float)bg->h;
	if(scale1<scale2) scale=scale1; else scale=scale2;

	// x 100, 900
	// y 140, 700

	SDL_Surface* tmp = BoxMaker(bg, 100,900, 140,700, screen->w,screen->h);
	SDL_FreeSurface(bg);
	bg = tmp;
	//bg = SDL_Resize(bg,screen->w,screen->h);

	//fnt_big = TTF_OpenFont("gfx/arial.ttf",36);
	//fnt_sml = TTF_OpenFont("gfx/arial.ttf",12);

	box2 = BoxMaker(box, 13,78, 23,85, 160*scale,70*scale );
	box3 = BoxMaker(box, 13,78, 23,85, 120*scale,70*scale );

	inited=true;
}

void C35::TSD::AddArrows(SDL_Surface* target)
{
	for( auto itr : boxes )
	{
		TechBox& tb = itr.second;
		SDL_Rect dr2 = { tb.x, tb.y };
		int i=2,n = tb.out.size()+3;
		for( int oi : tb.out )
		{
			TechBox& b2 = boxes[oi];
			int x1 = tb.x + tb.w;
			int y1 = tb.y + (i++*tb.h)/n;

			int i2=2, n2=b2.in.size()+3;
			for( int ii : b2.in )
			{
				if( ii == tb.id ) break;
				++i2;
			}

			int x2 = b2.x;
			int y2 = b2.y + (i2*b2.h)/n2;
			x1-=5;
			x2+=5;
			float dx = x2-x1;
			float dy = y2-y1;
			float ddy = dy/dx;
			float y = y1;
			for(int x=x1; x<=x2; ++x)
			{
				SDL_Rect dr = { x,(int)y };
				SDL_BlitSurface(arrow_wake,0,target,&dr);
				y += ddy;
			}
		}
	}
}

void C35::TSD::TechScreenStart(TechTree* tt,Player* p)
{
	pp=p;
	ttp=tt;
	Init();

	boxes.clear();

	for( const TechType& tt : TechType::All() )
	{
		TechBox tb;

		tb.in.clear(); tb.out.clear();

		tb.id = tt.Id;

		tb.x = scale * tt.posX;
		tb.y = scale * tt.posY;

		int ib = tt.enablesImprovement.size();
		int ub = tt.enablesUnit.size();
		int iconcount = ib+ub;

		tb.box = (iconcount>=2)?box2:box3;

		tb.w = tb.box->w;
		tb.h = tb.box->h;

		tb.name = tt.Name;

		for(int rid : tt.enablesResource )
			tb.icons.push_back( & Resource::Icon(rid) );

		if(ib) for(int iid : tt.enablesImprovement)
			tb.icons.push_back( & ImprovementType::Lookup(iid).Icon().anim[0] );

		if(ub) for(int uid : tt.enablesUnit)
			tb.icons.push_back( & UnitType::Lookup(uid).GetIcon().anim[0] );

		boxes[tt.Id] = tb;
	}

	MITBI beg,itr,end;
	beg = boxes.begin();
	end = boxes.end();
	for( itr=beg; itr != end; ++itr )
	{
		TechBox& tb = itr->second;
		TechType& tt = TechType::Lookup(tb.id);

		for( int r : tt.requiresTech )
		{
			tb.in.push_back( r );
			boxes[r].out.push_back( tb.id );
		}
	}

	// sort in and out on y

	for( itr=beg; itr != end; ++itr )
	{
		TechBox& tb = itr->second;

		auto pred = [&] ( int b1, int b2 ) -> bool
		{
			return boxes[b1].y < boxes[b2].y ;
		};

		sort( tb.in.begin(),  tb.in.end(),  pred );
		sort( tb.out.begin(), tb.out.end(), pred );
	}

	AddArrows(bg);
}

void C35::TSD::Display()
{
	//auto screen = SDL_GetVideoSurface();
	//SDL_FillRect(screen,0,0);

	static bool virgin = true;
	if(virgin)
	{
		static TechTree tt;
		TechScreenStart(&tt,GameEngine::PAK());
		virgin=false;
	}

	SDL_Surface* target = screen; // MakeSurface(bg->w,bg->h,screen->format);

	SDL_BlitSurface(bg,0,target,0);

	//SDL_Color col = { 0,0,0, 0 };
	//SDL_Surface* heading = TTF_RenderText_Solid(fnt_big, "Tech Tree", col );
	//SDL_Rect dr = { ( bg->w - heading->w ) / 2 , 15 };
	//SDL_BlitSurface(heading,0,target,&dr);
	//SDL_FreeSurface(heading);

	SDL_Color col2 = { 0,0,64, 0 };
	for( auto itr : boxes )
	{
		TechBox& tb = itr.second;
		//SDL_Rect dr2 = { tb.x, tb.y };
		//SDL_BlitSurface( tb.box,0,target,&dr2);
		//SDL_Surface* txt = TTF_RenderText_Solid(fnt_sml, tb.name.c_str(), col2 );
		//SDL_Rect dr3 = { tb.x+12, tb.y-1 };
		//SDL_BlitSurface(txt,0,target,&dr3);
		//SDL_FreeSurface(txt);
		int x = tb.x+10;
		int y = tb.y+15;
		for(CIS* cis : tb.icons)
		{
			UC hue = GameEngine::PAK()->color;
			cis->Instance(hue);
			cis->Refl(hue).Overlay(target,x,y);
			x+= 26*scale;
		}
		/*
		int i=2,n = tb.out.size()+3;
		for( int oi : tb.out )
		{
			TechBox& b2 = boxes[oi];
			int x1 = tb.x + tb.w;
			int y1 = tb.y + (i++*tb.h)/n;

			int i2=2, n2=b2.in.size()+3;
			for( int ii : b2.in )
			{
				if( ii == tb.id ) break;
				++i2;
			}

			int x2 = b2.x;
			int y2 = b2.y + (i2*b2.h)/n2;
			Draw_Line(target,x1,y1,x2,y2,SDL_MapRGB(target->format,64,64,255));
		}
		*/
	}


	//target = SDL_Resize(target,screen->w,screen->h);

	SDL_BlitSurface(target,0,screen,0);
	//SDL_FreeSurface(target);
	//SDL_Flip(screen);
}

static void mkr( SDL_Rect& r, int x,int y,int w=0,int h=0 )
{
	r.x=x; r.y=y;
	if(w) r.w=w;
	if(h) r.h=h;
}

SDL_Surface* C35::TSD::BoxMaker( SDL_Surface* src, int x1,int x2, int y1,int y2, int w,int h )
{
	int w1 = x1;
	int w2 = x2-x1;
	int w3 = src->w - x2;
	int h1 = y1;
	int h2 = y2-y1;
	int h3 = src->h - y2;

	SDL_PixelFormat* pf = SDL_GetVideoSurface()->format;
	SDL_Surface* result = C35::MakeSurface(w,h,pf);

	SDL_Rect sr,dr;

	{ // TL
		mkr(sr,0,0,w1,h1);
		mkr(dr,0,0);
		SDL_BlitSurface(src,&sr,result,&dr);
	}

	{ // TC
		SDL_Surface* top = C35::MakeSurface(w2,h1,pf);
		mkr(sr,x1,0,w2,h1);
		SDL_BlitSurface(src,&sr,top,0);
		top = SDL_Resize(top,w-(w1+w3),h1);
		mkr(dr,x1,0);
		SDL_BlitSurface(top,0,result,&dr);
		SDL_FreeSurface(top);
	}

	{ // TR
		mkr(sr,x2,0,w3,h1);
		mkr(dr,w-w3,0);
		SDL_BlitSurface(src,&sr,result,&dr);
	}

	{ // LC
		SDL_Surface* lft = C35::MakeSurface(w1,h2,pf);
		mkr(sr,0,y1,w1,h2);
		SDL_BlitSurface(src,&sr,lft,0);
		lft = SDL_Resize(lft,w1,h-(h1+h3));
		mkr(dr,0,y1);
		SDL_BlitSurface(lft,0,result,&dr);
		SDL_FreeSurface(lft);
	}

	{ // C
		SDL_Surface* cnt = C35::MakeSurface(w2,h2,pf);
		mkr(sr,x1,y1,w2,h2);
		SDL_BlitSurface(src,&sr,cnt,0);
		cnt = SDL_Resize(cnt,w-(w1+w3),h-(h1+h3));
		mkr(dr,x1,y1);
		SDL_BlitSurface(cnt,0,result,&dr);
		SDL_FreeSurface(cnt);
	}

	{ // RC
		SDL_Surface* rgt = C35::MakeSurface(w3,h2,pf);
		mkr(sr,x2,y1,w3,h2);
		SDL_BlitSurface(src,&sr,rgt,0);
		rgt = SDL_Resize(rgt,w3,h-(h1+h3));
		mkr(dr,w-w3,y1);
		SDL_BlitSurface(rgt,0,result,&dr);
		SDL_FreeSurface(rgt);
	}

	{ // BL
		mkr(sr,0,y2,w1,h3);
		mkr(dr,0,h-h3);
		SDL_BlitSurface(src,&sr,result,&dr);
	}

	{ // BC
		SDL_Surface* bot = C35::MakeSurface(w2,h3,pf);
		mkr(sr,x1,y2,w2,h3);
		SDL_BlitSurface(src,&sr,bot,0);
		bot = SDL_Resize(bot,w-(w1+w3),h3);
		mkr(dr,x1,h-h3);
		SDL_BlitSurface(bot,0,result,&dr);
		SDL_FreeSurface(bot);
	}

	{ // BR
		mkr(sr,x2,y2,w3,h3);
		mkr(dr,w-w3,h-h3);
		SDL_BlitSurface(src,&sr,result,&dr);
	}

	return result;
}

bool C35::TSD::Done() { return done; }

bool C35::TSD::Unload() { return false; }

void C35::TSD::Update(int ms)
{
}

void C35::TSD::ParseInput(SDL_Event& e)
{

	if( e.type == SDL_KEYDOWN )
	{
		if( e.key.keysym.sym == SDLK_ESCAPE )
		{
			done=true;
		}
		/*if( e.key.keysym.sym == SDLK_SPACE )
		{
			sb=!sb;
		}*/
	}
	else if( e.type == SDL_MOUSEMOTION )
	{
		mx = e.motion.x;
		my = e.motion.y;
	}

}

C35::Frame* GetTSD()
{
	static C35::TSD* tsd = 0;
	if(!tsd) tsd = new C35::TSD();
	tsd->done = false;
	return tsd;
}
