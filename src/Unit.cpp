
#include "stdafx.h"

// --------------------------------------------

#include "Unit.h"
#include "Orders.h"
#include "Board.h"
#include "City.h"
#include "GameEngine.h"
#include "Hexagon.h"
#include "UnitAction.h"

using namespace std;

namespace
{
bool is2p(int i)
{
	if (i <= 0)
		return false;
	if (i == 1)
		return true;
	if (i % 2)
		return false;
	return is2p(i / 2);
}
}  // namespace

// ************
// *** Unit ***
// ************

C35::AnimReflection C35::Unit::GetIcon()
{
	return UnitType::Lookup(type).GetIcon(owner);
}

void C35::Unit::ClearForTurn()
{
	stats.oneoffs.Clear();
	stats.move = stats.maxmove * 3000;
}

void C35::Unit::Won()
{
	if (stats.lev < 0)
	{
		stats.lev = 0;
		stats.hp = stats.maxhp = 3;
		return;
	}
	if (stats.xp >= 255)
	{
		stats.xp = 255;
		return;
	}
	++stats.xp;
	if (is2p(stats.xp))
	{
		UnitType& ut = UnitType::Lookup(type);
		stats.lev++;
		switch (stats.lev % 3)
		{
		case 0:  // main
			switch (ut.basetype)
			{
			case UnitType::Off:
				stats.off++;
				break;
			case UnitType::Def:
				stats.def++;
				break;
			case UnitType::Bmb:
				stats.bombard_str++;
				break;
			case UnitType::Trn:
				stats.maxmove++;
				break;
			default:
				break;
			}
			break;
		case 1:  // hitpoint
			stats.hp = ++stats.maxhp;
			break;
		case 2:  // perk
			if (ut.perk != UnitType::NoPerk)
				perks.push_back((UC)ut.perk);
			break;
		default:
			break;
		}
	}
}

int                     C35::Unit::nextId = 0;
std::vector<C35::Unit*> C35::Unit::units;

int C35::Unit::NewID()
{
	return ++nextId;
}

void C35::Unit::Register(int id, Unit* u)
{
	if (units.size() <= id)
	{
		units.resize(id + 1);
	}
	units[id] = u;
}

C35::Unit* C35::Unit::Lookup(int id)
{
	if (id < 0)
		return 0;
	if (id >= units.size())
		return 0;
	return units[id];
}

void C35::Unit::ClearAll()
{
	nextId = 0;
	units.clear();
}

void AddOrder(C35::Unit* u, C35::Orders* o)
{
	if (u->orders == nullptr)
	{
		u->orders = o;
	}
	else
	{
		C35::Orders* p = u->orders;
		while (p->queue)
		{
			p = p->queue;
		}
		assert(p && !p->queue);
		p->queue = o;
	}
}

C35::Unit* C35::Unit::FromStream(istream& is, Player* p)
{
	int type;
	ReadBinary(is, type);
	Unit* u = UnitType::MakeNewFromType(type, p);
	ReadBinary(is, u->stats.def);
	ReadBinary(is, u->stats.off);
	ReadBinary(is, u->stats.lev);
	ReadBinary(is, u->stats.hp);
	ReadBinary(is, u->stats.maxhp);
	ReadBinary(is, u->stats.xp);
	ReadBinary(is, u->stats.move);
	UC i, cnt;
	ReadBinary(is, cnt);
	for (i = 0; i < cnt; ++i)
	{
		Orders* o = new Orders();
		ReadBinary(is, o->action);
		ReadBinary(is, o->tx);
		ReadBinary(is, o->ty);
		ReadBinary(is, o->dir);
		o->queue = 0;
		AddOrder(u, o);
	}
	return u;
}

void C35::Unit::ToStream(ostream& os)
{
	WriteBinary(os, type);
	WriteBinary(os, stats.def);
	WriteBinary(os, stats.off);
	WriteBinary(os, stats.lev);
	WriteBinary(os, stats.hp);
	WriteBinary(os, stats.maxhp);
	WriteBinary(os, stats.xp);
	WriteBinary(os, stats.move);
	if (!orders)
		WriteBinary(os, (UC)0);
	else
	{
		Orders* o     = orders;
		int     count = 1;
		while (o->queue)
		{
			o = o->queue;
			++count;
		}
		WriteBinary(os, (UC)count);
		o = orders;
		while (o)
		{
			WriteBinary(os, o->action);
			WriteBinary(os, o->tx);
			WriteBinary(os, o->ty);
			WriteBinary(os, o->dir);
			o = o->queue;
		}
	}
}

