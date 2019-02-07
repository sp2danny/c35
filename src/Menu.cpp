
#include "stdafx.h"

// --------------------------------------------

#include "Menu.h"

#include "Player.h"
#include "City.h"
#include "Unit.h"
#include "Anim.h"

using namespace std;
using namespace C35;

namespace
{
	BA town_icon;
	BA tile_icon;
	CIS highlight;
	CIS normal;
	AC actions;
	//CIS actions;
	CIS pullout;
	CIS pulldown;
	Font* ff;
	UC menu_hue;

	CIS mv_fg, mv_fy, mv_e, mv_1, mv_2 ;

	CIS hp_e, hp_r, hp_y, hp_g ;

	CIS rank_insig[10];
	CIS unit_type[5];
	CIS perks[5];

	AnimReflection Zoom,ZoomHL, Goto,GotoHL, Fortify,FortifyHL, Wake,WakeHL;

	bool have = false;

	SDL_Surface* surface = 0;

	//TTF_Font* sf;

	struct SubLine
	{
		Unit* u;
		bool own,hl;
	};

	struct Line
	{
		enum { lk_hex, lk_unit, lk_town } kind;
		string name;
		int count;
		int idx;
		enum { sl_l, sl_lo, sl_d, sl_r, sl_ro, sl_none } slk;
		bool hl;
		bool own;
		UC hue;
		Line() : slk(sl_r), hl(false) {}
		vector<SubLine> subs;
		City* city;
	};

	void PrintText( SDL_Surface* target, string text, int x, int y, bool center = false )
	//void PrintText( SDL_Surface* target, string text, int x, int y, bool center = false )
	{
		/*SDL_Color col = { 0,0,0 };
		SDL_Surface* tt = TTF_RenderText_Solid(sf, text.c_str(), col );
		SDL_Rect tr = { x, y, tt->w, tt->h };
		if(center) tr.x -= tt->w/2;
		SDL_BlitSurface( tt, 0, target, &tr );*/
	}

	int form(int x)
	{
#ifdef _DEBUG
		x *= 2;
		x /= 3;
#endif
		// x /= 2;
		return x;
	}

	void DrawLine( SDL_Surface* target, int linenum, int x, int y, Line& l )
	{

		//int y = normal.h * linenum;
		SDL_Rect rr = { 0, y, normal.Width(), normal.Height() };
		(l.hl?highlight:normal).Get(menu_hue).Overlay(target,0,y);
		switch(l.kind)
		{
		case Line::lk_hex:
			tile_icon.Get(0,menu_hue).Overlay(target,1,y);
			PrintText(target,l.name,form(35),y+3);
			//ff->Print(target,l.name.c_str(),35,y+3);
			break;
		case Line::lk_unit:
			UnitType::Lookup(l.idx).GetIcon(l.hue).Overlay(target,1,y);
			//unit_icons.Closest(l.idx).Get(0,l.hue).Overlay(target,1,y);

			PrintText(target,l.name,form(35),y+3);
			PrintText(target,IntToStr(l.count),form(165),y+3);

			if(l.own)
			{
				int a = form(210   );
				int b = form(210+32);
				int c = form(210+64);
				int d = form(210+96);
				( ( l.hl && (x>a) && (x<b) ) ? FortifyHL : Fortify ) .Overlay(target,a,y);
				( ( l.hl && (x>b) && (x<c) ) ? GotoHL    : Goto    ) .Overlay(target,b,y);
				( ( l.hl && (x>c) && (x<d) ) ? WakeHL    : Wake    ) .Overlay(target,c,y);
			}

			break;
		case Line::lk_town:
			town_icon.Get(0,GameEngine::PAK()->color).Overlay(target,1,y);
			PrintText(target,l.name,form(35),y+3);
			if(l.own)
			{
				int a = form(210+32);
				int b = form(210+64);
				int c = form(210+96);
				( ( l.hl && (x>a) && (x<b) ) ? ZoomHL : Zoom  ) .Overlay(target,a,y);
				( ( l.hl && (x>b) && (x<c) ) ? GotoHL : Goto  ) .Overlay(target,b,y);
			}
			break;
		}
		switch(l.slk)
		{
			case Line::sl_r:
			case Line::sl_ro:
				pullout.Get(menu_hue).Overlay(target,form(315),y);
				break;
			case Line::sl_d:
				pulldown.Get(menu_hue).Overlay(target,form(315),y);
				break;
			default: break;
		}
	}

	vector<Line> lines;

	int menu_x, menu_y;

}

