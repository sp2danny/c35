
#include "stdafx.h"

// --------------------------------------------

#include "GUI.h"

#include "Font.h"
#include "Unit.h"
#include "Hexagon.h"
#include "Orders.h"
#include "City.h"
#include "Menu.h"
#include "Anim.h"
//#include "Tech.h"
#include "AssetRepository.h"
#include "GameEngine.h"
#include "MiniMap.h"
#include "FrameForwards.h"
#include "UnitAction.h"
#include "CityView.h"

using namespace C35;
using namespace std;

namespace C35
{
	const int SZ = 60;
	const int YSZ = 47;

	float offs_x=0.0f, offs_y=0.0f;
}

namespace
{

	bool show_grid = true;

	Font font_w, font_b;
	TTF_Font* ttf_font;

	void ShadowText( SDL_Surface* s, string text, int x, int y, bool c = false, bool r = false )
	{
		(r?font_w:font_b).Print(s,text.c_str(),x-1,y-1,c);
		(r?font_b:font_w).Print(s,text.c_str(),x+1,y+1,c);
	}

	[[maybe_unused]] char message[256] = {0};

	[[maybe_unused]] bool show_infl = false;

	[[maybe_unused]] bool in_road_to = false;

	[[maybe_unused]] bool in_rally = false;
	[[maybe_unused]] City* rally_city;

	[[maybe_unused]] bool in_city_view = false;
	[[maybe_unused]] City* view_city;

	[[maybe_unused]] Player* human = 0;

	[[maybe_unused]] bool in_tech_view = false;

	SDL_Surface* ld_t(const char* name)
	{
		static char buf[256];
		sprintf(buf,"gfx/%s.bmp",name);
		SDL_Surface* srf  = SDL_LoadBMP(buf);
		assert(srf);
		Uint32 key = SDL_MapRGB(srf->format,255,0,255);
		SDL_SetColorKey(srf,SDL_SRCCOLORKEY,key);
		return srf;
	}

	SDL_Surface* ld_nck(char* name)
	{
		static char buf[256];
		sprintf(buf,"gfx/%s.bmp",name);
		SDL_Surface* srf  = SDL_LoadBMP(buf);
		assert(srf);
		return srf;
	}

	int mouse_x;
	int mouse_y;

	bool has_lock_pos=false;
	int lp_x, lp_y;

	bool have_stack = false;
	vector<Unit*> stack;

	struct FloatText
	{
		float fx,fy;
		string text;
		float dx,dy;
		float lifeleft;
		SDL_Surface* tt;
		bool active;
	};

	std::list<FloatText> float_texts;

	void MakeFloatText( int x, int y, string text, int pixmove, float life, int r, int g, int b)
	//void MakeFloatText( int , int , string , int , float , int , int , int )
	{
		//ft.tt = nullptr;
		///*
		float_texts.emplace_back();
		FloatText& ft = float_texts.back();
		ft.fx = x; ft.fy = y;
		ft.dx = 0; ft.dy = pixmove*-0.001f/life;
		SDL_Color c= {(UC)r,(UC)g,(UC)b};
		ft.tt = TTF_RenderText_Solid(ttf_font, text.c_str(), c );
		ft.lifeleft=life;
		ft.active=true;
		//*/
	}

	void UpdateFT(int ms)
	{
		auto iter = float_texts.begin();
		while (iter != float_texts.end())
		{
			auto& ft = *iter;
			if (!ft.active) {
				iter = float_texts.erase(iter);
				continue;
			}
			ft.fx += ft.dx*ms;
			ft.fy += ft.dy*ms;
			ft.lifeleft -= ms/1000.0f;
			if(ft.lifeleft<0)
			{
				if (ft.tt)
					SDL_FreeSurface( ft.tt );
				ft.active=false;
			}
			++iter;
		}
	}

	void DisplayFT()
	{

		for( FloatText& ft : float_texts )
		{
			if(!ft.active) continue;
			SDL_Rect rr = { (short)(int(ft.fx) - offs_x), (short)(int(ft.fy) - offs_y), (unsigned short)(ft.tt->w), (unsigned short)(ft.tt->h) };
			SDL_BlitSurface(ft.tt,0,SDL_GetVideoSurface(),&rr);
		}
	}