C35::Unit* C35::UnitType::MakeNewFromType(int ut_idx, Player* p)
{
	Unit& u = *new Unit();

	for (int j = 0; j < 256; ++j)
		u.buffer[j] = j;

	u.id = Unit::NewID();
	Unit::Register(u.id, &u);
	u.type       = ut_idx;
	u.orders     = 0;
	u.owner      = 0;
	UnitType& ut = UnitType::Lookup(ut_idx);
	u.stats.xp = u.stats.lev = 0;
	u.stats.hp = u.stats.maxhp = 3;
	u.stats.off                = ut.start_off;
	u.stats.def                = ut.start_def;
	u.stats.bombard_str        = ut.start_bmb;
	u.stats.bombard_range      = ut.range;
	u.stats.move = u.stats.maxmove = ut.move;
	u.stats.oneoffs                = ut.capa;
	u.currently                    = false;
	u.owner                        = p;
	u.visible                      = true;
	u.perks.clear();
	if (p)
		u.ar.Set(ut.GetRefl("walk", 180, p));

	return &u;
}

C35::Unit* C35::UnitType::MakeConscriptFromType(int ut_idx, Player* p)
{
	Unit* u = MakeNewFromType(ut_idx, p);
	u->stats.hp--;
	u->stats.maxhp--;
	u->stats.lev--;
	return u;
}

void C35::Unit::KillUnit()
{
	if (GameEngine::ActiveUnit() == this)
		GameEngine::ActivateUnit(0);

	Hexagon& h = owner->game->Get(x, y);
	h.RemoveUnit(this);
	int myid = id;
	delete units[myid];
	units[myid] = 0;
}

bool C35::Unit::WantsOrder()
{
	if (orders)
		return false;
	if (stats.move == 0)
		return false;
	return true;
}

bool C35::Unit::AcceptsOrder()
{
	if (stats.move == 0)
		return false;
	if (orders)
	{
		if (orders->action == Orders::fortify)
		{
			delete orders;
			orders = 0;
			return true;
		}
		return false;
	}
	return true;
}

static short DirOff[] = {60, 0, 300, 240, 180, 120};

void C35::Unit::ExecuteOrder(Board* brd)
{
	if (!orders)
		return;
	UnitType& ut = UnitType::Lookup(type);

	switch (orders->action)
	{
	case Orders::move1:
		DoGo((Dir6)orders->dir);
		break;
	case Orders::move:
	{
		Hexagon&  h        = brd->Get(orders->tx, orders->ty);
		bool      ok       = false;
		UnitType& ut       = UnitType::Lookup(type);
		bool      landunit = ((ut.kind >= UnitType::Foot) && (ut.kind <= UnitType::Vehicle));
		bool      seaunit  = (ut.kind == UnitType::Sea);
		if (h.IsLand() && landunit)
			ok = true;
		if (h.IsSea() && seaunit)
			ok = true;
		if (!ok)
		{
			OrderDone();
			break;
		}
	}
		brd->PathFromTarget(Pos(x, y), MakeMovement(), Pos(orders->tx, orders->ty));
		if (brd->Get(orders->tx, orders->ty).route_cost == 0)
		{
			// cant get to target
			OrderDone();
		}
		else
		{
			while (stats.move && orders)
			{
				Hexagon& on      = brd->Get(x, y);
				int      current = on.id;
				Hexagon* h       = &brd->Get(orders->tx, orders->ty);
				while (h->route_from != current)
				{
					h = &brd->Get(h->route_from);
				}
				if (h->units.size() && (h->units[0]->owner != owner))
				{
					OrderDone(true);
					break;
				}
				on.RemoveUnit(this);
				h->AddUnit(this);
				stats.move -= (h->route_cost - on.route_cost);
				if (stats.move < 0)
					stats.move = 0;
				if ((h->x == orders->tx) && (h->y == orders->ty))
				{
					OrderDone();
					break;
				}
			}
		}
		break;
	case Orders::road:
	{
		SetAnim("road", 270);
		Hexagon& hex = brd->Get(x, y);
		hex.workaction_progress += 1;
		stats.move = 0;
		if (hex.workaction_progress >= (3 * hex.WorkMultiplyer()))
		{
			hex.AddRoad();
			// OrderDone(); hex stops all workers
			hex.workaction_progress = 0;
		}
	}
	break;
	case Orders::mine:
	{
		SetAnim("mine", 270);
		Hexagon& hex = brd->Get(x, y);
		hex.workaction_progress += 1;
		stats.move = 0;
		if (hex.workaction_progress >= (6 * hex.WorkMultiplyer()))
		{
			hex.AddMine();
			// OrderDone(); hex stops all workers
			hex.workaction_progress = 0;
		}
	}
	break;
	case Orders::irri:
	{
		SetAnim("irrigate", 270);
		Hexagon& hex = brd->Get(x, y);
		hex.workaction_progress += 1;
		stats.move = 0;
		if (hex.workaction_progress >= (4 * hex.WorkMultiplyer()))
		{
			hex.AddIrri();
			// OrderDone(); hex stops all workers
			hex.workaction_progress = 0;
		}
	}
	break;
	case Orders::bridge:
	{
		SetAnim("mine", 270);
		Hexagon& hex = brd->Get(x, y);
		hex.workaction_progress += 1;
		stats.move = 0;
		if (hex.workaction_progress >= (8 * hex.WorkMultiplyer()))
		{
			hex.AddBridge();
			// OrderDone(); hex stops all workers
			hex.workaction_progress = 0;
		}
	}
	break;
	case Orders::roadto:
	{
		Hexagon& hex = brd->Get(x, y);
		if (hex.Roaded())
		{
			if ((x == orders->tx) && (y == orders->ty))
			{
				// done!
				OrderDone();
			}
			else
			{
				brd->PathFromTarget(Pos(x, y), MakeMovement(), Pos(orders->tx, orders->ty));
				Hexagon* unit = &brd->Get(x, y);
				Hexagon* h    = &brd->Get(orders->tx, orders->ty);
				if (h->route_cost == 0)
				{
					// cant go there
					OrderDone();  // abort
				}
				else
				{
					while (true)
					{
						int rf = h->route_from;
						if (rf == unit->id)
							break;
						h = &brd->Get(rf);
					}
					Orders* o = new Orders;
					o->action = Orders::move;
					o->tx     = h->x;
					o->ty     = h->y;
					o->queue  = orders;
					orders    = o;
					ExecuteOrder(brd);
				}
			}
		}
		else
		{
			Orders* o = new Orders;
			o->action = Orders::road;
			o->tx     = x;
			o->ty     = y;
			o->queue  = orders;
			orders    = o;
			ExecuteOrder(brd);
		}
	}
	break;
	case Orders::explore:
	{
		brd->PathFrom(Pos(x, y), MakeMovement());
		int  tx, ty;
		bool found = false;
		int  cost;
		int  i, n = brd->HexCnt();
		for (i = 1; i <= n; ++i)
		{
			Hexagon& h = brd->Get(i);
			if (!h.route_cost)
				continue;
			if (!h.Obscure())
				continue;
			int samecount = 1;
			for (Dir6 d = d6_beg; d != d6_end; ++d)
			{
				Hexagon* n = h.neighbours[d];
				if (!n)
					continue;
				if (!n->Obscure())
				{
					if (!found)
					{
						found     = true;
						tx        = h.x;
						ty        = h.y;
						cost      = h.route_cost;
						samecount = 1;
					}
					else if (h.route_cost < cost)
					{
						tx        = h.x;
						ty        = h.y;
						cost      = h.route_cost;
						samecount = 1;
					}
					else if (h.route_cost == cost)
					{
						++samecount;
						if ((rand() % samecount) == 1)
						{
							tx   = h.x;
							ty   = h.y;
							cost = h.route_cost;
						}
					}

					break;
				}
			}
		}
		if (found)
		{
			Orders* o = new Orders;
			o->tx     = tx;
			o->ty     = ty;
			o->action = Orders::move;
			o->queue  = orders;
			orders    = o;
			ExecuteOrder(brd);
		}
		else
		{
			OrderDone();
		}
	}
	break;
	case Orders::autopilot:
		AutoPilot();
		break;
	case Orders::attack:
		SetAnim("attack", DirOff[orders->dir]);
		break;
	case Orders::fortify:
		break;
	default:
		OrderDone();
		break;
	}
	if (stats.move <= 0)
	{
		stats.move = 0;
		GameEngine::NextUnit();
	}
}

