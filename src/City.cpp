
#include "stdafx.h"

// --------------------------------------------

#include "City.h"

#include "Unit.h"
#include "Board.h"
#include "Orders.h"
#include "Hexagon.h"

// ************
// *** City ***
// ************

void C35::City::Tick()
{
	Board* b = owner->game;

	culture += Capital()?1:0;

	float shieldsFactor = 1.0f;
	int xp_bonus = 0;

	enum { land,air,sea,none } bld = none;

	if(building_unit)
	{
		UnitType& ut = UnitType::Lookup(build_id);
		switch(ut.kind)
		{
		case UnitType::Foot:
		case UnitType::Mounted:
		case UnitType::Vehicle:
			bld=land;
			break;
		case UnitType::Sea:
			bld=sea;
			break;
		case UnitType::Air:
			bld=air;
			break;
		}
	}


	int grow_max = 6; // b->Get(x,y).River() ? 12 : 6;

	for( CityImprovement& ci : buildings )
	{
		culture += std::floor(ci.effective_culture);
		ci.effective_culture *= 1.01f;
		ImprovementType& it = ImprovementType::Lookup(ci.imp_id);
		shieldsFactor *= it.shieldsFactor;
		switch(bld)
		{
			case land: xp_bonus += it.landXpBonus ; break;
			case air:  xp_bonus += it.airXpBonus  ; break;
			case sea:  xp_bonus += it.seaXpBonus  ; break;
		}
		if( it.enableCity )  grow_max = 12;
		if( it.enableMetro ) grow_max = 99;
	}

	foodbox += size;

	int maxf = MaxFood();
	if(foodbox>maxf) foodbox=maxf;
	if( (foodbox>=maxf) && (size<grow_max) )
	{
		foodbox -= maxf;
		size += 1;
	}

	if( ! (building_unit || building_improvement) ) { shieldbox=0; return; }

	shieldbox += shieldsFactor*(1+size);

	int maxs = MaxShield();
	if(shieldbox>=maxs)
	{
		if(building_improvement)
		{
			shieldbox=0;
			AddImp(build_id);
			building_improvement = building_unit = false;
		}
		else if(building_unit)
		{
			UnitType& ut = UnitType::Lookup(build_id);
			int poploss = ut.join_num;
			if( (size - poploss) >= 1 )
			{
				size -= poploss;
				shieldbox=0;
				Unit* u = UnitType::MakeNewFromType(build_id,owner);
				Hexagon& hex = b->Get(x,y);
				hex.AddUnit(u);
				u->owner = owner;
				while(xp_bonus--) u->Won();
				if(rallypoint)
				{
					Orders* o = u->orders = new Orders();
					o->action = Orders::move;
					o->tx = rallypoint_x;
					o->ty = rallypoint_y;
					o->queue = 0;
					u->ExecuteOrder(b);
				}
			} else {
				shieldbox = maxs;
			}
		}
	}

}

std::string C35::City::BuildingName()
{
	if(building_improvement)
	{
		ImprovementType& it = ImprovementType::Lookup(build_id);
		return it.name;
	}
	else if(building_unit)
	{
		UnitType& ut = UnitType::Lookup(build_id);
		return ut.name;
	}
	return "wealth";
}

/*
static void RecursiveConnectDir( C35::Hexagon* h, C35::Dir6 general )
{
	using namespace C35;
	int dirs[] = { general, (general+1)%6, (general+5)%6 };
	h->route_cost=1;
	for( int d : dirs )
	{
		Hexagon* n = h->neighbours[d];
		if(!n) continue;
		if(n->route_cost) continue;
		if(n->Roaded())
			RecursiveConnectDir(n,general);
	}
}
*/

static void RecursiveConnect( C35::Hexagon* h )
{
	using namespace C35;
	h->route_cost=1;
	for( Dir6 d=d6_beg; d!=d6_end; ++d )
	{
		Hexagon* n = h->neighbours[d];
		if(!n) continue;
		if(n->route_cost) continue;
		if(n->Roaded())
			RecursiveConnect(n);
	}
}

void C35::City::CalcRes()
{
	Board* brd = owner->game;

	int i,n=brd->HexCnt();
	for(i=1;i<=n;++i)
		brd->Get(i).route_cost=0;

	Hexagon* h = & brd->Get(x,y);
	RecursiveConnect(h);

	for(i=0;i<16;++i)
		resources[i] = 0;

	for(i=1;i<=n;++i)
	{
		h = & brd->Get(i);
		if(!h->route_cost) continue;
		if(!h->Owner()) continue;
		if(h->resource)
			resources[h->resource] = true;
	}

}

