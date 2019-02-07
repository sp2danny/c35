
#include "stdafx.h"

// --------------------------------------------

#include "Board.h"

#include "Hexagon.h"
#include "City.h"

#define ever ;;

// *******************
// *** PlayerBoard ***
// *******************

C35::PlayerHex& C35::PlayerBoard::Get(Pos p)
{
	assert( (p.x>=0) && (p.y>=0) && (p.x<width) && (p.y<height) );
	auto idx = p.x + p.y*width;
	assert( (idx>=0) && (idx<(int)map.size()) );
	return map[idx];
}

C35::PlayerHex& C35::PlayerBoard::Get(int idx)
{
	assert( (idx>=1) && (idx<=(int)map.size()) );
	return map[idx-1];

	for(ever)
	{
		break;
	}
}

bool C35::PlayerBoard::Inside(Pos p) const
{
	return
		p.x >= 0     &&
		p.x < width  &&
		p.y >= 0     &&
		p.y < height  ;
}

C35::Unit* C35::PlayerBoard::DisplayUnit( Pos p )
{
	if(!Inside(p)) return 0;
	PlayerHex& hex = Get(p);
	int max = -1;
	Unit* du =0;
	for(Unit* u : hex.units )
	{
		if( u->currently ) return u;
		int val = u->stats.hp * u->stats.def ;
		if(val>max) { max=val; du = u; }
	}
	return du;
}


int C35::PlayerBoard::Width()  const { return width; }
int C35::PlayerBoard::Height() const { return height; }
int C35::PlayerBoard::Size()   const { return width*height; }

void C35::PlayerBoard::PathFrom(Pos p,const Movement& mm)
{
	PathFromTarget(p,mm,Pos(-1,-1));
}

void C35::PlayerBoard::PathFromTarget(Pos p,const Movement& mm,Pos t)
{
	for( PlayerHex& hex : map )
	{
		hex.route_from = 0;
		hex.route_cost = 0;
	}

	Get(p).route_from = Get(p).id;

	struct ITEM
	{
		PlayerHex* hex;
		int from;
		int cost;
	};

	vector<ITEM> list;
	int alt = 0;
	bool havetarget = false;
	while(!havetarget)
	{
		++alt;
		list.clear();
		for( PlayerHex& hex : map )
		{
			if( ! hex.route_from ) continue;

			for( Dir6 d = d6_beg; d < d6_end; ++d )
			{
				PlayerHex* to = hex.Neighbours(d);
				if(!to) continue;
				if( to->route_from ) continue;
				if( ! to->obscure ) continue;

				bool dst_lnd = to->IsLand();
				bool dst_sea = to->IsSea();
				bool ok = false;
				if( dst_lnd && mm.land ) ok = true;
				if( dst_sea && mm.sea  ) ok = true;
				if(!ok) continue;

				ITEM item;
				int tc = hex.travelcost(d);
				if(tc) {
					item.cost = hex.route_cost + tc;
					item.from = hex.id;
					item.hex = to;
					list.push_back(item);
				}
			}
		}
		if( list.empty() ) break;
		int low = list[0].cost;
		int i,n = list.size();
		for(i=1;i<n;++i)
			if( list[i].cost < low )
				low = list[i].cost;


		for( i=((alt%2)?0:n-1) ; (alt%2)?(i<n):(i>=0) ; i += (alt%2)?+1:-1 )
		{
			if( list[i].cost == low )
			{
				list[i].hex->route_cost = list[i].cost;
				list[i].hex->route_from = list[i].from;
				if( (list[i].hex->x == t.x) && (list[i].hex->y == t.y) )
					havetarget=true;
			}
		}
	}

}

void C35::PlayerBoard::AddUnitOrder( Unit* , Orders )
{
}

void C35::PlayerBoard::AddStackOrder( vector<Unit*> us, Orders o)
{
	for(Unit* u : us) AddUnitOrder(u,o);
}

C35::PlayerBoard::PlayerBoard(Board* master)
{
	width  = master->Width();
	height = master->Height();
	map.clear();
	map.reserve(width*height);
	int i,n=master->HexCnt();
	for(i=1;i<=n;++i)
	{
		map.emplace_back();
		PlayerHex& ph = map.back();
		HexCore& hc = ph;
		hc = master->Get(i);
		ph.route_cost=0;
		ph.route_from=0;
		ph.units.clear();
	}
	do_neighbours(true);
}