	void grid_from_mouse(int& gx, int& gy)
	{
		[[maybe_unused]] const int MARG = 0;
		float base = (mouse_y+offs_y) ;
		if(base<0) base -= YSZ;
		gy = base / YSZ ;

		if(gy%2)
			base = (mouse_x+offs_x) - (SZ);
		else
			base = (mouse_x+offs_x) - (SZ/2);
		if( base<0 ) base -= SZ;
		gx = base / SZ;
	}

	bool inside( int x, int y )
	{
		int ay,ax;
		grid_from_mouse(ax,ay);
		//if(ax>=brd->Width()) ax-=brd->Width();
		return (ax==x) && (ay==y);
	}

	void ToggleRoad()
	{
		int gx, gy;
		grid_from_mouse(gx, gy);
		Board* brd = & GameEngine::Game();
		if( brd->Inside(gx,gy) )
		{
			Hexagon& h = brd->Get(gx,gy);
			h.mask ^= road;
		}
	}

	void SpawnWarrior()
	{
		int gx, gy;
		grid_from_mouse(gx, gy);
		Board* brd = & GameEngine::Game();
		if( brd->Inside(gx,gy) )
		{
			Hexagon& h = brd->Get(gx,gy);
			int uid = UnitType::ByName("Warrior").front();
			Unit* w = UnitType::MakeNewFromType(uid,GameEngine::CurrentlyPlaying());
			w->SetAnim("attack", rand()%360 );
			h.AddUnit(w);
		}
	}

	void SpawnHorseman()
	{
		int gx, gy;
		grid_from_mouse(gx, gy);
		Board* brd = & GameEngine::Game();
		if( brd->Inside(gx,gy) )
		{
			Hexagon& h = brd->Get(gx,gy);
			int uid = UnitType::ByName("Horseman").front();
			Unit* w = UnitType::MakeNewFromType(uid,GameEngine::CurrentlyPlaying());
			w->SetAnim("idle", rand()%360 );
			h.AddUnit(w);

		}
	}

	void CheatVisuals()
	{
		PlayerBoard* brd = GameEngine::CurrentlyPlaying()->board;
		int i,n=brd->Size();
		for(i=1;i<=n;++i)
		{
			PlayerHex& ph = brd->Get(i);
			if(ph.obscure==0) ph.obscure=1;
		}
		GameEngine::Game().PreparePlayerBoard(GameEngine::CurrentlyPlaying());
		MiniMap::Update(*brd);
	}

	enum class OverlayId {
		none,
		camp, goody,
		irri, mine,
		forest, jungle,
		town1, town2, town3
	};


}