static void RecursiveConnect(C35::Hexagon* h)
{
	using namespace C35;
	h->route_cost = 1;
	for (Dir6 d = d6_beg; d != d6_end; ++d)
	{
		Hexagon* n = h->neighbours[d];
		if (!n)
			continue;
		if (n->route_cost)
			continue;
		if (n->Roaded())
			RecursiveConnect(n);
	}
}

void C35::Unit::AutoPilot()
{
	vector<City*> cities;
	City*         cap = owner->capital;
	if (!cap)
	{
		OrderDone();
		return;
	}
	Board* brd = owner->game;
	int    i, n = brd->HexCnt();
	for (i = 1; i <= n; ++i)
	{
		Hexagon& h   = brd->Get(i);
		h.route_cost = 0;
		if (!h.Town())
			continue;
		if (h.Town()->owner != owner)
			continue;
		cities.push_back(h.Town());
	}
	if (cities.empty())
	{
		OrderDone();
		return;
	}
	Hexagon& h = brd->Get(cap->x, cap->y);
	RecursiveConnect(&h);

	for (auto c : cities)
	{
		Hexagon& ch = brd->Get(c->x, c->y);
		if (!ch.route_cost)
		{
			// found a nonconnected city
			Orders* gt = new Orders;
			gt->action = Orders::move;
			gt->tx     = cap->x;
			gt->ty     = cap->y;
			Orders* rt = new Orders;
			rt->action = Orders::roadto;
			rt->tx     = c->x;
			rt->ty     = c->y;
			rt->queue  = orders;
			gt->queue  = rt;
			orders     = gt;
			ExecuteOrder(brd);
			return;
		}
	}

	// all where connected
	OrderDone();
}

void C35::Unit::AbortOrder()
{
	if (orders)
		OrderDone(true);
}