void Fix(CIS& cis, UC hue)
{
	cis.Hot(Pos(0,0));

	//cis.HalfSizeMe();
	//cis.Scale150();
#ifdef _DEBUG
	cis.Skip3();
#endif
	cis.Instance(hue);
}

void Fix(BA& ba,UC hue)
{
	for( CIS& cis : ba.anim)
	{
		cis.Hot(Pos(0,0));//cis.hx=cis.hy=0;
	}
	//ba.DoAll< &CIS::HalfSizeMe > ();
	//ba.DoAll< &CIS::Scale150 > ();
#ifdef _DEBUG
	ba.DoAll< &CIS::Skip3 > ();
#endif
	ba.Instance(hue);
}

void Fix(AC& ac,UC hue)
{
	for( NAV* nav : ac.core )
		for(AD& ad : nav->variants)
			for(BA& ba : ad.bad )
				Fix(ba,hue);
	ac.Instance(hue);
}

void C35::Menu::InitMenu(Font& f)
{
	town_icon  .LoadExt( "gfx/menu/town.bmp"      );
	tile_icon  .LoadExt( "gfx/menu/tile.bmp"      );
	highlight  .LoadExt( "gfx/menu/bar_hl.cis"    );
	normal     .LoadExt( "gfx/menu/bar.cis"       );
	actions    .LoadExt( "gfx/menu/ActionBtn.ac"  );
	pullout    .LoadExt( "gfx/menu/pull_r.cis"    );
	pulldown   .LoadExt( "gfx/menu/pull_d.cis"    );

	mv_fg      .LoadExt( "gfx/menu/mi_fg.bmp"    );
	mv_fy      .LoadExt( "gfx/menu/mi_fy.bmp"    );
	mv_e       .LoadExt( "gfx/menu/mi_e.bmp"     );
	mv_1       .LoadExt( "gfx/menu/mi_1.bmp"     );
	mv_2       .LoadExt( "gfx/menu/mi_2.bmp"     );

	hp_e       .LoadExt( "gfx/menu/hp_e.bmp"     );
	hp_r       .LoadExt( "gfx/menu/hp_r.bmp"     );
	hp_y       .LoadExt( "gfx/menu/hp_y.bmp"     );
	hp_g       .LoadExt( "gfx/menu/hp_g.bmp"     );

	for(int r=0;r<=9;++r)
	{
		char buff[256];
		sprintf(buff,"gfx/menu/insig_%d.cis",r);
		rank_insig[r] .LoadExt( buff );
	}
	// struct UnitType { enum { Foot, Mounted, Vehicle, Sea, Air } kind;
	unit_type[UnitType::Foot]    .LoadExt( "gfx/menu/foot.bmp"      );
	unit_type[UnitType::Mounted] .LoadExt( "gfx/menu/mounted.bmp"   );
	unit_type[UnitType::Vehicle] .LoadExt( "gfx/menu/vehicle.bmp"   );
	unit_type[UnitType::Sea]     .LoadExt( "gfx/menu/sea.bmp"       );
	unit_type[UnitType::Air]     .LoadExt( "gfx/menu/air.bmp"       );

	perks[ UnitType::OpenTerrainCombat ] . LoadExt( "gfx/menu/OpenTerrain.bmp"    );
	perks[ UnitType::MountedDefense    ] . LoadExt( "gfx/menu/MountedDefense.bmp" );
	perks[ UnitType::MeleeCombat       ] . LoadExt( "gfx/menu/FootCombat.bmp"     );


	ff = &f;

	//unit_icons.DoAll<&BA::DoAll<&CIS::HalfSizeMe>>();
	//unit_icons.DoAll<&BA::DoAll<&CIS::Scale150>>();
#ifdef _DEBUG
	//unit_icons.DoAll<&BA::DoAll<&CIS::Skip3>>();
#endif
	//unit_icons .Instance(player_hue);
	Fix( town_icon ,GameEngine::PAK()->color);
	menu_hue = /*65; // */ 160;
	Fix( tile_icon  ,menu_hue);
	Fix( highlight  ,menu_hue);
	Fix( normal     ,menu_hue);
	Fix( actions    ,menu_hue);
	Fix( pullout    ,menu_hue);
	Fix( pulldown   ,menu_hue);

	Fix( perks[ UnitType::OpenTerrainCombat ] ,menu_hue);
	Fix( perks[ UnitType::MountedDefense    ] ,menu_hue);
	Fix( perks[ UnitType::MeleeCombat       ] ,menu_hue);

	Fix( mv_fg , menu_hue);
	Fix( mv_fy , menu_hue);
	Fix( mv_e  , menu_hue);
	Fix( mv_1  , menu_hue);
	Fix( mv_2  , menu_hue);

	Fix( hp_e  , menu_hue);
	Fix( hp_r  , menu_hue);
	Fix( hp_y  , menu_hue);
	Fix( hp_g  , menu_hue);

	for(int r=0;r<=9;++r) Fix( rank_insig[r], GameEngine::PAK()->color );
	for(int t=UnitType::Foot; t<=UnitType::Air; ++t) Fix( unit_type[t], menu_hue );

	Done();

	Zoom      = actions.Refl("Zoom",      0, menu_hue);
	ZoomHL    = actions.Refl("ZoomHL",    0, menu_hue);
	Goto      = actions.Refl("Goto",      0, menu_hue);
	GotoHL    = actions.Refl("GotoHL",    0, menu_hue);
	Fortify   = actions.Refl("Fortify",   0, menu_hue);
	FortifyHL = actions.Refl("FortifyHL", 0, menu_hue);
	Wake      = actions.Refl("Wake",      0, menu_hue);
	WakeHL    = actions.Refl("WakeHL",    0, menu_hue);

/*
#ifdef _DEBUG
	sf=TTF_OpenFont("gfx/arial.ttf",16);
#else
	sf=TTF_OpenFont("gfx/arial.ttf",24);
#endif
*/

}

