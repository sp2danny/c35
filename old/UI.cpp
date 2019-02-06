
#include "stdafx.h"

// --------------------------------------------

#include "UI.h"

#include "Common.h"
#include "Board.h"
#include "City.h"
//#include "Menu.h"

C35::UI::UI(Board* b,Player* p)
{
	board=b;
	active=0;
	player=p;
}

C35::UI::~UI()
{
}

C35::Unit* C35::UI::ActiveUnit()
{
	return active;
}

namespace
{
	struct UH
	{
		C35::Unit* u;
		C35::Hexagon* h;
		UH(C35::Unit* unit,C35::Hexagon* hex) : u(unit),h(hex) {}
	};
	typedef std::vector<UH> UHList;
	UHList MakeUnitList(C35::Board* board,C35::Player* p)
	{
		UHList units;

		int w = board->Width();
		int h = board->Height();
		for(int y=0;y<h;++y) for(int x=0;x<w;++x)
		{
			C35::Hexagon& hex = board->Get(x,y);
			for( C35::Unit* u : hex.units )
			{
				if( u->owner == p )
					units.push_back( UH(u,&hex) );
			}
		}
		return units;
	}
	void FindF(int& f,UHList& ul,C35::Unit* active)
	{
		int i,n=ul.size();
		for(i=0;i<n;++i)
		{
			if( active == ul[i].u )
			{
				f=i;
				break;
			}
		}
	}
	void RecVd( C35::Hexagon* h, int r, C35::Dir6 gd)
	{
		if(!h) return;
		h->Obscure(2);
		if(r<=0) return;
		RecVd(h->neighbours[   gd ],r-1,gd);
		RecVd(h->neighbours[CW(gd)],r-1,gd);
	}
	void RecV( C35::Hexagon* h, int r)
	{
		if(!h) return;
		h->Obscure(2);
		if(r<=0) return;
		for(C35::Dir6 d=C35::d6_beg;d!=C35::d6_end;++d)
			RecVd(h->neighbours[d],r-1,d);
	}

	void RecCd( C35::Hexagon* h, C35::Player* p, int r, C35::Dir6 gd)
	{
		if(!h) return;
		h->Owner(p);
		if(r<=0) return;
		RecCd(h->neighbours[   gd ],p,r-1,gd);
		RecCd(h->neighbours[CW(gd)],p,r-1,gd);
	}
	void RecC( C35::Hexagon* h, C35::Player* p, int r)
	{
		if(!h) return;
		h->Owner(p);
		if(r<=0) return;
		for(C35::Dir6 d=C35::d6_beg;d!=C35::d6_end;++d)
			RecCd(h->neighbours[d],p,r-1,d);
	}

}

C35::Hexagon* C35::UI::OnHex()
{
	if(!active) return 0;
	return &board->Get( active->x, active->y );
}

C35::Unit* C35::UI::NextUnit()
{
	UHList units = MakeUnitList(board,player);
	if(units.empty())
	{
		Activate(0,0);
		return active;
	}
	if(!active)
	{
		Activate(units.front().u,0);
		return active;
	}

	int f=-1, n=units.size();
	FindF(f,units,active);
	f = (f+n-1)%n;
	
	Activate(units[f].u,0);
	return active;

}

C35::Unit* C35::UI::PrevUnit()
{
	UHList units = MakeUnitList(board,player);
	if(units.empty())
	{
		Activate(0,0);
		return active;
	}
	if(!active)
	{
		Activate(units.front().u,0);
		return active;
	}

	int f=+1, n=units.size();
	FindF(f,units,active);
	f = (f+n-1)%n;
	
	Activate(units[f].u,0);
	return active;
}

void C35::UI::Activate(Unit* u,Hexagon* h)
{
	if( u && (u->owner != player)) return;
	if(active)
		active->currently = false;
	active = u;
	if(active)
		active->currently = true;
	//UnitInfo::MakeFrom(u);
}

void C35::UI::ReObsc()
{
	int i,n=board->HexCnt();
	for(i=1;i<=n;++i)
	{
		Hexagon& h=board->Get(i);
		h.Owner(0);
		if(h.Obscure()==2)
			h.Obscure(1);
	}

	for(i=1;i<=n;++i)
	{
		Hexagon& h=board->Get(i);
		City* c=h.Town();
		if(c && (c->owner==player) )
		{
			int infl = c->Influence();
			RecV(&h,infl+1);
			RecC(&h,c->owner,infl);
		} 
		else if(!h.units.empty())
		{
			if(h.units[0]->owner==player)
			{
				int i = 1;
				if( (h.tile==hill) || (h.tile==mount) ) i=2;
				RecV(&h,i);
			}
		}
	}

	while(true)
	{
		bool found = false;
		for(i=1;i<=n;++i)
		{
			Hexagon& h=board->Get(i);
			if(h.Owner()==0)
			{
				for(int j=0;j<3;++j)
				{
					Hexagon* h1 = h.neighbours[j];
					if(!h1) continue;
					if(!h1->Owner()) continue;
					Hexagon* h2 = h.neighbours[j+3];
					if(!h2) continue;
					if( h1->Owner() != h2->Owner() ) continue;
					found = true;
					h.Owner ( h1->Owner() );
					break;
				}
			}
		}
		if(!found) break;
	}
}

void C35::UI::EnterFight()
{
}

void C35::UI::ReadyTurn()
{
	ReObsc();
	int i,n=board->HexCnt();
	for(i=1;i<=n;++i)
	{
		Hexagon& h=board->Get(i);
		City* c=h.Town();
		if(c) c->Tick();
		if( h.Goody() == 2 )
		{
			if( (rand() % 100) == 0 )
			{
				Player* barb = & board->GetPlayer(0);
				Unit* u = UnitType::MakeConscriptFromType((int)UnitId::barbarian,barb);
				u->owner = barb;
				h.AddUnit(u);
			}
		}
		for( auto u : h.units )
			u->ClearForTurn();
	}

}

void C35::UI::FinishTurn()
{
	UHList units = MakeUnitList(board,player);
	for(UH& uh : units)
	{
		if( uh.u->stats.move )
			uh.u->ExecuteOrder(board);
	}
}

bool C35::UI::AllDone()
{
	UHList units = MakeUnitList(board,player);
	for(UH& uh : units)
	{
		if(uh.u->WantsOrder()) return false;
	}
	return true;
}

C35::Unit* C35::UI::NextNotDone()
{
	if(active && active->WantsOrder()) return active;
	UHList units = MakeUnitList(board,player);
	for(UH& uh : units)
	{
		if(uh.u->WantsOrder()) return uh.u;
	}
	return 0;
}