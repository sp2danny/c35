
#include "stdafx.h"

// --------------------------------------------

#include "MiniMap.h"

#include "Board.h"
#include "Anim.h"
#include "GameEngine.h"
#include "AssetRepository.h"
#include "MakeFrame.h"

using namespace C35;

namespace 
{
	AnimReflection
		mm_g, mm_s, mm_c, mm_d, mm_m, 
		mm_n, mm_o, mm_p, mm_r,
		mm_oo, mm_oe;
	CIS* mm_i;

	SDL_Surface* minimap;
	SDL_Surface* mm_frame=0;
	int w,h;
	int WW, HH;
}

	struct MMSettings
	{
		enum Town { t_all, t_mine, t_none } town;
		enum Res  { r_horse, r_iron, r_none } resource;
		enum Geo  { g_geo, g_influ } geografy;
		enum Unit { u_all, u_enemy, u_none } units;
		MMSettings() : town(t_all), resource(r_none), geografy(g_geo), units(u_none), t_out(false), r_out(false), g_out(false), u_out(false) {}
		bool t_out, r_out, g_out, u_out;
		bool ParseClick(int,int);
		std::string ToolTip(int,int);
		void Display(SDL_Surface*);
	} mmSett;

CIS* ico_town   = 0;
CIS* ico_geo    = 0;
CIS* ico_notown = 0;
CIS* ico_influ  = 0;
CIS* ico_horse  = 0;
CIS* ico_iron   = 0;
CIS* ico_nores  = 0;
CIS* ico_unit   = 0;
CIS* ico_nounit = 0;

void C35::MiniMap::Init()
{
	CIS* cis;
	cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/mmm_g.bmp" ) ) ; cis->Instance(0); mm_g = cis->Refl(0);
	cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/mmm_o.bmp" ) ) ; cis->Instance(0); mm_o = cis->Refl(0);
	cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/mmm_s.bmp" ) ) ; cis->Instance(0); mm_s = cis->Refl(0);
	cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/mmm_c.bmp" ) ) ; cis->Instance(0); mm_c = cis->Refl(0);
	cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/mmm_d.bmp" ) ) ; cis->Instance(0); mm_d = cis->Refl(0);
	cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/mmm_m.bmp" ) ) ; cis->Instance(0); mm_m = cis->Refl(0);
	cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/mmm_p.bmp" ) ) ; cis->Instance(0); mm_p = cis->Refl(0);

	cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/mmm_o_o.bmp" ) ) ; cis->Instance(0); mm_oo = cis->Refl(0);
	cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/mmm_o_e.bmp" ) ) ; cis->Instance(0); mm_oe = cis->Refl(0);

	mm_i = & (CIS&) Asset<CIS>  ( "gfx/MiniMap/mmm_i.cis" ) ;
	for( UC hue : GameEngine::Game().AllPlayerColors() )
		mm_i->Instance(hue);

	ico_town   = cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/ico_town.bmp"   ) ) ; cis->Instance(0);
	ico_geo    = cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/ico_geo.bmp"    ) ) ; cis->Instance(0);
	ico_notown = cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/ico_notown.bmp" ) ) ; cis->Instance(0);
	ico_influ  = cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/ico_influ.bmp"  ) ) ; cis->Instance(0);
	ico_horse  = cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/ico_horse.bmp"  ) ) ; cis->Instance(0);
	ico_iron   = cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/ico_iron.bmp"   ) ) ; cis->Instance(0);
	ico_nores  = cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/ico_nores.bmp"  ) ) ; cis->Instance(0);
	ico_unit   = cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/ico_unit.bmp"   ) ) ; cis->Instance(0);
	ico_nounit = cis = & ( (CIS&) Asset<CIS> ( "gfx/MiniMap/ico_nounit.bmp" ) ) ; cis->Instance(0);

	minimap = 0;
}

bool C35::MiniMap::ParseClick( int mx,int my, float& ofsx,float& ofsy )
{
	if(!mm_frame) return false;

	if( mx > mm_frame->w ) return mmSett.ParseClick(mx,my);
	if( my > mm_frame->h ) return false;

	extern const int SZ, YSZ;

	float fx = (w*SZ)  ;
	float fy = (h*YSZ) ;

	ofsx = (mx / (float)mm_frame->w) * fx - WW/2;
	ofsy = (my / (float)mm_frame->h) * fy - HH/2;

	return true;
}

void C35::MiniMap::Update(PlayerBoard& brd)
{
	//if(minimap) return;

	//Board* brd = &GameEngine::Game();

	if(!minimap)
	{
		w =brd.Width();
		h =brd.Height();
		minimap = MakeSurface( 4*(w+1) , 3*h, SDL_GetVideoSurface()->format );
	}

	WW = SDL_GetVideoSurface()->w;
	HH = SDL_GetVideoSurface()->h;

	for(int y=0;y<h;++y) for(int x=0;x<w;++x)
	{
		int xx = 4*x + ((y%2)?2:0);
		int yy = 3*y;
		PlayerHex& hex = brd.Get(x,y);
		if(hex.obscure==0)
		{
			mm_o.Overlay(minimap,xx,yy);
			continue;
		}
		switch(hex.tile)
		{
			case grass  :   
			case bonus  :   
			case hill   :   mm_g.Overlay(minimap,xx,yy);  break;
			case plains :   mm_p.Overlay(minimap,xx,yy);  break;
			case desert :   
			case floodp :   
			case tundra :   mm_d.Overlay(minimap,xx,yy);  break;
			case mount  :   mm_m.Overlay(minimap,xx,yy);  break;
			case coast  :   
			case sea    :   
			case ocean  :   mm_s.Overlay(minimap,xx,yy);  break;
		}
		if(hex.obscure==1)
			((y%2)?mm_oo:mm_oe).Overlay(minimap,xx,yy);
	}

	int w2 = brd.Width()*4;
	int h2 = brd.Height()*3;
	w2+=4; h2+=3;
	if(!mm_frame) mm_frame = MakeFrame(w2,h2);

}

