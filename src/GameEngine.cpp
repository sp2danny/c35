
#include "stdafx.h"

// --------------------------------------------

#include "GameEngine.h"

#include "GUI.h"
#include "City.h"
#include "Tech.h"
#include "Resource.h"
#include "Menu.h"
#include "UnitAction.h"
#include "FrameSystem.h"
#include "CityView.h"
#include "AI.h"
#include "LockedAnimation.h"

using namespace std;

C35::Board C35::GameEngine::board;
C35::Frame* C35::GameEngine::gui;
C35::Unit* C35::GameEngine::active;
int C35::GameEngine::ap;
int C35::GameEngine::hp;
deque<C35::Pos> C35::GameEngine::ap_stack;

void C35::GameEngine::Init()
{

	gui = new C35::GUI();
	UnitCreator();
	ImprovementCreator();
	Resource::Maker();
	TechType::Creator();
	CityView::Init();
}

namespace
{
	struct P { int x,y; };
	vector<P> psp;
	//C35::Player* human;
	/*
	bool in_anim = false;
	C35::AnimReflection la_ar;
	C35::Pos la_pos;
	*/
	C35::LockedAnimation* locked=0;
}

void C35::GameEngine::LoadGame(const char*)
{
}

void C35::GameEngine::PrepareTurn(bool ppb)
{
	Player* p = CP();
	for( Hexagon& h : board.map )
	{
		for(Unit* u : h.units)
		{
			if( u->owner != p ) continue;
			u->stats.move = u->stats.maxmove*3000;
		}
	}
	if(ppb) board.PreparePlayerBoard(p);
}

void ScoutSome(C35::PlayerHex* hex, int r)
{
	using namespace C35;
	if(r<=0) return;
	hex->obscure=1;
	for(Dir6 d=d6_beg; d!=d6_end;++d)
	{
		PlayerHex* n = hex->Neighbours(d);
		if(!n) continue;
		if( (r>5) || ( (rand()%100) > 60)  ) ScoutSome(n,r-1);
	}
}

void ScoutSome(C35::Player* pl,C35::Pos p, int r)
{
	C35::PlayerHex* hex = & pl->board->Get(p);
	ScoutSome(hex,r);
}

void C35::GameEngine::Setup(bool fromstart)
{
	int i,n=psp.size();

	int x,w = board.Width();
	int y,h = board.Height();
	for(y=0;y<h;++y) for(x=0;x<w;++x)
	{
		Hexagon& h = board.Get(x,y);
		if( (h.tile == desert) && h.River() )
			h.tile = floodp;
	}

	int hue = rand()%256;
	int hs = 256 / n;

	[[maybe_unused]] Player* human = board.AddHumanControlled(hue);

	for(i=1;i<n;++i)
	{
		hue = (hue+hs)%256;
		[[maybe_unused]] Player* ai = board.AddComputerControlled(hue);
		//Player* ai = board.AddHumanControlled(hue);
	}

	Frame::AddActive( new AI );

	Player* barbs = & board.GetPlayer(0);

	UnitType::HaveAllNow();
	GUI::HaveAllNow();

	if(fromstart)
	{

		for(i=0;i<n;++i)
		{
			Player* p = &board.GetPlayer(i+1) ;
			Hexagon& hex = board.Get( psp[i].x , psp[i].y );
			{
				Unit* war;
				war = UnitType::MakeNewFromType((int)UnitId::warrior,p);
				hex.AddUnit(war);
			}
			{
				Unit* set;
				set = UnitType::MakeNewFromType((int)UnitId::settler,p);
				hex.AddUnit(set);
			}
			{
				Unit* wrk;
				wrk = UnitType::MakeNewFromType((int)UnitId::worker,p);
				hex.AddUnit(wrk);
			}
			ScoutSome(p,Pos(psp[i].x,psp[i].y),7);
		}

		for(y=0;y<h;++y) for(x=0;x<w;++x)
		{
			Hexagon& h = board.Get(x,y);
			if( h.Goody() == 2 )
			{
				Unit* barb;
				barb = UnitType::MakeConscriptFromType((int)UnitId::barbarian,barbs);
				barb->owner=barbs;
				h.AddUnit(barb);
				barb->SetAnim("idle",270);
			}
		}
	}

	hp = ap = 1;
	NextUnit();
	if(active)
	{
		Pos p ( active->x, active->y );
		ap_stack.push_back( p );
	}

	PrepareTurn();
}

