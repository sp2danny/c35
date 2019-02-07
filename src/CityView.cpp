
#include "stdafx.h"

// --------------------------------------------

#include "CityView.h"

#include "City.h"
#include "Resource.h"
#include "Font.h"
#include "Board.h"

using namespace std;

namespace
{
	C35::City* city;
	C35::CityView* view;
	SDL_Surface* gui_tile;
	SDL_Surface* city_cover;
	int WW, HH;
	//TTF_Font* ttf_font;
	//Font* font_b;
	int last_list_width;
	bool done;
}

extern const int SZ;
extern const int YSZ;

extern Font f_w;
extern Font f_b;

void C35::CityView::ViewCity(City* c)
{
	done = false;
	city = c;
	Push(view);
}

void C35::CityView::Init( /*TTF_Font* ttf, Font* blackfont*/ )
{
	view = new CityView;
	//ttf_font = ttf;
	//font_b = blackfont;
	gui_tile = SDL_LoadBMP( "gfx/gui_tile.bmp" );
	city_cover = SDL_LoadBMP( "gfx/CityCover.bmp" );
	SDL_SetColorKey( city_cover, SDL_SRCCOLORKEY, SDL_MapRGB( city_cover->format, 255, 0, 255 ) );
	HH = screen->h;
	WW = screen->w;
}

// 	ShadowText( screen, c->name, WW/2, FH, true, true );

void ShadowText ( SDL_Surface* srf, string text, int x, int y, bool center, bool )
{
	f_b.Print( srf, text.c_str(), x, y, center );
}

void C35::CityView::Update([[maybe_unused]] int ms)
{
}

void C35::CityView::ParseInput(SDL_Event& e)
{
	static int mouse_x, mouse_y;

	if( e.type == SDL_MOUSEMOTION )
	{
		mouse_x = e.motion.x;
		mouse_y = e.motion.y;
	}

	if( e.type == SDL_KEYDOWN )
	{
		if( e.key.keysym.sym == SDLK_ESCAPE )
		{
			done=true;
		}
		if( e.key.keysym.sym == SDLK_RETURN )
		{
			done=true;
		}
		//if(done) GameEngine:: ui->ReObsc();
		if( e.key.keysym.sym == SDLK_UP )
		{
			if(city->building_unit)
			{
				unsigned int idx;
				IdList AU = city->AvailUnit();
				for(idx=0;idx<AU.size();++idx)
					if(AU[idx]==city->build_id) break;
				if(idx) city->build_id=AU[idx-1];
				else city->building_unit=false;
			} else if(city->building_improvement)
			{
				unsigned int idx;
				IdList AI = city->AvailImp();
				for(idx=0;idx<AI.size();++idx)
					if(AI[idx]==city->build_id) break;
				if(idx) city->build_id=AI[idx-1];
				else { city->building_improvement=false; city->building_unit=true; city->build_id = city->AvailUnit().back(); }
			}
		}
		if( e.key.keysym.sym == SDLK_DOWN )
		{
			if(city->building_improvement)
			{
				int idx;
				IdList AI = city->AvailImp();
				int n = AI.size();
				for(idx=0;idx<n;++idx)
					if(AI[idx]==city->build_id) break;
				if( (idx+1)<n ) city->build_id = AI[idx+1];
			}
			else if(city->building_unit)
			{
				int idx;
				IdList AU = city->AvailUnit();
				int n = AU.size();
				for(idx=0;idx<n;++idx)
					if(AU[idx]==city->build_id) break;
				if( (idx+1)<n ) city->build_id = AU[idx+1];
				else { city->building_improvement=true; city->building_unit=false; city->build_id = city->AvailImp().front(); }
			} else {
				city->building_unit=true;
				city->build_id = city->AvailUnit().front();
			}
		}

	}
	if( e.type == SDL_MOUSEBUTTONDOWN )
	{
		if( e.button.button == SDL_BUTTON_LEFT )
		{
			const int FH = 31;
			if( (mouse_x>(FH/2)) && (mouse_x<(last_list_width+FH+FH/2)) && (mouse_y>=(FH*4)) )
			{
				int idx = (mouse_y - (FH*4)) / FH;
				int n1 = city->AvailUnit().size();
				int n2 = city->AvailImp().size();
				if(idx==0)
				{
					city->building_unit = false;
					city->building_improvement = false;
				}
				else if( (idx-1) < n1 )
				{
					city->building_improvement = false;
					city->building_unit = true;
					city->build_id = city->AvailUnit()[idx-1] ;
				}
				else if( (idx-n1-1) < n2 )
				{
					city->building_improvement = true;
					city->building_unit = false;
					city->build_id = city->AvailImp()[idx-n1-1] ;
				}
			}
		}
	}

}

bool C35::CityView::Done()
{
	return done;
}

bool C35::CityView::Unload()
{
	return false;
}