void C35::Menu::Done()
{
	lines.clear();
	have=false;
	if(surface)
		SDL_FreeSurface(surface);
	surface=0;
}

string HexName(C35::PlayerHex& hex)
{
	if(hex.obscure==0) return "Unexplored";
	switch(hex.tile)
	{
		case C35::grass   : return "Grassland";  
		case C35::bonus   : return "Grassland";  
		case C35::plains  : return "Plains";     
		case C35::desert  : return "Desert";     
		case C35::floodp  : return "Floodplains";
		case C35::tundra  : return "Tundra";     
		case C35::hill    : return "Hill";       
		case C35::mount   : return "Mountains";  
		case C35::coast   : return "Coast";      
		case C35::sea     : return "Sea";        
		case C35::ocean   : return "Ocean";      
		default:            return "Unknown";    
	}
}

void C35::Menu::MakeFrom(PlayerHex& h, int mx,int my)
{
	if(have) Done();

	Line l;
	l.kind = Line::lk_hex;
	l.name = HexName(h);
	l.slk = Line::sl_none;
	lines.push_back(l);

	int maxlines = 1;

	bool obs = h.obscure==0;

	if(h.city&&!obs)
	{
		Line l;
		l.kind = Line::lk_town;
		l.name = h.city->name;
		l.slk = Line::sl_none;
		l.city = h.city;
		l.own = h.city->owner == GameEngine::PAK();
		lines.push_back(l);
		maxlines += 1;
	}

	int i,n = UnitType::Size();

	if(h.units.size()&&!obs)
	{
		UC ph = h.units[0]->owner->color;
		//unit_icons.Instance(ph);
		bool own = h.units[0]->owner == GameEngine::PAK();

		for( i=1; i<=n; ++i)
		{
			int cnt = 0;
			vector<SubLine> slv;
			for( Unit* u : h.units )
			{
				if(u->type == i)
				{
					++cnt;
					SubLine sl;
					sl.own = own;
					sl.hl = false;
					sl.u = u;
					slv.push_back(sl);
				}
			}

			if(cnt)
			{
				UnitType& ut = UnitType::Lookup(i);
				Line l;
				l.kind = Line::lk_unit;
				l.name = ut.name;
				l.count = cnt;
				l.idx = i;
				l.hue = ph;
				l.own = own;
				l.subs = slv;
				lines.push_back(l);
				maxlines += 1 + slv.size();
			}
		}
	}

	menu_x = mx;
	menu_y = my;

	n = lines.size();
	surface = MakeSurface( normal.Width(), maxlines*normal.Height(), SDL_GetVideoSurface()->format );
	int y = 0;
	for(i=0;i<n;++i)
	{
		DrawLine(surface,i, -1,y, lines[i]);
		y += normal.Height();
	}

	static bool first = true;
	if(first)
	{
		first=false;
		//SDL_SaveBMP( surface, "gfx/menu/text.bmp");
	}

	have = true;
}

bool C35::Menu::Active()
{
	return have;
}