void C35::GameEngine::LoadMap(const char* mapname)
{

	ifstream ifs(mapname,ios_base::in | ios_base::binary );

	int i,n;
	ReadBinary(ifs,n);
	psp.clear();
	for(i=0;i<n;++i)
	{
		P p;
		ReadBinary(ifs,p.x);
		ReadBinary(ifs,p.y);
		psp.push_back(p);
	}

	board.FromStream(ifs);

	Setup();
}

void C35::GameEngine::RandomMap()
{
}

void C35::GameEngine::SaveGame(const char*)
{
}

C35::Board& C35::GameEngine::Game()
{
	return board;
}

C35::Frame* C35::GameEngine::GUI()
{
	return gui;
}

// -------------------------------------------

void C35::GameEngine::WasModifyed()
{
	Game().PreparePlayerBoard(PAK());
	ActivateUnit(active);
}

void C35::GameEngine::ActivateUnit(Unit* u)
{
	if( u && (u->owner != CP())) return;
	if(active)
		active->currently = false;
	active = u;
	if(active)
		active->currently = true;
	UnitInfo::MakeFrom(u);
	UnitAction::MakeFrom(u,Frame::screen->w/2,Frame::screen->h-40);
}

C35::Unit* C35::GameEngine::NextUnit()
{
	vector<Unit*> ulst;
	for( Hexagon& hex : board.map )
	{
		for( Unit* u : hex.units )
		{
			if( u->owner != CP() ) break;
			if( u->WantsOrder())
			{
				ulst.push_back(u);
			}
		}
	}

	if(ulst.empty())
	{
		ActivateUnit(0);
		return active;
	}

	if(active && (active->owner == CP()) )
	{
		vector<Unit*>::iterator itr = find( ulst.begin(), ulst.end(), active );
		if(itr!=ulst.end()) ++itr;
		if(itr==ulst.end()) itr=ulst.begin();
		ActivateUnit(*itr);
		return active;
	}

	ActivateUnit(ulst.front());
	return active;
}

C35::Unit* C35::GameEngine::ActiveUnit()
{
	return active;
}

C35::Player* C35::GameEngine::CurrentlyPlaying()
{
	return & board.GetPlayer(ap);
}

C35::Player* C35::GameEngine::PresentAtKeyboard()
{
	return & board.GetPlayer(hp);
}

void C35::GameEngine::AddUnitOrder( Unit* u, Orders o )
{
	CP()->board->AddUnitOrder(u,o);
}

void C35::GameEngine::AddStackOrder( vector<Unit*> uv , Orders o )
{
	CP()->board->AddStackOrder(uv,o);
}

void C35::GameEngine::TurnDone()
{
	ap++;
	if( ap >= (int)board.players.size() )
	{
		// barbs ai here

		ap = 1;

		int i,n = board.HexCnt();

		for(i=1;i<=n;++i)
		{
			Hexagon& h = board.Get(i);
			if(h.city) h.city->Tick();
		}

		PrepareTurn(false);

		for(i=1;i<=n;++i)
		{
			Hexagon& h = board.Get(i);
			for( Unit* u : h.units )
			{
				if( u->stats.move )
					u->ExecuteOrder(&board);
			}
		}
		board.UpdateInflu();

		board.PreparePlayerBoard(CP());

	} else {

		PrepareTurn();
	}

	if( CP()->pak )
	{
		hp=ap;

		NextUnit();
		Unit* u = ActiveUnit();
		if(u) ap_stack.push_back( Pos(u->x,u->y) );
		else if( City* c = CP()->capital ) ap_stack.push_back( Pos(c->x, c->y) );
	}

}

bool C35::GameEngine::ActionPosition( Pos& p )
{
	if(ap_stack.empty()) return false;
	p = ap_stack.front();
	ap_stack.pop_front();
	return true;
}

void C35::GameEngine::Update(int ms)
{
	if(!locked) return;

	locked->Update(ms);
	if( locked->Done() )
	{
		if(locked->Unload()) delete locked;
		locked = 0;
	}
}

void C35::GameEngine::SetLA(LockedAnimation& la)
{
	locked = &la;
}

bool C35::GameEngine::InAnimation( AnimReflection& ar, Pos& worldCoord )
{
	if(locked)
	{
		ar = locked->Refl();
		worldCoord = locked->Position();
		return true;
	}
	return false;
}

bool C35::GameEngine::InAnimation()
{
	return locked;
}



