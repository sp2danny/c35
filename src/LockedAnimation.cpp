
#include "stdafx.h"

// --------------------------------------------

#include "LockedAnimation.h"
#include "Unit.h"

// --------------------------------------------

// **************
// *** MoveLA ***
// **************

// --------------------------------------------

namespace C35
{
	class MoveLA : public LockedAnimation
	{
	public:
		MoveLA(Unit*,Dir6);
		virtual void Update(int);
		virtual bool Done();
		virtual AnimReflection Refl();
		virtual Pos Position();
		virtual bool Unload();
	private:
		AnimReflection ar;
		float dx,dy;
		float x,y;
		float x2,y2;
		int tick;
		short dir;
		enum { MX = 650 };
		Unit* u;
	};
}

short GetDir(C35::Dir6 d)
{
	switch(d)
	{
		case C35::d6_upright    : return 60;
		case C35::d6_right      : return 0;
		case C35::d6_downright  : return 300;
		case C35::d6_downleft   : return 240;
		case C35::d6_left       : return 180;
		case C35::d6_upleft     : return 120;
		default: break;
	}
	return 0;
}

C35::MoveLA::MoveLA(Unit* unit,Dir6 d)
{
	u = unit;
	PlayerBoard* pb = u->owner->board;
	PlayerHex& ph1 = pb->Get(u->x,u->y);
	x = ph1.px;
	y = ph1.py;

	PlayerHex* ph2 = ph1.Neighbours(d);
	assert(ph2);
	x2 = ph2->px;
	y2 = ph2->py;

	dx = (x2 - x) / MX;
	dy = (y2 - y) / MX;

	tick = 0;

	dir = GetDir(d);
	UnitType& ut = UnitType::Lookup( u->type );
	ar = ut.GetRefl( "walk", dir, u->owner );
	u->visible = false;
}

bool C35::MoveLA::Done()
{
	if(tick > MX)
	{
		u->visible = true;
		u->SetAnim("idle",dir);
		return true;
	} else {
		return false;
	}
}

bool C35::MoveLA::Unload()
{
	return true;
}

void C35::MoveLA::Update(int ms)
{
	ar.Update(ms);
	tick += ms;
	if( tick >= MX )
	{
		x=x2; y=y2;
	} else {
		x += ms*dx;
		y += ms*dy;
	}
}

C35::AnimReflection C35::MoveLA::Refl()
{
	return ar;
}

C35::Pos C35::MoveLA::Position()
{
	return Pos( (int)x, (int)y );
}

C35::LockedAnimation* C35::NewMovementAnim(Unit* u,Dir6 d)
{
	return new MoveLA(u,d);
}


// --------------------------------------------

// **************
// *** MoveLA ***
// **************

// --------------------------------------------

namespace C35
{
	class FightLA : public LockedAnimation
	{
	public:
		FightLA(Unit*,Dir6);
		virtual void Update(int);
		virtual bool Done();
		virtual AnimReflection Refl();
		virtual Pos Position();
		virtual bool Unload();
	private:
		AnimReflection ar1;
		//AnimReflection ar2;
		int x,y;
		int tick;
		short dir;
		enum { MX = 650 };
		Unit* u1;
		Unit* u2;
		bool done;
	};
}

C35::FightLA::FightLA(Unit* unit,Dir6 d)
{
	u1 = unit;
	PlayerBoard* pb = u1->owner->board;
	PlayerHex& ph1 = pb->Get(u1->x,u1->y);

	x = ph1.px;
	y = ph1.py;

	done = false;
	tick = 0;

	dir = GetDir(d);
	UnitType& ut1 = UnitType::Lookup( u1->type );
	ar1 = ut1.GetRefl( "attack", dir, u1->owner );
	u1->visible = false;

	PlayerHex* ph2 = ph1.Neighbours(d);

	u2 = ph2->DisplayUnit();
	assert(u2);
	UnitType& ut2 = UnitType::Lookup( u2->type );
	short dir2 = (dir+180)%360;
	u2->SetAnim("attack", dir2);
	//ar2 = ut2.GetRefl( "attack", dir2, u2->owner );
	//u2->visible = false;

}

bool C35::FightLA::Done()
{
	if(done)
	{
		u1->visible = true;
		u1->SetAnim("idle",dir);
		return true;
	} else {
		return false;
	}
}

bool C35::FightLA::Unload()
{
	return true;
}

void C35::FightLA::Update(int ms)
{
	ar1.Update(ms);
	tick += ms;
}

C35::AnimReflection C35::FightLA::Refl()
{
	return ar1;
}

C35::Pos C35::FightLA::Position()
{
	return Pos(x,y);
}


C35::LockedAnimation* C35::NewFightAnim(Unit* u,Dir6 d)
{
	return new FightLA(u,d);
}