void  C35::MiniMap::Display(SDL_Surface* dst, int tx,int ty)
{
	SDL_Rect dr = { tx,ty };
	SDL_BlitSurface( minimap,  0, dst, &dr );

	extern float offs_x, offs_y;
	extern const int SZ, YSZ;

	{
		int x = (int) ( (4.0f * offs_x) / SZ ) ;
		int y = (int) ( (3.0f * offs_y) / YSZ ) ;
		int w = (int) ( WW * 4.0f / SZ ) ;
		int h = (int) ( HH * 3.0f / YSZ ) ;

		Board* brd = &GameEngine::Game();
		int w2 = brd->Width()*4;
		int h2 = brd->Height()*3;
		SDL_Rect cr = { 0,0,w2,h2 };
		Uint32 white = SDL_MapRGB( dst->format, 255,255,255 );
		if( (offs_x>=0) && (offs_y>=0) )
		{
			Draw_Line( dst, x,y, x,y+h , white );
			Draw_Line( dst, (x+w)%w2,y, (x+w)%w2,y+h , white );
			if( (x+w) > w2 )
			{
				Draw_Line( dst, x,y, w2-1,y , white );
				Draw_Line( dst, x,y+h, w2-1,y+h , white );
				Draw_Line( dst, 0,y, (x+w)%w2,y , white );
				Draw_Line( dst, 0,y+h, (x+w)%w2,y+h , white );
			} else {
				Draw_Line( dst, x,y, x+w,y , white );
				Draw_Line( dst, x,y+h, x+w,y+h , white );
			}
		}
	}

	SDL_BlitSurface( mm_frame, 0, dst, &dr );

	mmSett.Display(dst);
}

static bool in_rect( int x, int y, const SDL_Rect& r)
{
	return
		( x >= r.x       ) &&
		( x <  (r.x+r.w) ) &&
		( y >= r.y       ) &&
		( y <  (r.y+r.h) ) ;
}

#define RECTS \
	int mmw = GameEngine::Game().Width()*4+12; \
	SDL_Rect tr = { mmw,   8, 24,24 }; \
	SDL_Rect gr = { mmw,  40, 24,24 }; \
	SDL_Rect rr = { mmw,  72, 24,24 }; \
	SDL_Rect ur = { mmw, 104, 24,24 }

bool MMSettings::ParseClick(int mx,int my)
{
	RECTS;

	if(in_rect(mx,my,tr))
	{
		town = (Town)((town+1)%3);
		return true;
	}
	if(in_rect(mx,my,gr))
	{
		geografy = (Geo)((geografy+1)%2);
		return true;
	}
	if(in_rect(mx,my,rr))
	{
		resource = (Res)((resource+1)%3);
		return true;
	}
	if(in_rect(mx,my,ur))
	{
		units = (Unit)((units+1)%3);
		return true;
	}

	return false;
}

std::string MMSettings::ToolTip(int mx,int my)
{
	RECTS;

	if(in_rect(mx,my,tr))
	{
		switch(this->town)
		{
			case t_all:  return "Showing all towns";
			case t_mine: return "Showing own towns";
			case t_none: return "Not showing towns";
		}
	}
	if(in_rect(mx,my,gr))
	{
		switch(this->geografy)
		{
			case g_geo:    return "Showing geografy";
			case g_influ:  return "Showing influence";
		}
	}
	if(in_rect(mx,my,rr))
	{
		switch(this->resource)
		{
			case r_horse: return "Showing horses";
			case r_iron:  return "Showing iron";
			case r_none:  return "Not showing resources";
		}
	}
	if(in_rect(mx,my,ur))
	{
		switch(this->units)
		{
			case u_all:   return "Showing all units";
			case u_none:  return "Not showing units";
			case u_enemy: return "Showing enemies";
		}
	}
	return "";
}

void MMSettings::Display(SDL_Surface* vids)
{
	RECTS;

	//SDL_Surface* vids = SDL_GetVideoSurface();

	switch(this->town)
	{
		case t_all:  ico_town    ->Refl(0).Overlay(vids, tr.x, tr.y ); break;
		case t_mine: ico_town    ->Refl(0).Overlay(vids, tr.x, tr.y ); break;
		case t_none: ico_notown  ->Refl(0).Overlay(vids, tr.x, tr.y ); break;
	}
	
	switch(this->geografy)
	{
		case g_geo:   ico_geo    ->Refl(0).Overlay(vids, gr.x, gr.y ); break;
		case g_influ: ico_influ  ->Refl(0).Overlay(vids, gr.x, gr.y ); break;
	}

	switch(this->resource)
	{
		case r_horse: ico_horse  ->Refl(0).Overlay(vids, rr.x, rr.y ); break;
		case r_iron:  ico_iron   ->Refl(0).Overlay(vids, rr.x, rr.y ); break;
		case r_none:  ico_nores  ->Refl(0).Overlay(vids, rr.x, rr.y ); break;
	}

	switch(this->units)
	{
		case u_all:   ico_unit    ->Refl(0).Overlay(vids, ur.x, ur.y ); break;
		case u_enemy: ico_unit    ->Refl(0).Overlay(vids, ur.x, ur.y ); break;
		case u_none:  ico_nounit  ->Refl(0).Overlay(vids, ur.x, ur.y ); break;
	}

}

#undef RECTS