void C35::Unit::OrderDone(bool all)
{
	Orders* o = orders;
	orders    = o->queue;
	delete o;
	if (all)
	{
		if (orders)
			OrderDone(true);
	}
	else
	{
		if (stats.move > 0)
			ExecuteOrder(owner->game);
	}
	// SetAnim("idle",270);
}

C35::Movement C35::Unit::MakeMovement()
{
	Movement  m;
	UnitType& ut = UnitType::Lookup(type);
	m.land       = ((ut.kind >= UnitType::Foot) && (ut.kind <= UnitType::Vehicle));
	m.sea        = (ut.kind == UnitType::Sea);
	m.movement   = stats.maxmove;
	m.remaining  = stats.move;
	return m;
}

bool C35::Unit::CanCity()
{
	UnitType& ut = UnitType::Lookup(type);
	if (!ut.capa.settle)
		return false;
	Board*   brd = owner->game;
	Hexagon& hex = brd->Get(x, y);
	if (hex.Town())
		return false;
	if (hex.tile == mount)
		return false;
	if (hex.Owner() && (hex.Owner() != owner))
		return false;
	for (Dir6 d = d6_beg; d != d6_end; ++d)
	{
		Hexagon* n = hex.neighbours[d];
		if (n && n->Town())
			return false;
	}
	return true;
}

bool C35::Unit::CanGo(Dir6 d) const
{
	UnitType& ut = UnitType::Lookup(type);

	Hexagon& h1 = owner->game->Get(x, y);
	Hexagon* h2 = h1.neighbours[d];

	if (!h2)
		return false;

	if (ut.kind == UnitType::Air)
		return false;

	if (ut.kind == UnitType::Sea)
	{
		if (!h2->IsSea())
		{
			return false;  // CanUnload(h2->x,h2->y);
		}
		if (h2->Town())
			return (h2->Town()->owner == owner);
		if (h2->units.empty())
			return true;
		return h2->units[0]->owner == owner;
	}
	else
	{
		if (!h2->IsLand())
		{
			if (h2->units.empty())
				return false;
			if (h2->units[0]->owner != owner)
				return false;
			for (Unit* u : h2->units)
			{
				if (u->CanLoad(this))
					return true;
			}
			return false;
		}
		else
		{
			if ((!h2->IsRealLand()) && (!h1.IsRealLand()))
				return false;
			if (h2->units.empty())
				return true;
			return h2->units[0]->owner == owner;
		}
	}
}

void C35::Unit::DoGo(Dir6 d)
{
	Board*   brd = owner->game;
	Hexagon& on  = brd->Get(x, y);
	Hexagon* h   = on.neighbours[orders->dir];
	if (!h)
	{
		OrderDone();
		return;
	}

	if (!h->units.empty())
	{
		if (h->units[0]->owner != owner)
		{
			OrderDone();
			return;
		}
	}

	UnitType& ut = UnitType::Lookup(type);
	if (ut.kind == UnitType::Air)
	{
		OrderDone();
		return;
	}
	if (ut.kind == UnitType::Sea)
	{
		if (!h->IsSea())
		{
			OrderDone();
			return;
		}
		if (h->Town() && (h->Town()->owner != owner))
		{
			OrderDone();
			return;
		}

		SetAnim("walk", DirOff[orders->dir]);  // .FromIA( ut.ic.Get( owner->color, DirOff[orders->dir] ) );

		stats.move -= 3000;
		if (stats.move < 0)
			stats.move = 0;
		on.RemoveUnit(this);
		h->AddUnit(this);

		OrderDone();
	}
	else
	{  // land unit

		if (!h->IsLand())
		{
			if (h->units.empty())
			{
				OrderDone();
				return;
			}
			for (Unit* u : h->units)
			{
				if (u->CanLoad(this))
				{
					stats.move = 0;
					u->DoLoad(this);
					OrderDone();
					return;
				}
			}
			{
				OrderDone();
				return;
			}
		}
		if (h->Town() && (h->Town()->owner != owner))
		{
			OrderDone();
			return;
		}

		int cost = on.travelcost((Dir6)orders->dir);

		if (!cost)
		{
			OrderDone();
			return;
		}

		SetAnim("walk", DirOff[orders->dir]);  // .FromIA( ut.ic.Get( owner->color, DirOff[orders->dir] ) );

		stats.move -= cost;
		if (stats.move < 0)
			stats.move = 0;
		on.RemoveUnit(this);
		h->AddUnit(this);

		OrderDone();
	}
}

void C35::Unit::SetAnim(string name, int dir)
{
	UnitType& ut = UnitType::Lookup(type);
	ar           = ut.GetRefl(name, dir, owner);
}