bool C35::City::Capital()
{
	return owner->capital == this;
}

int C35::City::Influence()
{
	int infl = 1;
	int ii = 10;
	while(true)
	{
		if(culture>=ii)
		{
			infl += 1;
			ii *= 10;
		} else {
			break;
		}
	}
	return infl;
}

void C35::City::AddImp(int id)
{
	ImprovementType it = ImprovementType::Lookup(id);
	CityImprovement ci;
	ci.effective_culture = it.culture;
	if(owner->game)
		ci.was_build = owner->game->TurnNumber();
	ci.imp_id = id;
	buildings.push_back(ci);
}

int C35::City::MaxFood()
{
	return size*10;
}

int C35::City::MaxShield()
{
	if(building_unit)
	{
		UnitType& ut = UnitType::Lookup(build_id);
		return ut.cost;
	}
	else if(building_improvement)
	{
		ImprovementType& it = ImprovementType::Lookup(build_id);
		return it.shields;
	}
	else
		return 0;
}

bool C35::City::Coastal()
{
	Board* b = owner->game;
	Hexagon& h = b->Get(x,y);
	for( Dir6 d=d6_beg; d!=d6_end; ++d )
	{
		Hexagon* n = h.neighbours[d];
		if( n && n->IsSea() )
			return true;
	}
	return false;
}

C35::IdList C35::City::AvailImp()
{
	IdList lst;

	Hexagon& hex = owner->game->Get(x,y);

	int i,n = ImprovementType::Count();
	for(i=1;i<=n;++i)
	{
		bool ok = true;
		for( CityImprovement& ci : buildings )
		{
			if( ci.imp_id == i )
			{
				ok = false; // i already have it
				break;
			}
			ImprovementType& it = ImprovementType::Lookup(ci.imp_id);
			if(it.disablesImp == i)
			{
				ok = false; // i have a building that disables this
				break;
			}
		}
		if(!ok) continue;

		ImprovementType& it = ImprovementType::Lookup(i);

		if(!it.visible) continue;

		if(it.needRiver && !hex.River()) continue;
		if(it.needSea && !Coastal()) continue;

		for(int res=1;res<16;++res)
		{
			if( it.reqStrategic[res] )
			{
				if(!resources[res])
				{
					ok=false; // lack required resource
					break;
				}
			}
		}
		if(!ok) continue;

		if( it.requiresImp )
		{
			ok=false;
			for( CityImprovement& ci : buildings )
			{
				if( ci.imp_id == it.requiresImp )
				{
					ok = true; // have required building
					break;
				}
			}
		}
		if(!ok) continue;


		lst.push_back(i);
	}

	return lst;
}

C35::IdList C35::City::AvailUnit()
{
	IdList lst;

	int i,n = UnitType::Size();
	for(i=1;i<=n;++i)
	{
		UnitType& ut = UnitType::Lookup(i);
		if( (ut.kind==UnitType::Sea) && !Coastal() ) continue;
		if( ut.res && ! resources[ut.res] ) continue;
		if( !ut.available ) continue;
		lst.push_back(i);
	}

	return lst;
}

static std::string LoadStr( std::istream& is )
{
	std::string s;
	while(true)
	{
		char c;
		ReadBinary(is,c);
		if(!c) break;
		s+=c;
	}
	return s;
}

static void SaveStr( std::ostream& os, std::string str )
{
	os.write( str.c_str(), str.size()+1 );
}

void C35::City::ToStream( City* c, std::ostream& o )
{
	if(!c)
	{
		WriteBinary(o, (int)0 );
		return;
	}
	WriteBinary(o, c->size );
	SaveStr(o,c->name);
	WriteBinary(o, c->build_id );
	WriteBinary(o, c->building_unit );
	WriteBinary(o, c->building_improvement );
	WriteBinary(o, c->build_id );
	WriteBinary(o, c->foodbox );
	WriteBinary(o, c->shieldbox );
	WriteBinary(o, c->culture );
	WriteBinary(o, c->rallypoint );
	WriteBinary(o, c->rallypoint_x );
	WriteBinary(o, c->rallypoint_y );

	int j,n = c->buildings.size();
	WriteBinary(o,n);
	for(j=0;j<n;++j)
	{
		CityImprovement& ci = c->buildings[j];
		WriteBinary( o, ci.imp_id );
		WriteBinary( o, ci.effective_culture );
		WriteBinary( o, ci.was_build );

	}

}