C35::GUI::GUI()
{
	RawFont rf;
	rf.LoadFile("gfx/arial.sdlf");
	SDL_PixelFormat* pf = SDL_GetVideoSurface()->format;
	rf.Instansiate( font_w, SDL_MapRGB( pf, 255,255,255 ) );
	rf.Instansiate( font_b, SDL_MapRGB( pf, 0,0,0 ) );
	done=false;

	Menu::InitMenu(font_b);

	AD& tiles = Asset<AD>("gfx/tiles.ad");

	for( AD::BAD& bd : tiles.bad )
	{
		for( CIS& cis : bd.anim )
		{
			cis.hx=0;
			cis.hy=0;
		}
	}

	tiles.Instance(0);

	AD& roads = Asset<AD>("gfx/roads.ad");
	roads.Instance(0);

	CIS& r_ur = Asset<CIS>("gfx/river_ur.bmp");
	r_ur.hx=r_ur.hy=0;
	r_ur.Instance(0);

	CIS& r_r = Asset<CIS>("gfx/river_r.bmp");
	r_r.hx=r_r.hy=0;
	r_r.Instance(0);

	CIS& r_dr = Asset<CIS>("gfx/river_dr.bmp");
	r_dr.hx=r_dr.hy=0;
	r_dr.Instance(0);

	AD& grass_spread = Asset<AD>("gfx/grass_spread.ad");
	grass_spread.Instance(0);

	AD& plains_spread = Asset<AD>("gfx/plains_spread.ad");
	plains_spread.Instance(0);

	CIS& grid = Asset<CIS>("gfx/grid.bmp");
	grid.Instance(0);

	CIS& cb = Asset<CIS>("gfx/circ_b.bmp");
	cb.Instance(0);

	CIS& cg = Asset<CIS>("gfx/circ_g.bmp");
	cg.Instance(0);

	CIS& o_o = Asset<CIS>("gfx/obscure_o.bmp"); o_o.Instance(0);
	CIS& o_e = Asset<CIS>("gfx/obscure_e.bmp"); o_e.Instance(0);

	AD& ol = Asset<AD>("gfx/overlay.ad");
	for( AD::BAD& bd : ol.bad )
	{
		for( CIS& cis : bd.anim )
		{
			if( (bd.dir < 5) || (bd.dir > 6) )
			{
				cis.Skip3();
				cis.Hot(Pos(8,8));
			}
		}
	}
	ol.Instance(0);

	CIS& infl = Asset<CIS>("gfx/influence.cis");
	AD& border =  Asset<AD>("gfx/border.ad");

	for( UC hue : GameEngine::Game().AllPlayerColors() )
	{
		infl.Instance(hue);
		border.Instance(hue);
	}
	
	TTF_Init();
	ttf_font = TTF_OpenFont("gfx/arial.ttf", 12);
	
	MiniMap::Init();
}

void C35::GUI::HaveAllNow()
{
	CIS& infl = Asset<CIS>("gfx/influence.cis");
	AD& border =  Asset<AD>("gfx/border.ad");

	for( UC hue : GameEngine::Game().AllPlayerColors() )
	{
		infl.Instance(hue);
		border.Instance(hue);
	}
}

C35::GUI::~GUI()
{
}

static int dx=0;
static int dy=0;

void C35::GUI::Update(int ms)
{
	delta=ms;

	offs_x += (dx*ms/2.5f);
	offs_y += (dy*ms/2.5f);

	Pos p;
	if( GameEngine::ActionPosition(p) )
	{
		Board* brd = & GameEngine::Game();

		int WW = Frame::screen->w;
		int HH = Frame::screen->h;


		Hexagon& h = brd->Get(p.x,p.y);
		offs_x = h.px - WW/2;
		offs_y = h.py - HH/2;
	}

	MiniMap::Update( * GameEngine::PAK()->board );
	
	UpdateFT(ms);
}

const int MARG = 0;
int OffsX() { int ox = (int)offs_x; ox = ox & ~1; return ox; }
int OffsY() { int oy = (int)offs_y; oy = oy & ~1; return oy; }
#define MakeMaster SDL_Rect MasterRect = { (short)(hex.px-OffsX()), (short)(hex.py-OffsY()), 128,128 }

static int redo_count = 0;
static int frame_count = 0;
static int frame_time = 0;
static int fps = 0;

bool want_out = false;