void C35::PlayerBoard::add_n( C35::PlayerHex& h, Dir6 d, int x,int y, bool wrap )
{
	h.neighbours[d] = 0;
	if( y<0 || y>= height ) return;
	if( x<0 )
	{
		if(wrap) x+=width; else return;
	}
	if( x>= width )
	{
		if(wrap) x-=width; else return;
	}
	h.neighbours[d] = &Get(Pos(x,y));
}

extern const int SZ;
extern const int YSZ;

void C35::PlayerBoard::do_neighbours(bool wrap)
{
	for(int y=0;y<height;++y) for(int x=0;x<width;++x)
	{
		PlayerHex& h = Get(Pos(x,y));
		h.x=x; h.y=y;
		if( y%2 )
		{
			// odd
			add_n( h, d6_upleft    ,  x  , y-1 , wrap );
			add_n( h, d6_upright   , x+1 , y-1 , wrap );
			add_n( h, d6_downleft  ,  x  , y+1 , wrap );
			add_n( h, d6_downright , x+1 , y+1 , wrap );
			h.px = SZ+SZ*x;
			h.py = YSZ * y;
		} else {
			// even
			add_n( h, d6_upleft    , x-1 , y-1 , wrap );
			add_n( h, d6_upright   ,  x  , y-1 , wrap );
			add_n( h, d6_downleft  , x-1 , y+1 , wrap );
			add_n( h, d6_downright ,  x  , y+1 , wrap );
			h.px = (SZ/2) + SZ*x;
			h.py = YSZ * y;
		}
		add_n( h, d6_right , x+1 , y , wrap );
		add_n( h, d6_left  , x-1 , y , wrap );
	}
}

// *************
// *** Board ***
// *************

std::vector<C35::UC> C35::Board::AllPlayerColors()
{
	std::vector<UC> apc;
	for(Player* p : players)
		apc.push_back( (UC) p->color );
	return apc;
}

C35::Board::Board()
{
	barb_controller.capital=0;
	barb_controller.name="A Barbarian Chiefdom";
	barb_controller.color=0;
	barb_controller.isbarb=true;
	barb_controller.id=0;
	barb_controller.gold=0;
	barb_controller.game = this;
	
	players.push_back(&barb_controller);
}

void C35::Board::add_n( C35::Hexagon& h, Dir6 d, int x,int y, bool wrap )
{
	h.neighbours[d] = 0;
	if( y<0 || y>= height ) return;
	if( x<0 )
	{
		if(wrap)
			x+=width;
		else
			return;
	}
	if( x>= width )
	{
		if(wrap)
			x-=width;
		else
			return;
	}
	h.neighbours[d] = &Get(x,y);
}

void C35::Board::MakeEmptyMap(int w, int h, bool wrap)
{
	width=w;
	height=h;
	int sz = w*h;
	map.reserve(sz);
	map.resize(sz);

	for(int i=0;i<sz;++i)
	{
		map[i].id = i+1;
		map[i].mask = 0;
		map[i].Owner(0);
		map[i].tile = 0;
		map[i].route_cost = 0;
		map[i].route_from = 0;
		map[i].Obscure(0);
	}

	do_neighbours(wrap);
}

void C35::Board::do_neighbours(bool wrap)
{
	for(int y=0;y<height;++y)
	{
		for(int x=0;x<width;++x)
		{
			Hexagon& h = Get(x,y);
			h.x=x; h.y=y;
			//	enum Dir6 { d6_beg=0, d6_upright=d6_beg, d6_right, d6_downright, d6_downleft, d6_left, d6_upleft, d6_end };
			// bredd 110
			// höjd  125
			if( y%2 )
			{
				// odd
				add_n( h, d6_upleft    ,  x  , y-1 , wrap );
				add_n( h, d6_upright   , x+1 , y-1 , wrap );
				add_n( h, d6_downleft  ,  x  , y+1 , wrap );
				add_n( h, d6_downright , x+1 , y+1 , wrap );
				h.px = SZ+SZ*x;
				h.py = YSZ * y;
			} else {
				// even
				add_n( h, d6_upleft    , x-1 , y-1 , wrap );
				add_n( h, d6_upright   ,  x  , y-1 , wrap );
				add_n( h, d6_downleft  , x-1 , y+1 , wrap );
				add_n( h, d6_downright ,  x  , y+1 , wrap );
				h.px = (SZ/2) + SZ*x;
				h.py = YSZ * y;
			}
			add_n( h, d6_right , x+1 , y , wrap );
			add_n( h, d6_left  , x-1 , y , wrap );
		}
	}
}