C35::City* C35::City::FromStream( std::istream& i, Player* p )
{
	int sz;
	ReadBinary(i,sz);
	if(!sz) return nullptr;

	City* c = new City();
	c->size = sz;
	c->name = LoadStr(i);
	ReadBinary(i, c->build_id             );
	ReadBinary(i, c->building_unit        );
	ReadBinary(i, c->building_improvement );
	ReadBinary(i, c->build_id             );
	ReadBinary(i, c->foodbox              );
	ReadBinary(i, c->shieldbox            );
	ReadBinary(i, c->culture              );
	ReadBinary(i, c->rallypoint           );
	ReadBinary(i, c->rallypoint_x         );
	ReadBinary(i, c->rallypoint_y         );

	c->owner = p;

	c->buildings.clear();
	int j,n;
	ReadBinary(i, n);
	for(j=0;j<n;++j)
	{
		int bld;
		ReadBinary(i, bld);
		c->AddImp(bld);
		ReadBinary(i, c->buildings.back().effective_culture );
		ReadBinary(i, c->buildings.back().was_build );
	}

	return c;
}


// ***********************
// *** ImprovementType ***
// ***********************

std::vector<C35::ImprovementType> C35::ImprovementType::list;
C35::AD C35::ImprovementType::icons;

C35::AnimReflection C35::ImprovementType::Refl()
{
	return icons.Refl(icon_id,0);
}

C35::BA& C35::ImprovementType::Icon()
{
	return icons.Closest(icon_id);
}

int C35::ImprovementType::ToId(string n)
{
	for(ImprovementType& it : list)
	{
		if(it.name==n) return it.Id;
	}
	return 0;
}

C35::IdList C35::ImprovementType::ByName(string name)
{
	IdList vi;
	while(true)
	{
		if(name.empty()) break; 
		int p = name.find(',');
		int id = ToId( name.substr(0,p) );
		if(id)
			vi.push_back(id);
		if(p==string::npos) break;
		name = name.substr( p+1 );
	}
	return vi;
}

C35::ImprovementType& C35::ImprovementType::Lookup(int id)
{
	assert(id>=1);
	assert(id<=list.size());

	return list[id-1];
}

void C35::ImprovementType::Add(ImprovementType& it)
{
	it.Id = list.size()+1;
	list.push_back(it);
}


// **************************
// *** ImprovementCreator ***
// **************************

static void ClearAll(C35::ImprovementType& it)
{
	it.name              = "Noname";
	it.visible           = false ;
	it.shields           = 0;
	it.enableCity        = false ;
	it.enableMetro       = false ;
	it.enableLuxBonus    = false ;
	it.enableNoPopPoll   = false ;
	it.enableNoImpPoll   = false ;
	it.enableSeaTrade    = false ;
	it.enableAirTrade    = false ;
	it.canHurry          = false ;
	it.canTourist        = false ;
	it.landXpBonus       = 0 ;
	it.seaXpBonus        = 0 ;
	it.airXpBonus        = 0 ;
	it.cost              = 0 ;
	it.pollution         = 0 ;
	it.content           = 0 ;
	it.culture           = 0 ;
	it.wealthFactor      = 1.0f ;
	it.shieldsFactor     = 1.0f ;
	it.researchFactor    = 1.0f ;
	it.requiresImp       = 0 ;
	it.requiresTech      = 0 ;
	it.disablesImp       = 0 ;
	it.needRiver         = false ;
	it.needSea           = false ;
	for(int i=0;i<16;++i)
		it.reqStrategic[i] = false ;
	it.oneOffCiv         = false ;
	it.oneOffWorld       = false ;
	it.giveCivPerk       = 0 ;
}

int C35::river_id;