void C35::CityView::Display()
{
	int y=0;
	while(true)
	{
		int x=0;
		while(true)
		{
			SDL_Rect r = { (short)(x),(short)(y),(unsigned short)(gui_tile->w),(unsigned short)(gui_tile->h) };
			SDL_BlitSurface( gui_tile, 0, screen, &r );
			x += gui_tile->w;
			if( x >= WW ) break;
		}
		y += gui_tile->h;
		if( y >= HH ) break;
	}

	//SDL_Color col = { 0,0,0, 0 };
	//SDL_Surface* tt = TTF_RenderText_Solid(ttf_font, city->name.c_str(), col );
	//SDL_Rect tr = { (WW-tt->w)/2, 60, tt->w, tt->h };
	//SDL_BlitSurface( tt, 0, screen, &tr );

	// SDL_Rect MasterRect = { -MARG+hex.px-offs_x-9, -MARG/2+hex.py-offs_y+17, 128,128 }

	//int old_ofs_x = offs_x;
	//int old_ofs_y = offs_y;
	Board* brd = city->owner->game;
	[[maybe_unused]] Hexagon& h = brd->Get(city->x,city->y);
	//offs_x = h.px - WW/2 -MARG -9 + 64;
	//offs_y = h.py - HH/2 -MARG/2 +17 + 64;
	//in_city_view=false;
	[[maybe_unused]] int HW = 110;
	SDL_Rect cr = { (short)(WW/2-SZ*3-15) ,(short)(HH/2-YSZ*3-15), (unsigned short)(SZ*6+30), (unsigned short)(YSZ*6+30) };
	SDL_SetClipRect( SDL_GetVideoSurface(), &cr );
	//Display(false);
	SDL_SetClipRect( SDL_GetVideoSurface(), 0 );
	//in_city_view=true;
	//offs_x = old_ofs_x;
	//offs_y = old_ofs_y;
	cr.x = 143 + (WW-1024)/2;
	cr.y =  78 + (HH- 768)/2;
	SDL_BlitSurface( city_cover, 0, screen, &cr );

	for(int r = 1; r <= 2; ++r )
	{
		if( city->resources[r] )
		{
			[[maybe_unused]] SDL_Rect rr = { (short)(42*r-32),10,32,32 };
			//SDL_BlitSurface( res[r], 0, screen, &rr );
		}
	}

	const int FH = 31;

	ShadowText( screen, city->name, WW/2, FH, true, true );

	//font_sr.UseAsFont( screen, Pos(WW/2,80), 85*2, c->name );

	// list of buildable stuff
	y = FH*4;

	IdList lst;

	//int shl = 0;
	int tw,maxw = f_b.Width("wealth");
	for(int i : city->AvailUnit() )
		if( (tw = f_b.Width(UnitType::Lookup(i).name.c_str())) > maxw ) maxw=tw;
	for(int i : city->AvailImp() )
		if( (tw = f_b.Width(ImprovementType::Lookup(i).name.c_str())) > maxw ) maxw=tw;

	last_list_width=maxw;

	Uint32 grey = SDL_MapRGB(screen->format, 128,128,128 );

	if( !city->building_unit && !city->building_improvement)
	{
		SDL_Rect r = { (short)(FH/2),(short)(y),(unsigned short)(maxw+FH+FH),(unsigned short)(FH) };
		SDL_FillRect(screen,&r,grey);
	}
	ImprovementType::icons.Refl(0,0).Overlay(screen,FH,y);
	ShadowText( screen, "wealth", FH+FH,y, false, true);
	
	lst = city->AvailUnit();
	for(int i : lst )
	{
		y+= FH;
		UnitType& ut = UnitType::Lookup(i);
		if( city->building_unit && city->build_id==i )
		{
			//shl = ut.cost;
			SDL_Rect r = { (short)(FH/2),(short)(y),(unsigned short)(maxw+FH+FH),(unsigned short)(FH) };
			SDL_FillRect(screen,&r,grey);
		}
		ut.GetIcon(GameEngine::PAK()).Overlay(screen, FH,y);
		//unit_icons.Refl(i,human->color).Overlay(screen, FH,y);
		ShadowText( screen, ut.name, FH+FH,y, false, true );
	}

	lst = city->AvailImp();
	for(int i : lst )
	{
		y+= FH;
		ImprovementType& it = ImprovementType::Lookup(i);
		if( city->building_improvement && city->build_id==i )
		{
			//shl = it.cost;
			SDL_Rect r = { (short)(FH/2),(short)(y),(unsigned short)(maxw+FH+FH),(unsigned short)(FH) };
			SDL_FillRect(screen,&r,grey);
		}
		it.Refl().Overlay(screen,FH,y);
		ShadowText( screen, it.name, FH+FH,y, false, true );
	}

	char buff[256];
	sprintf(buff,"food %d / %d", city->foodbox, city->MaxFood() );
	ShadowText( screen, buff, WW/2, FH*4, false, true );
	sprintf(buff,"shield %d / %d", city->shieldbox, city->MaxShield() );
	ShadowText( screen, buff, WW/2, FH*5, false, true );
	sprintf(buff,"population %d", city->size );
	ShadowText( screen, buff, WW/2, FH*6, false, true );
	sprintf(buff,"culture %d", city->culture );
	ShadowText( screen, buff, WW/2, FH*7, false, true );

}