bool C35::Board::Inside(Pos p) const
{
	return
		p.x >= 0     &&
		p.x < width  &&
		p.y >= 0     &&
		p.y < height  ;
}


C35::Hexagon& C35::Board::Get(Pos p)
{
	assert( p.x>=0 && p.x<width );
	assert( p.y>=0 && p.y<height );
	unsigned idx = p.y * width + p.x ;
	assert( idx < map.size() );
	return map[idx];
}

C35::Hexagon& C35::Board::Get(int id)
{
	assert( id > 0 );
	assert( id <= (int)map.size() );
	return map[id-1];
}

C35::Unit* C35::Board::DisplayUnit( Pos p )
{
	if(!Inside(p)) return 0;
	Hexagon& hex = Get(p);
	int max = -1;
	Unit* du =0;
	for(Unit* u : hex.units )
	{
		int val = u->stats.hp * u->stats.def ;
		if(val>max) { max=val; du = u; }
	}
	return du;
}

void C35::Board::PathFrom(Pos p,const Movement& mm)
{
	PathFromTarget(p,mm,Pos(-1,-1));
}

void C35::Board::PathFromTarget(Pos p,const Movement& mm,Pos t)
{
	for( Hexagon& hex : map )
	{
		hex.route_from = 0;
		hex.route_cost = 0;
	}

	Get(p).route_from = Get(p).id;

	struct ITEM
	{
		Hexagon* hex;
		int from;
		int cost;
	};

	vector<ITEM> list;
	int alt = 0;
	bool havetarget = false;
	while(!havetarget)
	{
		++alt;
		list.clear();
		for( Hexagon& hex : map )
		{
			if( ! hex.route_from ) continue;

			for( Dir6 d = d6_beg; d < d6_end; ++d )
			{
				Hexagon* to = hex.neighbours[d];
				if(!to) continue;
				if( to->route_from ) continue;
				if( ! to->Obscure() ) continue;

				bool dst_lnd = to->IsLand();
				bool dst_sea = to->IsSea();
				bool ok = false;
				if( dst_lnd && mm.land ) ok = true;
				if( dst_sea && mm.sea  ) ok = true;
				if(!ok) continue;

				ITEM item;
				int tc = hex.travelcost(d);
				if(tc) {
					item.cost = hex.route_cost + tc;
					item.from = hex.id;
					item.hex = to;
					list.push_back(item);
				}
			}
		}
		if( list.empty() ) break;
		int low = list[0].cost;
		int i,n = list.size();
		for(i=1;i<n;++i)
			if( list[i].cost < low )
				low = list[i].cost;


		for( i=((alt%2)?0:n-1) ; (alt%2)?(i<n):(i>=0) ; i += (alt%2)?+1:-1 )
		{
			if( list[i].cost == low )
			{
				list[i].hex->route_cost = list[i].cost;
				list[i].hex->route_from = list[i].from;
				if( (list[i].hex->x == t.x) && (list[i].hex->y == t.y) )
					havetarget=true;
			}
		}
	}

}

void C35::Board::ToStream(ostream& o) const
{
	WriteBinary(o,width);
	WriteBinary(o,height);

	assert( map.size() == (unsigned)(width*height) );

	for( auto h : map )
	{
		h.ToStream(o);
	}
}

void  C35::Board::AllToStream(ostream& o) const
{
	WriteBinary(o,width);
	WriteBinary(o,height);

	assert( map.size() == (unsigned)(width*height) );

	for( auto h : map )
	{
		h.AllToStream(o);
	}
}

void C35::Board::FromStream(istream& in)
{
	ReadBinary(in,width);
	ReadBinary(in,height);

	map.clear();
	map.reserve(width*height);

	int i,n=width*height;

	for(i=0;i<n;++i)
	{
		Hexagon h;
		h.FromStream(in);
		map.push_back(h);
	}

	do_neighbours(true);
}