void C35::Unit::Update(int ms)
{
	ar.Update((4 * ms) / 5);
}
void C35::Unit::Display(int x, int y)
{
	if (!visible)
		return;
	ar.Overlay(SDL_GetVideoSurface(), x, y);

	UnitType& ut     = UnitType::Lookup(type);
	bool      notype = (ut.basetype == UnitType::None);

	if (!notype)
	{
		x -= SZ / 2 - 3;
		y -= YSZ - 3;
		SDL_PixelFormat* pf = SDL_GetVideoSurface()->format;
		for (int i = stats.maxhp; i > 0; --i)
		{
			Uint32   col = (i > stats.hp) ? SDL_MapRGB(pf, 255, 0, 0) : SDL_MapRGB(pf, 0, 255, 0);
			SDL_Rect r   = {x, y, 6, 6};
			SDL_FillRect(SDL_GetVideoSurface(), &r, col);
			y += 8;
		}
	}
}

void C35::Unit::Attack(Dir6 d)
{
	// SetAnim("attack",DirOff[d]);
	Board*   brd = owner->game;
	Hexagon& h1  = brd->Get(x, y);
	Hexagon* h2  = h1.neighbours[d];
	if (!h2)
		return;
	Unit* u2 = h2->DisplayUnit();
	if ((!u2) || (u2->owner == owner))
	{
		Orders* o = new Orders(Orders::move1);
		o->dir    = d;
		orders    = o;
		UnitAction::Go1(this, d);
		ExecuteOrder(brd);
	}
	else
	{
		GameEngine::SetLA(*NewFightAnim(this, d));

		// ui->EnterFight();
	}
	GameEngine::Game().PreparePlayerBoard(GameEngine::PAK());
}

bool C35::Unit::CanLoad(const Unit* u) const
{
	UnitType& ut_other = UnitType::Lookup(u->type);
	switch (ut_other.kind)
	{
	case UnitType::Sea:
	case UnitType::Air:
		return false;
	default:
		break;
	}
	UnitType& ut_me = UnitType::Lookup(type);

	if ((loaded.size() + 1) > ut_me.load_capa)
		return false;

	return true;
}

void C35::Unit::DoLoad(Unit* u)
{
	Hexagon& hex = owner->game->Get(u->x, u->y);
	hex.RemoveUnit(u);
	loaded.push_back(u);
}

bool C35::Unit::CanUnload(int tx, int ty) const
{
	Hexagon& hex = owner->game->Get(tx, ty);
	if (!hex.IsLand())
		return false;
	if (loaded.empty())
		return false;
	if (hex.units.empty())
		return true;
	return hex.units[0]->owner == owner;
}

bool C35::Unit::CanUnload(Dir6 d) const
{
	Hexagon& hex = owner->game->Get(x, y);
	Hexagon* h2  = hex.neighbours[d];
	if (!h2)
		return false;
	return CanUnload(h2->x, h2->y);
}

void C35::Unit::DoUnload(int tx, int ty)
{
	Hexagon& hex = owner->game->Get(tx, ty);
	for (Unit* u : loaded)
	{
		hex.AddUnit(u);
	}
	loaded.clear();
}

void C35::Unit::DoUnload(Dir6 d)
{
	Hexagon& hex = owner->game->Get(x, y);
	Hexagon* h2  = hex.neighbours[d];
	assert(h2);
	DoUnload(h2->x, h2->y);
}

C35::Capabilities& C35::Unit::Capa()
{
	UnitType& ut = UnitType::Lookup(type);

	bool hm = stats.move != 0;

	Hexagon& hex = owner->game->Get(x, y);

	Movement&& mm = MakeMovement();

	capa.canMove          = hm;
	capa.canAttack        = hm && stats.off;
	capa.canDefend        = stats.def;
	capa.captureOnLoss    = false;
	capa.canBombard       = hm && stats.bombard_str && stats.oneoffs.bombard;
	capa.canAirdrop       = false;
	capa.canDefendBombard = stats.bombard_str && stats.oneoffs.def_bomb;
	capa.canBuild         = hm && ut.capa.worker;
	capa.canMoveCity      = hm;
	capa.canMoveLand      = hm && mm.land;
	capa.canMoveWater     = hm && mm.sea;
	capa.canSettle        = hm && ut.capa.settle && CanCity();
	capa.canJoin          = hm && ut.join_num;
	capa.canColony        = false;
	capa.canFortify       = true;

	return capa;
}

// ****************
// *** UnitType ***
// ****************

int                        C35::UnitType::nextId = 1;
std::vector<C35::UnitType> C35::UnitType::types;
C35::AD                    C35::UnitType::unit_icons;

C35::UnitType& C35::UnitType::New()
{
	/*static bool inited = false;
	if(!inited)
	{
		inited=true;
		nextId=1;
	}*/

	types.push_back(UnitType());

	types.back().id = nextId++;

	types.back().have_gfx = have_none;

	assert(types.size() == types.back().id);

	return types.back();
}

C35::UnitType& C35::UnitType::Lookup(int id)
{
	assert(id > 0);
	assert(id <= types.size());

	return types[id - 1];
}

int C35::UnitType::Size()
{
	return types.size();
}

void C35::UnitType::Clear()
{
	types.clear();
	nextId = 1;
}