void C35::GUI::Display()
{
	PlayerBoard* brd = GameEngine::PAK()->board;

	int x,y;
	int w = brd->Width();
	int h = brd->Height();

	int WW = Frame::screen->w;
	int HH = Frame::screen->h;

	struct P { int x,y,c; P():x(0),y(0),c(0){} P(int xx,int yy,int cost):x(xx),y(yy),c(cost){} };
	std::vector<P> pth;

	Unit* u = 0;
	// calculate path
	if(has_lock_pos&&u)
	{
		UnitType& ut = UnitType::Lookup(u->type);
		int MM = 3000 * ut.move;
		grid_from_mouse(x,y);
		if(x>=brd->Width()) x-=brd->Width();
		if( brd->Inside(x,y) )
		{
			if(brd->Get(x,y).route_cost)
			{
				pth.push_back(P(x,y,brd->Get(x,y).route_cost/MM));
				int counter = 0;
				while( (x!=lp_x) || (y!=lp_y) )
				{
					PlayerHex& at = brd->Get(x,y);
					int idx_frm = at.route_from;
					if(!idx_frm)
					{
						break;
					}
					PlayerHex& frm = brd->Get( idx_frm );
					x = frm.x;
					y = frm.y;
					pth.push_back(P(x,y,frm.route_cost/MM));
					++counter;
					if(counter>250)
					{
						break; // bug break
					}
				}
			}
		}
	}

	AD& tiles = Asset<AD>("gfx/tiles.ad");
	AD& grass_spread = Asset<AD>("gfx/grass_spread.ad");
	AD& plains_spread = Asset<AD>("gfx/plains_spread.ad");

	// 1 - tiles
	for(y=0;y<h;++y) for(x=0;x<w;++x)
	{
		PlayerHex& hex = brd->Get(x,y);
		MakeMaster;

		if( (MasterRect.x+64) < 0 )
			MasterRect.x += w*SZ;
		if( MasterRect.x > WW ) continue;
		if( (MasterRect.y+64) < 0 ) continue;
		if( MasterRect.y > HH ) continue;

		if( hex.obscure == 0 ) continue;

		AnimReflection ref = tiles.Refl(hex.tile,0);
		SDL_Rect dr=MasterRect;
		ref.Overlay(screen, dr.x,dr.y);

		// 1.2 - spread
		switch(hex.tile)
		{
		case desert:
		case plains:
		case floodp:
		case tundra:
			for(Dir6 d = d6_beg; d != d6_end; ++d )
			{
				PlayerHex* h2 = hex.Neighbours(d);
				if(!h2) continue;
				if( (h2->tile==grass) || (h2->tile==bonus) || (h2->tile==hill) || (h2->tile==mount) )
				{
					ref = grass_spread.Refl(d,0);
					ref.Overlay(screen, dr.x,dr.y);
				}
				else if( h2->tile==coast ) 
				{
					ref = plains_spread.Refl(d,0);
					ref.Overlay(screen, dr.x,dr.y);
				}
				else if( (h2->tile==plains) && (hex.tile!=plains) )
				{
					ref = plains_spread.Refl(d,0);
					ref.Overlay(screen, dr.x,dr.y);
				}
			}
			break;
		case coast:
			for(Dir6 d = d6_beg; d != d6_end; ++d )
			{
				PlayerHex* h2 = hex.Neighbours(d);
				if(!h2) continue;
				if( h2->IsRealLand() )
				{
					ref = plains_spread.Refl(d,0);
					ref.Overlay(screen, dr.x,dr.y);
				}
			}
			break;
		case grass:
		case bonus:
		case hill:
		case mount:
			for(Dir6 d = d6_beg; d != d6_end; ++d )
			{
				PlayerHex* h2 = hex.Neighbours(d);
				if(!h2) continue;
				if( h2->tile==coast ) 
				{
					ref = plains_spread.Refl(d,0);
					ref.Overlay(screen, dr.x,dr.y);
				}
			}
			break;
		}
	}

	CIS& infl = Asset<CIS>("gfx/influence.cis");
	AD& border = Asset<AD>("gfx/border.ad");

	// 2 - border 
	for(y=0;y<h;++y) for(x=0;x<w;++x)
	{
		PlayerHex& hex = brd->Get(x,y);
		MakeMaster;

		if( (MasterRect.x+64) < 0 )
			MasterRect.x += w*SZ;
		if( MasterRect.x > WW ) continue;
		if( (MasterRect.y+64) < 0 ) continue;
		if( MasterRect.y > HH ) continue;

		if( hex.obscure == 0 ) continue;

		if( ! hex.owner ) continue;

		for(Dir6 d=d6_beg;d!=d6_end;++d)
		{
			PlayerHex* n = hex.Neighbours(d);
			if( (!n) || (n->owner!=hex.owner) )
			{
				short deg = Degree[d];
				UC hue = hex.owner->color;
				AnimReflection refl = border.Refl(deg,hue);
				refl.Overlay( screen, MasterRect.x, MasterRect.y );
			}
		}
	}


	[[maybe_unused]] AD& roads = Asset<AD>("gfx/roads.ad");
	[[maybe_unused]] AD& overlay = Asset<AD>("gfx/overlay.ad");

	// 3 - overlays
	for(y=0;y<h;++y) for(x=0;x<w;++x)
	{
		PlayerHex& hex = brd->Get(x,y);
		MakeMaster;

		if( (MasterRect.x+64) < 0 )
			MasterRect.x += w*SZ;
		if( MasterRect.x > WW ) continue;
		if( (MasterRect.y+64) < 0 ) continue;
		if( MasterRect.y > HH ) continue;

		if( hex.obscure == 0 ) continue;

		if( hex.mask & river_upright )
		{
			CIS& cis = Asset<CIS>("gfx/river_ur.bmp");
			cis.Instance(0);
			cis.Refl(0).Overlay( screen, MasterRect.x, MasterRect.y );
		}
		if( hex.mask & river_right )
		{
			CIS& cis = Asset<CIS>("gfx/river_r.bmp");
			cis.Instance(0);
			cis.Refl(0).Overlay( screen, MasterRect.x, MasterRect.y );
		}
		if( hex.mask & river_downright )
		{
			CIS& cis = Asset<CIS>("gfx/river_dr.bmp");
			cis.Instance(0);
			cis.Refl(0).Overlay( screen, MasterRect.x, MasterRect.y );
		}

		if( hex.mask & forest )
			overlay.Refl((int)OverlayId::forest,0).Overlay( screen, MasterRect.x, MasterRect.y );
		else if( hex.mask & jungle )
			overlay.Refl((int)OverlayId::jungle,0).Overlay( screen, MasterRect.x, MasterRect.y );
		else if( hex.mask & irri )
			overlay.Refl((int)OverlayId::irri,0).Overlay( screen, MasterRect.x, MasterRect.y );


		if( hex.Roaded() )
		{
			int num = 0;
			for(Dir6 d = d6_beg; d != d6_end; ++d )
			{
				PlayerHex* h2 = hex.Neighbours(d);
				if(!h2) continue;
				if(h2->Roaded()) num |= ( 1<<d );
			}
			AD& roads = Asset<AD>("gfx/roads.ad");
			roads.Instance(0);
			AnimReflection refl = roads.Refl( num, 0 );
			refl.Overlay( screen, MasterRect.x, MasterRect.y );
		}

		if( hex.mask & mine )
			overlay.Refl((int)OverlayId::mine,0).Overlay( screen, MasterRect.x, MasterRect.y );

		if( hex.goody == 1 )
			overlay.Refl((int)OverlayId::goody,0).Overlay( screen, MasterRect.x, MasterRect.y );
		else if( hex.goody == 2 )
			overlay.Refl((int)OverlayId::camp,0).Overlay( screen, MasterRect.x, MasterRect.y );


		if( hex.city )
		{
			if( hex.city->size >= 12 )
				overlay.Refl((int)OverlayId::town3,0).Overlay( screen, MasterRect.x, MasterRect.y );
			else if( hex.city->size >= 6 )
				overlay.Refl((int)OverlayId::town2,0).Overlay( screen, MasterRect.x, MasterRect.y );
			else
				overlay.Refl((int)OverlayId::town1,0).Overlay( screen, MasterRect.x, MasterRect.y );

		}

	}

	CIS& grid = Asset<CIS>("gfx/grid.bmp");
	CIS& o_o  = Asset<CIS>("gfx/obscure_o.bmp");
	CIS& o_e  = Asset<CIS>("gfx/obscure_e.bmp");

	// 3 - grid, obscure & infl
	for(y=0;y<h;++y) for(x=0;x<w;++x)
	{
		PlayerHex& hex = brd->Get(x,y);
		MakeMaster;

		if( (MasterRect.x+64) < 0 )
			MasterRect.x += w*SZ;
		if( MasterRect.x > WW ) continue;
		if( (MasterRect.y+64) < 0 ) continue;
		if( MasterRect.y > HH ) continue;

		Pos p = Pos(MasterRect.x, MasterRect.y);

		if(show_grid)
			grid.Refl(0).Overlay( screen, p );

		if( show_infl && hex.owner && hex.obscure )
		{
			infl.Refl(hex.owner->color).Overlay( screen, p );
		}
		else if( hex.obscure == 1 )
		{
			[[maybe_unused]] SDL_Rect dr=MasterRect;
			((y%2)?o_e:o_o).Refl(0).Overlay(screen,p);
		}

	}

	CIS& active_marker = Asset<CIS>("gfx/circ_g.bmp");

	// 4 - unit
	Unit* au = GameEngine::ActiveUnit();
	AnimReflection ar;
	Pos arp;
	bool locked = GameEngine::InAnimation(ar,arp);
	for(y=0;y<h;++y) for(x=0;x<w;++x)
	{
		PlayerHex& hex = brd->Get(x,y);
		if( hex.units.empty() ) continue;

		MakeMaster;

		if( (MasterRect.x+64) < 0 )
			MasterRect.x += w*SZ;
		if( MasterRect.x > WW ) continue;
		if( (MasterRect.y+64) < 0 ) continue;
		if( MasterRect.y > HH ) continue;

		Unit* u = hex.DisplayUnit();
		assert(u);

		if( (u==au) && !locked )
			active_marker.Refl(0).Overlay( screen, MasterRect.x, MasterRect.y );

		u->Update(delta);
		u->Display(MasterRect.x+SZ/2,MasterRect.y+YSZ);
	}

	// 4b - animation
	if(locked)
	{
		arp.x -= offs_x - SZ/2;
		arp.y -= offs_y - YSZ;
		ar.Overlay(screen,arp);
	}

	int gx,gy;
	grid_from_mouse(gx,gy);
	if( brd->Inside(gx,gy) )
	{
		PlayerHex& hex = brd->Get(gx,gy);
		MakeMaster;

		CIS& cis = Asset<CIS>("gfx/circ_b.bmp");
		cis.Refl(0).Overlay( screen, MasterRect.x, MasterRect.y );
	}

	static float fps = 30.0f;
	fps = ( 30*fps + 1000.0f/delta ) / 31.0f ;
	static char buff[256];
	sprintf(buff,"%0.1f",fps);

	font_b.Print( screen, buff, screen->w - 74, 16 );
	font_w.Print( screen, buff, screen->w - 75, 15 );

	MiniMap::Display( screen, 0,0 );

	UnitInfo::Display(mouse_x,mouse_y);

	DisplayFT();

	if(Menu::Active()) Menu::Display(mouse_x,mouse_y);
}