void DrawSub( SDL_Surface* target, SubLine& sub, int x, int y )
{
	if(sub.hl)
		highlight.Get( menu_hue ).Overlay(target,x,y);
	else
		normal.Get( menu_hue ).Overlay(target,x,y);
	Stats& st = sub.u->stats;

	// ffPrint( target, ("lvl"+IntToStr(st.lev+1)).c_str(), x+form(50), y+3);

	UnitType& ut = UnitType::Lookup( sub.u->type );
	bool notype = ( ut.basetype == UnitType::None );

	if(!notype)
	{
		rank_insig[st.lev+1].Instance(GameEngine::PAK()->color);
		rank_insig[st.lev+1].Refl(GameEngine::PAK()->color).Overlay( target,  x+form(50), y );
	}

	#define ffPrint PrintText
	ffPrint( target, (IntToStr(st.off)+","+IntToStr(st.def)).c_str(), x+form(110), y+3);
	#undef ffPrint

	int mv = sub.own ? st.move : 0;
	if( mv == st.maxmove*3000 )
	{
		for(int i=0; i<st.maxmove; ++i)
			mv_fg.Refl(menu_hue).Overlay( target, x+form(170)+form(32)*i, y );
	} else {
		float ml = mv/3000.0f;
		float mm = 0;
		for(int i=0; i<st.maxmove; ++i)
		{
			/**/ if(ml>(mm+0.9f))
				mv_fy.Refl(menu_hue).Overlay( target, x+form(170)+form(32)*i, y );
			else if(ml>(mm+0.6f))
				mv_2.Refl(menu_hue).Overlay( target, x+form(170)+form(32)*i, y );
			else if(ml>(mm+0.3f))
				mv_1.Refl(menu_hue).Overlay( target, x+form(170)+form(32)*i, y );
			else
				mv_e.Refl(menu_hue).Overlay( target, x+form(170)+form(32)*i, y );
			mm += 1.0f;
		}
	}

	if(!notype)
	{
		CIS* full;

		if(st.hp==1 )
			full = &hp_r;
		else if(st.hp==st.maxhp)
			full = &hp_g;
		else
			full = &hp_y;

		for( int i=0; i<st.maxhp; ++i)
		{
			if( i>=st.hp )
				hp_e.Refl(menu_hue).Overlay( target, x+form(300)+form(7)*i, y);
			else
				full->Refl(menu_hue).Overlay( target, x+form(300)+form(7)*i, y);
		}
	}

}

void C35::Menu::Display(int mx,int my)
{
	static long time = SDL_GetTicks();
	long now = SDL_GetTicks();
	long dif = now-time; time=now;
	AnimReflection*  ara[] = { &Zoom,&ZoomHL, &Goto,&GotoHL, &Fortify,&FortifyHL, &Wake,&WakeHL };
	for( AnimReflection* ar : ara ) ar->Update(dif);

	int i,n = lines.size();
	int nn = n;
	for(i=0;i<n;++i)
	{
		if(lines[i].slk==Line::sl_d)
			nn += lines[i].subs.size();
	}

	int hl = -1;
	if( (mx > menu_x) && (mx < (menu_x+normal.Width()) ) )
		if( (my > menu_y) && (my < (menu_y+nn*normal.Height()) ) )
			hl = (my - menu_y) / normal.Height() ;

	int ii = 0;
	int y = 0;
	int dx = mx-menu_x;
	for(i=0;i<n;++i)
	{
		Line& ll = lines[i];
		ll.hl = (hl==ii);
		DrawLine(surface,i,(hl==ii)?dx:-1,y,ll);
		y+=normal.Height(); ii+=1;

		if(ll.slk==Line::sl_d)
		{
			for(int j=0;j<ll.subs.size();++j)
			{
				SubLine& sl = ll.subs[j];
				sl.hl = (hl==ii);
				DrawSub( surface, sl, 0, y );
				y+=normal.Height(); ii+=1;
			}
		}
	}

	/*
	if( (hl!=-1) && ((mx-menu_x)>(form(310))) )
	{
		lines[hl].slo = true;
		DrawSl( lines[hl].subs, hl );
	}
	*/

	SDL_Surface* vidsurf = SDL_GetVideoSurface();
	SDL_Rect dr = { menu_x,menu_y, surface->w, y };
	SDL_Rect sr = { 0,0, surface->w, y };

	SDL_BlitSurface(surface,&sr,vidsurf,&dr);

}