namespace
{
SDL_Surface* ld_t(const char* name)
{
	static char buf[256];
	sprintf(buf, "gfx/%s.bmp", name);
	SDL_Surface* srf = SDL_LoadBMP(buf);
	assert(srf);
	Uint32 key = SDL_MapRGB(srf->format, 255, 0, 255);
	SDL_SetColorKey(srf, SDL_SRCCOLORKEY, key);
	return srf;
}
}  // namespace

void C35::UnitType::LoadBmp(string name)
{
	SDL_Surface* img = SDL_LoadBMP(("gfx/" + name + ".bmp").c_str());
	SDL_Surface* tm  = MakeTransMaskFromImage(img);
	cis.FromImg(img, tm, 0, 0);
	have_gfx = have_cis;
	SDL_FreeSurface(img);
	SDL_FreeSurface(tm);
}

void C35::UnitType::LoadAD(string name)
{
	std::ifstream ifs("gfx/" + name + ".ad", ios_base::in | ios_base::binary);
	ad.Load(ifs);
	have_gfx = have_ad;
}

void C35::UnitType::Instance(Player* p)
{
	if (have_gfx == have_ad)
		ad.Instance(p->color);
	if (have_gfx == have_ac)
		ac.Instance(p->color);
	if (have_gfx == have_cis)
		cis.Instance(p->color);
}

void C35::UnitType::LoadAC(string name)
{
	std::ifstream ifs("gfx/" + name + ".ac", ios_base::in | ios_base::binary);
	ac.Load(ifs);
	have_gfx = have_ac;
}

void C35::UnitType::LoadLate(string name)
{
	gfxname  = name;
	have_gfx = have_name;
}

void C35::UnitType::MakeWhite()
{
	// s.MakeWhite();
}
void C35::UnitType::DoneImg()
{
	// ad.FreeData();
	// ic.AllInstanced();
	// s.FreeData();
}

C35::AnimReflection C35::UnitType::GetIcon(UC hue)
{
	return unit_icons.Refl(icon_id, hue);
}

C35::BA& C35::UnitType::GetIcon()
{
	return unit_icons.Closest(icon_id);
}

void C35::UnitType::HaveAllNow()
{
	unit_icons.LoadExt("gfx/icons/ui.ad");

#ifdef _DEBUG
	unit_icons.DoAll<&BA::DoAll<&CIS::Skip3>>();
#endif

	for (Player* p : Player::All())
		if (!p->isbarb)
		{
			unit_icons.Instance(p->color);
			for (UnitType& ut : types)
				ut.Instance(p);
		}

	// for barbs
	BasicAnim& ba = unit_icons.Closest(barbarian_utp->icon_id);
	ba.DoAll<&CIS::MakeWhite>();
	ba.DoAll<&CIS::MakeWhite>();
	ba.DoAll<&CIS::MakeWhite>();
	ba.Instance(0);
}

void C35::UnitType::DoneAll()
{
	for (UnitType& ut : types)
	{
		ut.DoneImg();
	}
}

extern string ExtractFileExt(std::string fn);
extern string ExtractFileBase(std::string fn);

C35::AnimReflection C35::UnitType::GetRefl(string name, short dir, Player* p)
{
	if (have_gfx == have_name)
	{
		string ext = ExtractFileExt(gfxname);
		string nam = ExtractFileBase(gfxname);
		/**/ if (ext == "bmp")
			LoadBmp(nam);
		else if (ext == "ac")
			LoadAC(nam);
		else if (ext == "ad")
			LoadAD(nam);
		if (have_gfx != have_name)
		{
			for (Player* pp : Player::All())
				if (!pp->isbarb)
					Instance(pp);
		}
	}
	switch (have_gfx)
	{
	case have_ad:
		return ad.Refl(dir, p->color);
	case have_ac:
		return ac.Refl(name, dir, p->color);
	case have_cis:
		return cis.Refl(p->color);
	default:
		assert(false);
		throw "error";
	}
}

void C35::UnitType::Scale150()
{
	switch (have_gfx)
	{
	case have_ac:
		ac.DoAll<&NAV::DoAll<&AD::DoAll<&BA::DoAll<&CIS::Scale150>>>>();
		break;
	case have_ad:
		ad.DoAll<&BA::DoAll<&CIS::Scale150>>();
		break;
	case have_cis:
		cis.Scale150();
		break;
	default:
		assert(false);
		throw "error";
	}
}

int C35::UnitType::ToId(string n)
{
	for (UnitType& ut : types)
	{
		if (ut.name == n)
			return ut.id;
	}
	return 0;
}

C35::IdList C35::UnitType::ByName(string name)
{
	IdList vi;
	while (true)
	{
		if (name.empty())
			break;
		int p  = name.find(',');
		int id = ToId(name.substr(0, p));
		if (id)
			vi.push_back(id);
		if (p == string::npos)
			break;
		name = name.substr(p + 1);
	}
	return vi;
}

// *******************
// *** UnitCreator ***
// *******************