void C35::Board::AllFromStream(istream& in)
{
	ReadBinary(in,width);
	ReadBinary(in,height);

	map.clear();
	map.reserve(width*height);

	int i,n=width*height;

	for(i=0;i<n;++i)
	{
		Hexagon h;
		h.AllFromStream(in);
		map.push_back(h);
	}

	do_neighbours(true);
}

namespace
{
	void RecVd( C35::PlayerHex* h, int r, C35::Dir6 gd)
	{
		if(!h) return;
		h->obscure=2;
		if(r<=0) return;
		RecVd(h->Neighbours(   gd ),r-1,gd);
		RecVd(h->Neighbours(CW(gd)),r-1,gd);
	}
	void RecV( C35::PlayerHex* h, int r)
	{
		if(!h) return;
		h->obscure=2;
		if(r<=0) return;
		for(C35::Dir6 d=C35::d6_beg;d!=C35::d6_end;++d)
			RecVd(h->Neighbours(d),r-1,d);
	}
	void RecI( C35::Hexagon* h, C35::Player* p, int r)
	{
		if(!h) return;
		h->owner=p;
		if(r<=0) return;
		for(C35::Dir6 d=C35::d6_beg;d!=C35::d6_end;++d)
			RecI(h->neighbours[d],p,r-1);
	}

}

void C35::Board::UpdateInflu()
{
	for( Hexagon& h : map )
	{
		if( ! h.city ) continue;
		int i = h.city->Influence();
		RecI(&h,h.city->owner,i);
	}
}

void C35::Board::PreparePlayerBoard(Player* p)
{
	// 1 - set obscurity;

	PlayerBoard* b = p->board;
	int i,n=b->Size();
	for(i=1;i<=n;++i)
	{
		PlayerHex& h=b->Get(i);
		if(h.obscure==2)
			h.obscure=1;
	}

	for(i=1;i<=n;++i)
	{
		PlayerHex& ph=b->Get(i);
		Hexagon&   rh=Get(i);
		City* c=rh.city;
		if(c && (c->owner==p) )
		{
			int infl = c->Influence();
			RecV(&ph,infl+1);
		} 
		else if(!rh.units.empty())
		{
			if(rh.units[0]->owner==p)
			{
				int i = 1;
				if( (rh.tile==hill) || (rh.tile==mount) ) i=2;
				RecV(&ph,i);
			}
		}
	}

	// 2 - copy stuff player will know

	for(i=1;i<=n;++i)
	{
		PlayerHex& ph=b->Get(i);
		Hexagon&   rh=Get(i);

		bool onno;
		int obs = ph.obscure;
		switch(obs)
		{
		case 0:
			onno=false;
			for( Dir6 d=d6_beg; d!=d6_end; ++d)
			{
				PlayerHex* n = ph.Neighbours(d);
				if( n && n->obscure )
					onno=true;
			}
			if(!onno)
			{
				ph.clr();
				ph.units.clear();
				break;
			}
		case 1:
			(HexCore&)ph = (HexCore)rh;
			ph.units.clear();
			break;
		case 2:
			(HexCore&)ph = (HexCore)rh;
			ph.units.assign( rh.units.begin(), rh.units.end() );
			break;
		}
		ph.obscure=obs;
	}
}

C35::Player* C35::Board::AddHumanControlled(UC hue)
{
	Player* p = Player::MakeNew();

	p->isbarb = false;
	p->color = hue;
	p->name = "Human Controlled";
	p->game = this;
	p->board = new PlayerBoard(this);
	p->pak = true;

	assert( (int)players.size() == p->id );

	players.push_back(p);

	return p;
}

C35::Player* C35::Board::AddComputerControlled(UC hue)
{
	Player* p = Player::MakeNew();

	p->isbarb = false;
	p->color = hue;
	p->name = "Computer Controlled";
	p->game = this;
	p->board = new PlayerBoard(this);
	p->pak = false;

	assert( (int)players.size() == p->id );

	players.push_back(p);

	return p;
}

C35::Player* C35::Board::AddRemoteControlled(UC )
{
	return 0; // NIY
}