bool C35::Menu::ParseClick(int mx,int my,MenuChoise& mc)
{
	if( (!surface) || (mx < menu_x) || (my < menu_y) || (mx > (menu_x+surface->w)) || (my > (menu_y+surface->h)) )
	{
		mc.kind = MenuChoise::JustClose;
		return true;
	}

	int dx = mx - menu_x ;
	int dy = my - menu_y ;

	int i,n = lines.size();
	int ii=0;
	int yy = 0;
	int h = normal.Height();
	for(i=0;i<n;++i)
	{
		Line& ll = lines[i];
		// 1 - do we have the line?
		if( (dy>yy) && (dy<(yy+h)) )
		{
			// parse click on ordinary line

			if( dx > form(315) ) // the pullout btn
			{
				if( ll.slk != Line::sl_none )
					ll.slk = (ll.slk==Line::sl_d) ? Line::sl_r : Line::sl_d ;
				return false;
			}

			if( ll.kind == Line::lk_town )
			{
				if(!ll.own) { mc.kind = MenuChoise::JustClose; return true; }

				/**/ if( dx < form(210+64) ) mc.kind = MenuChoise::ZoomCity;
				else if( dx < form(210+96) ) mc.kind = MenuChoise::RallyPoint;
				else return false; // just missed it :P

				mc.city = ll.city;
				return true;
			}
			else if( ll.kind == Line::lk_unit )
			{

				if( dx < form(210) ) // the actual line
				{
					if( ll.subs.empty() || !ll.subs[0].own )
					{
						mc.kind = MenuChoise::JustClose;
					} else {
						mc.kind = MenuChoise::ActivateUnits;
						mc.units.clear();
						mc.units.push_back( ll.subs[0].u );
					}
					return true;
				}

				if(!ll.own) { mc.kind = MenuChoise::JustClose; return true; }

				/**/ if( dx < form(210+32) ) mc.kind = MenuChoise::SleepUnits;
				else if( dx < form(210+64) ) mc.kind = MenuChoise::GotoUnits;
				else if( dx < form(210+96) ) mc.kind = MenuChoise::ActivateUnits;
				else return false; // just missed it :P

				mc.units.clear();
				for( SubLine& sl : ll.subs )
					mc.units.push_back(sl.u);

				return true;
			}
			// some other kind, dc
		}
		yy += h;

		// 2 - parse subs
		if( ll.slk == Line::sl_d )
		{
			int n2=ll.subs.size();
			for(int j=0;j<n2;++j)
			{
				SubLine& sl = ll.subs[j];
				// 2.1 - do we have the line?
				if( (dy>yy) && (dy<(yy+h)) )
				{
					// parse click on sub line
					mc.kind = MenuChoise::ActivateUnits;
					mc.units.clear();
					mc.units.push_back(sl.u);
					return true;
				}
				yy += h;
			}
		}
	}
	// this is a click below
	mc.kind = MenuChoise::JustClose;
	return true;
}

namespace
{
	SDL_Surface* bg = 0;
	SDL_Surface* uis = 0;

}

const char* rank_names[] = {
	"Conscript",
	"Private", "Sergant", "Liutenent", "Captain",
	"Brigadier General", "Major General", "Liutenent General", "General",
	"General Marshalk"
};

void C35::UnitInfo::MakeFrom(Unit* u)
{
	if(!bg) bg = SDL_LoadBMP("gfx/menu/uib.bmp");
	if(!uis) uis = MakeSurface(bg->w,bg->h,bg->format);

	SDL_SetColorKey(uis,SDL_SRCCOLORKEY,SDL_MapRGB(uis->format,255,0,255));

	SDL_BlitSurface( bg,0,uis,0 );

	if(!u) return;

	UnitType& ut = UnitType::Lookup(u->type);
	AnimReflection r = ut.GetRefl("idle",270,u->owner);
	r.Overlay( uis, 55,95 );
	bool notype = ( ut.basetype == UnitType::None );

	ff->Print( uis, ut.name.c_str(), 165, 20, true );

	if(!notype) PrintText( uis, rank_names[u->stats.lev+1], 165, 45, true );

	int xx = 60;
	ut.GetIcon(u->owner).Overlay( uis, xx+=32,95 );
	if(!notype)
	{
		UC hue = GameEngine::PAK()->color;
		rank_insig[u->stats.lev+1].Instance(hue);
		rank_insig[u->stats.lev+1].Refl(hue).Overlay( uis, xx+=32,95 );
	}
	unit_type[ut.kind].Refl(menu_hue).Overlay( uis, xx+=32,95 );
	if(!notype) for( auto pp : u->perks )
	{
		perks[pp].Refl(menu_hue).Overlay( uis, xx+=32,95 );
	}

}

void C35::UnitInfo::Display(int mx,int my)
{
	SDL_Surface* vs = SDL_GetVideoSurface();
	if(!uis) return;
	SDL_Rect r = { vs->w-uis->w, vs->h-uis->h };
	SDL_BlitSurface( uis, 0, vs, &r );
}
	