C35::UnitType* C35::worker_utp;
C35::UnitType* C35::settler_utp;
C35::UnitType* C35::warrior_utp;
C35::UnitType* C35::curragh_utp;
C35::UnitType* C35::horseman_utp;
C35::UnitType* C35::pikeman_utp;
C35::UnitType* C35::galley_utp;
C35::UnitType* C35::fregatte_utp;
C35::UnitType* C35::barbarian_utp;

#ifdef _DEBUG
//#define SIMPLELOAD
#endif

#define NOSCALE

void C35::UnitCreator()
{
	UnitType::Clear();

	UnitType::types.reserve((int)UnitId::max);

	const bool T = true;
	const bool F = false;
	{
		UnitType& worker = UnitType::New();
		assert(worker.id == (int)UnitId::worker);
		worker_utp = &worker;

		worker.basetype  = UnitType::None;
		worker.kind      = UnitType::Foot;
		worker.perk      = UnitType::NoPerk;
		worker.start_off = 0;
		worker.start_def = 0;
		worker.start_bmb = 0;
		worker.range     = 0;
		worker.move      = 1;
		worker.join_num  = 1;
		worker.name      = "Worker";
		worker.load_capa = 0;
		worker.cost      = 10;
		worker.capa      = Capa(F, F, F, F, T, F);
		worker.res       = 0;
		worker.icon_id   = 2;
#ifdef SIMPLELOAD
		worker.LoadBmp("plain/worker");
#else
		worker.LoadLate("units/worker.ac");
#ifndef NOSCALE
		worker.Scale150();
#endif
#endif
		worker.available = true;
	}

	{
		UnitType& settler = UnitType::New();
		assert(settler.id == (int)UnitId::settler);
		settler_utp = &settler;

		settler.basetype  = UnitType::None;
		settler.kind      = UnitType::Foot;
		settler.perk      = UnitType::NoPerk;
		settler.start_off = 0;
		settler.start_def = 0;
		settler.start_bmb = 0;
		settler.range     = 0;
		settler.move      = 1;
		settler.join_num  = 2;
		settler.name      = "Settler";
		settler.load_capa = 0;
		settler.cost      = 30;
		settler.capa      = Capa(F, F, F, F, F, T);
		settler.res       = 0;
		settler.icon_id   = 1;
#ifdef SIMPLELOAD
		settler.LoadBmp("plain/settler");
#else
		settler.LoadLate("units/settler.ac");
#ifndef NOSCALE
		settler.Scale150();
#endif
#endif
		settler.available = true;
	}

	{
		UnitType& warrior = UnitType::New();
		assert(warrior.id == (int)UnitId::warrior);
		warrior_utp = &warrior;

		warrior.basetype  = UnitType::Off;
		warrior.kind      = UnitType::Foot;
		warrior.perk      = UnitType::MeleeCombat;
		warrior.start_off = 1;
		warrior.start_def = 1;
		warrior.start_bmb = 0;
		warrior.range     = 0;
		warrior.move      = 1;
		warrior.join_num  = 0;
		warrior.name      = "Warrior";
		warrior.load_capa = 0;
		warrior.cost      = 10;
		warrior.capa      = Capa(T, F, T, F, F, F);
		warrior.res       = 0;
		warrior.icon_id   = 7;
#ifdef SIMPLELOAD
		warrior.LoadBmp("plain/warrior");
#else
		warrior.LoadLate("units/warrior.ac");
#ifndef NOSCALE
		warrior.Scale150();
#endif
#endif
		warrior.available = true;
	}

	{
		UnitType& curragh = UnitType::New();
		assert(curragh.id == (int)UnitId::curragh);
		curragh_utp = &curragh;

		curragh.basetype  = UnitType::Trn;
		curragh.kind      = UnitType::Sea;
		curragh.perk      = UnitType::NoPerk;
		curragh.start_off = 0;
		curragh.start_def = 0;
		curragh.start_bmb = 0;
		curragh.range     = 0;
		curragh.move      = 2;
		curragh.join_num  = 0;
		curragh.name      = "Curragh";
		curragh.load_capa = 1;
		curragh.cost      = 20;
		curragh.capa      = Capa(F, F, F, F, F, F);
		curragh.res       = 0;
		curragh.icon_id   = 198;
#ifdef SIMPLELOAD
		curragh.LoadBmp("plain/curragh");
#else
		curragh.LoadLate("units/curragh.ac");
#ifndef NOSCALE
		curragh.Scale150();
#endif
#endif
		curragh.available = true;
	}

	{
		UnitType& horseman = UnitType::New();
		assert(horseman.id == (int)UnitId::horseman);
		horseman_utp = &horseman;

		horseman.basetype  = UnitType::Off;
		horseman.kind      = UnitType::Mounted;
		horseman.perk      = UnitType::OpenTerrainCombat;
		horseman.start_off = 3;
		horseman.start_def = 1;
		horseman.start_bmb = 0;
		horseman.range     = 0;
		horseman.move      = 2;
		horseman.join_num  = 0;
		horseman.name      = "Horseman";
		horseman.load_capa = 0;
		horseman.cost      = 30;
		horseman.capa      = Capa(T, F, F, F, F, F);
		horseman.res       = 1;
		horseman.icon_id   = 12;
#ifdef SIMPLELOAD
		horseman.LoadBmp("plain/horseman");
#else
		horseman.LoadLate("units/horseman.ac");
#ifndef NOSCALE
		horseman.Scale150();
#endif
#endif
		horseman.available = true;
	}

	{
		UnitType& pikeman = UnitType::New();
		assert(pikeman.id == (int)UnitId::pikeman);
		pikeman_utp = &pikeman;

		pikeman.basetype  = UnitType::Def;
		pikeman.kind      = UnitType::Foot;
		pikeman.perk      = UnitType::MountedDefense;
		pikeman.start_off = 1;
		pikeman.start_def = 3;
		pikeman.start_bmb = 0;
		pikeman.range     = 0;
		pikeman.move      = 1;
		pikeman.join_num  = 0;
		pikeman.name      = "Pikeman";
		pikeman.load_capa = 0;
		pikeman.cost      = 30;
		pikeman.capa      = Capa(T, F, F, F, F, F);
		pikeman.res       = 2;
		pikeman.icon_id   = 13;
#ifdef SIMPLELOAD
		pikeman.LoadBmp("plain/pikeman");
#else
		pikeman.LoadLate("units/pikeman.ac");
#ifndef NOSCALE
		pikeman.Scale150();
#endif
#endif
		pikeman.available = true;
	}

	{
		UnitType& galley = UnitType::New();
		assert(galley.id == (int)UnitId::galley);
		galley_utp = &galley;

		galley.basetype  = UnitType::Trn;
		galley.kind      = UnitType::Sea;
		galley.perk      = UnitType::NoPerk;
		galley.start_off = 1;
		galley.start_def = 1;
		galley.start_bmb = 0;
		galley.range     = 0;
		galley.move      = 3;
		galley.join_num  = 0;
		galley.name      = "Galley";
		galley.load_capa = 2;
		galley.cost      = 40;
		galley.capa      = Capa(F, F, F, F, F, F);
		galley.res       = 0;
		galley.icon_id   = 30;
#ifdef SIMPLELOAD
		galley.LoadBmp("plain/curragh");
#else
		galley.LoadLate("units/galley.ac");
#ifndef NOSCALE
		galley.Scale150();
#endif
#endif
		galley.available = true;
	}

	{
		UnitType& fregatte = UnitType::New();
		assert(fregatte.id == (int)UnitId::fregatte);
		fregatte_utp = &fregatte;

		fregatte.basetype  = UnitType::Off;
		fregatte.kind      = UnitType::Sea;
		fregatte.perk      = UnitType::NoPerk;
		fregatte.start_off = 2;
		fregatte.start_def = 2;
		fregatte.start_bmb = 2;
		fregatte.range     = 1;
		fregatte.move      = 4;
		fregatte.join_num  = 0;
		fregatte.name      = "Fregatte";
		fregatte.load_capa = 0;
		fregatte.cost      = 60;
		fregatte.capa      = Capa(F, T, F, T, F, F);
		fregatte.res       = 0;
		fregatte.icon_id   = 32;
#ifdef SIMPLELOAD
		fregatte.LoadBmp("plain/fregatte");
#else
		fregatte.LoadLate("units/fregatte.ac");
#ifndef NOSCALE
		fregatte.Scale150();
#endif
#endif
		fregatte.available = true;
	}

	{
		UnitType& barbarian = UnitType::New();
		assert(barbarian.id == (int)UnitId::barbarian);
		barbarian_utp = &barbarian;

		barbarian.basetype  = UnitType::Off;
		barbarian.kind      = UnitType::Foot;
		barbarian.perk      = UnitType::NoPerk;
		barbarian.start_off = 1;
		barbarian.start_def = 1;
		barbarian.start_bmb = 0;
		barbarian.range     = 0;
		barbarian.move      = 1;
		barbarian.join_num  = 0;
		barbarian.name      = "Barbarian";
		barbarian.load_capa = 0;
		barbarian.cost      = 0;
		barbarian.capa      = Capa(T, F, F, F, F, F);
		barbarian.res       = 0;

#ifdef SIMPLELOAD
		barbarian.LoadBmp("plain/barbarian");
#else
		barbarian.LoadAC("units/barbarian");
#ifndef NOSCALE
		barbarian.Scale150();
#endif
#endif

		barbarian.available = false;
		barbarian.icon_id   = 7;
	}

#ifdef SIMPLELOAD

	int i, n = UnitType::Size();
	for (i = 1; i <= n; ++i)
	{
		UnitType& ut = UnitType::Lookup(i);
		ut.cis.Skip3();
		ut.cis.hx = ut.cis.w / 2;
		ut.cis.hy = ut.cis.h * 2 / 3;
	}

#endif
}