void C35::ImprovementCreator()
{
	ImprovementType::Clear();

	ImprovementType::icons.LoadExt("gfx/icons/ii.ad");
#ifdef _DEBUG
	ImprovementType::icons.DoAll<&BA::DoAll<&CIS::Skip3>>();
#endif
	ImprovementType::icons.Instance(0);

	int temple_id,library_id,aq_id;

	{
		ImprovementType temple;

		ClearAll(temple);
		temple.name      = "Temple";
		temple.visible   = true ;
		temple.shields   = 60 ;
		temple.cost      = 1 ;
		temple.culture   = 2 ;
		temple.content   = 1 ;
		temple.icon_id   = 4;

		ImprovementType::Add(temple);
		temple_id = temple.Id;
	}

	{
		ImprovementType barracks;

		ClearAll(barracks);
		barracks.name         = "Barracks";
		barracks.visible      = true ;
		barracks.shields      = 40 ;
		barracks.cost         = 1 ;
		barracks.landXpBonus  = 1 ;
		barracks.icon_id      = 2;

		ImprovementType::Add(barracks);
	}

	{
		ImprovementType cathedral;

		ClearAll(cathedral);
		cathedral.name      = "Cathedral";
		cathedral.visible   = true ;
		cathedral.shields   = 80 ;
		cathedral.cost      = 2 ;
		cathedral.culture   = 3 ;
		cathedral.content   = 3 ;
		cathedral.requiresImp = temple_id;
		cathedral.icon_id   = 11;

		ImprovementType::Add(cathedral);
	}

	{
		ImprovementType colloseum;

		ClearAll(colloseum);
		colloseum.name      = "Colloseum";
		colloseum.visible   = true ;
		colloseum.shields   = 60 ;
		colloseum.cost      = 2 ;
		colloseum.culture   = 2 ;
		colloseum.content   = 2 ;
		colloseum.icon_id   = 13;

		ImprovementType::Add(colloseum);
	}

	{
		ImprovementType library;

		ClearAll(library);
		library.name      = "Library";
		library.visible   = true ;
		library.shields   = 40 ;
		library.cost      = 1 ;
		library.culture   = 1 ;
		library.researchFactor = 1.5f;
		library.icon_id   = 6;

		ImprovementType::Add(library);
		library_id=library.Id;
	}

	{
		ImprovementType university;

		ClearAll(university);
		university.name      = "University";
		university.visible   = true ;
		university.shields   = 80 ;
		university.cost      = 2 ;
		university.culture   = 1 ;
		university.researchFactor = 1.5f;
		university.requiresImp = library_id;
		university.icon_id   = 12;

		ImprovementType::Add(university);
	}

	{
		ImprovementType aqueduct;

		ClearAll(aqueduct);
		aqueduct.name       = "Aqueduct";
		aqueduct.visible    = true ;
		aqueduct.shields    = 80 ;
		aqueduct.cost       = 1 ;
		aqueduct.culture    = 0 ;
		aqueduct.enableCity = true;
		aqueduct.icon_id    = 9;

		ImprovementType::Add(aqueduct);
		aq_id = aqueduct.Id;
	}

	{
		ImprovementType river;

		ClearAll(river);
		river.name        = "river";
		river.visible     = false ;
		river.enableCity  = true;
		river.disablesImp = aq_id;

		ImprovementType::Add(river);
		river_id = river.Id;
	}

	{
		ImprovementType harbour;

		ClearAll(harbour);
		harbour.name       = "Harbour";
		harbour.visible    = true ;
		harbour.shields    = 80 ;
		harbour.cost       = 1 ;
		harbour.seaXpBonus = 1;
		harbour.needSea    = true;
		harbour.enableSeaTrade = true;
		harbour.icon_id    = 26;

		ImprovementType::Add(harbour);
	}

	{
		ImprovementType courthouse;

		ClearAll(courthouse);
		courthouse.name       = "Courthouse";
		courthouse.visible    = true ;
		courthouse.shields    = 120 ;
		courthouse.cost       = 1 ;
		courthouse.icon_id    = 7;

		ImprovementType::Add(courthouse);
	}

	{
		ImprovementType marketplace;

		ClearAll(marketplace);
		marketplace.name       = "Marketplace";
		marketplace.visible    = true ;
		marketplace.shields    = 80 ;
		marketplace.cost       = 1 ;
		marketplace.icon_id    = 5;
		marketplace.enableLuxBonus = true;
		marketplace.wealthFactor = 1.5f;

		ImprovementType::Add(marketplace);
	}

	{
		ImprovementType granary;

		ClearAll(granary);
		granary.name       = "Granary";
		granary.visible    = true ;
		granary.shields    = 40 ;
		granary.cost       = 1 ;
		granary.icon_id    = 3;

		ImprovementType::Add(granary);
	}


	{
		ImprovementType palace;

		ClearAll(palace);
		palace.name       = "Palace";
		palace.visible    = true ;
		palace.shields    = 1000 ;
		palace.icon_id    = 1;
		palace.canHurry   = false;
		palace.culture    = 1;
		palace.oneOffCiv  = true;

		ImprovementType::Add(palace);
	}

}