void C35::GUI::ParseInput(SDL_Event& e)
{
	if(GameEngine::InAnimation()) return;

	auto m1 = [] (Dir6 d)
	{
		Unit* u = GameEngine::ActiveUnit();
		if( u && u->AcceptsOrder() )
		{
			u->Attack(d);
		}
	};

	if( e.type == SDL_KEYDOWN )
	{
		if( e.key.keysym.sym == SDLK_ESCAPE )
			done=true;

		if( e.key.keysym.sym == SDLK_F1 )
		{
			Push( GetTSD() );
			return;
		}

		if( e.key.keysym.sym == SDLK_o )
			show_infl = !show_infl;

		if( e.key.keysym.sym == SDLK_g )
			show_grid = !show_grid;

		if( e.key.keysym.sym == SDLK_v )
			CheatVisuals();

		if( e.key.keysym.sym == SDLK_w )
			if( GameEngine::ActiveUnit() )
			{
				GameEngine::ActiveUnit()->Won();
				GameEngine::WasModifyed();
			}

		if( e.key.keysym.sym == SDLK_u )
			GameEngine::WasModifyed();

		if( e.key.keysym.sym == SDLK_n )
			GameEngine::NextUnit();

		if( e.key.keysym.sym == SDLK_RETURN )
			GameEngine::TurnDone();

		if( e.key.keysym.sym == SDLK_SPACE )
		{
			Unit* u = GameEngine::ActiveUnit();
			if(u)
			{
				PlayerHex& ph = u->owner->board->Get( u->x, u->y );
				offs_x = ph.px - screen->w/2;
				offs_y = ph.py - screen->h/2;
			} else {
				GameEngine::TurnDone();
			}
		}

		if( e.key.keysym.sym == SDLK_KP9 ) m1( d6_upright   );
		if( e.key.keysym.sym == SDLK_KP7 ) m1( d6_upleft    );
		if( e.key.keysym.sym == SDLK_KP4 ) m1( d6_left      );
		if( e.key.keysym.sym == SDLK_KP1 ) m1( d6_downleft  );
		if( e.key.keysym.sym == SDLK_KP3 ) m1( d6_downright );
		if( e.key.keysym.sym == SDLK_KP6 ) m1( d6_right     );

		if( e.key.keysym.sym == SDLK_UP    ) dy -= +1;
		if( e.key.keysym.sym == SDLK_DOWN  ) dy += +1;
		if( e.key.keysym.sym == SDLK_LEFT  ) dx -= +1;
		if( e.key.keysym.sym == SDLK_RIGHT ) dx += +1;
		
		if (e.key.keysym.sym == SDLK_f)
		{
			MakeFloatText(100, 100, "hello", 3, 1.5f, 255,255,0);
		}

		bool processed = UnitAction::ProcessKeypress(&e);

		if( ! processed )
		{
			if( e.key.keysym.sym == SDLK_r ) ToggleRoad();
			//if( e.key.keysym.sym == SDLK_w ) SpawnWarrior();
			if( e.key.keysym.sym == SDLK_h ) SpawnHorseman();
		}

	}

	if( e.type == SDL_KEYUP )
	{
		if( e.key.keysym.sym == SDLK_UP    ) dy -= -1;
		if( e.key.keysym.sym == SDLK_DOWN  ) dy += -1;
		if( e.key.keysym.sym == SDLK_LEFT  ) dx -= -1;
		if( e.key.keysym.sym == SDLK_RIGHT ) dx += -1;
	}

	static bool lmd = false;

	if( e.type == SDL_MOUSEMOTION )
	{
		mouse_x = e.motion.x;
		mouse_y = e.motion.y;
		if(lmd) MiniMap::ParseClick( mouse_x, mouse_y, offs_x, offs_y );
	}

	// -------------------------------------

	if( GameEngine::CP() != GameEngine::PAK() ) return;


	if( e.type == SDL_MOUSEBUTTONDOWN )
	{
		if( e.button.button == SDL_BUTTON_LEFT )
		{
			lmd=true;
			bool caught = MiniMap::ParseClick( mouse_x, mouse_y, offs_x, offs_y );
			if( (!caught) && Menu::Active() )
			{
				MenuChoise mc;
				caught = Menu::ParseClick( mouse_x, mouse_y, mc );
				if( caught )
				{
					if( mc.kind == MenuChoise::JustClose)
						Menu::Done();
					else if( mc.kind == MenuChoise::ZoomCity )
					{
						CityView::ViewCity(mc.city);
						Menu::Done();
					}
					else if( mc.kind == MenuChoise::ActivateUnits )
					{
						if( ! mc.units.empty() )
						{
							for( Unit* u : mc.units )
							{
								UnitAction::DoWake(u);
							}
							GameEngine::ActivateUnit( mc.units.back() );
						}
						Menu::Done();
					}
					else if( mc.kind == MenuChoise::GotoUnits )
					{
					}
					else if( mc.kind == MenuChoise::SleepUnits )
					{
						for( Unit* u : mc.units )
						{
							UnitAction::DoFortify(u);
						}
						Menu::Done();
					}
				}
			}

			if(!caught)
			{
				Pos p;
				grid_from_mouse(p.x,p.y);
				Unit* u = GameEngine::Game(). /*ActivePlayer()->board->*/ DisplayUnit(p);
				if(u) GameEngine::ActivateUnit(u);
			}
		}
		if( e.button.button == SDL_BUTTON_RIGHT )
		{
			Pos p;
			grid_from_mouse(p.x,p.y);
			PlayerHex& h = GameEngine::PAK()->board->Get(p);
			Menu::MakeFrom(h,mouse_x,mouse_y);
		}
	}
	if( e.type == SDL_MOUSEBUTTONUP )
	{
		if( e.button.button == SDL_BUTTON_LEFT )
			lmd = false;
	}

}

bool C35::GUI::Done()
{
	return done;
}

bool C35::GUI::Unload()
{
	return false;
}

