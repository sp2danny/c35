
#include "stdafx.h"

// --------------------------------------------

#include "UnitAction.h"

#include "Unit.h"
#include "City.h"

namespace C35 { namespace UnitAction
{
	Unit* made_from = 0;
	int tx, ty;
}}

void C35::UnitAction::DoSettle  (Unit* u) 
{
	Board& b = GameEngine::Game();
	Hexagon& h = b.Get( u->x, u->y );
	//h.RemoveUnit(u);
	City* city = new City();
	city->owner = u->owner;
	city->x = u->x;
	city->y = u->y;
	city->name = "Noname";
	city->building_improvement = city->building_unit = false;
	city->build_id = 0;
	city->buildings.clear();
	city->culture = 0;
	city->foodbox = 0;
	city->shieldbox = 0;
	city->size = 1;
	city->rallypoint = false;
	h.city = city;	

	if( ! u->owner->capital )
	{
		u->owner->capital = city;
	}

	u->KillUnit();

	GameEngine::Game().UpdateInflu();

	GameEngine::Game().PreparePlayerBoard(GameEngine::PAK());
	GameEngine::Game().PreparePlayerBoard(GameEngine::CP());

	GameEngine::NextUnit();
}

void C35::UnitAction::DoRoad    (Unit* u)
{
	Orders* o = new Orders();
	o->action = Orders::road;
	o->queue = u->orders;
	u->orders = o;
	u->ExecuteOrder( & GameEngine::Game() );
}

void C35::UnitAction::DoMine    (Unit* u)
{
	Orders* o = new Orders();
	o->action = Orders::mine;
	o->queue = u->orders;
	u->orders = o;
	u->ExecuteOrder( & GameEngine::Game() );
}

void C35::UnitAction::DoIrri    (Unit* u)
{
	Orders* o = new Orders();
	o->action = Orders::irri;
	o->queue = u->orders;
	u->orders = o;
	u->ExecuteOrder( & GameEngine::Game() );
}


void C35::UnitAction::DoRail    (Unit*) {}
void C35::UnitAction::DoClear   (Unit*) {}
void C35::UnitAction::GoTo      (Unit*,Pos) {}

void C35::UnitAction::Go1       (Unit* u,Dir6 d)
{
	GameEngine::SetLA( * NewMovementAnim( u, d ) );

}

void C35::UnitAction::DoFortify (Unit* u)
{
	Orders* o = new Orders();
	o->action = Orders::fortify;
	o->queue = u->orders;
	u->orders = o;
	u->ExecuteOrder( & GameEngine::Game() );
	u->SetAnim("fortify",270);
	GameEngine::NextUnit();
}

void C35::UnitAction::DoWake    (Unit* u)
{
	if( u && u->orders )
	{
		u->AbortOrder();
		u->SetAnim("idle",270);
	}
}
void C35::UnitAction::DoWaste   (Unit*) {}

void C35::UnitAction::DoPillage (Unit* u)
{
	/*
	Orders* o = new Orders();
	o->action = Orders::pi;
	o->queue = u->orders;
	u->orders = o;
	u->ExecuteOrder( & GameEngine::Game() );
	*/
}

void C35::UnitAction::DoBombard (Unit*,Pos) {}


void C35::UnitAction::MakeFrom( Unit* u, int x, int y )
{
	if(u==made_from) return;
	made_from=u;
}

void C35::UnitAction::DrawIcons( int mx, int my )
{
}

bool C35::UnitAction::ProcessClick( int mx, int my )
{
	return false;
}

bool C35::UnitAction::ProcessKeypress( SDL_Event* e )
{
	if(!made_from) return false;
	auto k = e->key.keysym.sym;
	UnitType& ut = UnitType::Lookup(made_from->type);
	if(ut.capa.settle)
		if(k==SDLK_b) { DoSettle(made_from); return true; }
	if(ut.capa.worker)
	{
		if(k==SDLK_r) { DoRoad(made_from); return true; }
		if(k==SDLK_m) { DoMine(made_from); return true; }
		if(k==SDLK_i) { DoIrri(made_from); return true; }
	}
	if(ut.capa.pillage)
		if(k==SDLK_p) { DoPillage(made_from); return true; }

	if(k==SDLK_f) { DoFortify(made_from); return true; }

	return false;
}


