
#include "stdafx.h"

// --------------------------------------------

#include "Hexagon.h"

#include "Unit.h"
#include "Orders.h"
#include "Player.h"
#include "City.h"

// ***************
// *** HexCore ***
// ***************

bool C35::HexCore::Bridge() const
{
	return IsRealLand() && (mask&bridge);
}

bool C35::HexCore::Hang() const
{
	return IsRealSea() && (mask&bridge);
}

bool C35::HexCore::Roaded() const
{
	return city || (mask & road) || (mask & rail) || ( (mask & bridge) && Hang() );
}

bool C35::HexCore::Railed() const
{
	return city || (mask & rail) ;
}

bool C35::HexCore::IsLand() const
{
	if(city) return true;
	switch(tile)
	{
		case sea:
		case coast:
		case ocean:
			return (mask&bridge) && Hang();
		default:
			return true;
	}
}

bool C35::HexCore::IsSea() const
{
	if(city) return true;
	switch(tile)
	{
		case coast:
		case sea:
		case ocean:
			return true;
		default:
			return false;
	}
}

bool C35::HexCore::IsRealLand() const
{
	switch(tile)
	{
		case sea:
		case coast:
		case ocean:
			return false;
		default:
			return true;
	}
}
bool C35::HexCore::IsRealSea() const
{
	switch(tile)
	{
		case sea:
		case coast:
		case ocean:
			return true;
		default:
			return false;
	}
}

// *****************
// *** PlayerHex ***
// *****************

C35::PlayerHex* C35::PlayerHex::Neighbours(Dir6 d)
{
	return neighbours[d];
}

int C35::PlayerHex::travelcost(Dir6 d)
{
	PlayerHex* h2 = neighbours[d];
	if(!h2) return 0;
	bool sea_ok = IsSea() && h2->IsSea();
	bool lnd_ok = IsLand() && h2->IsLand();
	if( (!sea_ok) && (!lnd_ok) ) return 0;

	if( IsRealLand() && (!city) && h2->IsRealSea() && !Bridge() ) return 0;
	if( IsRealSea() && h2->IsRealLand() && (!h2->city) && !h2->Bridge() ) return 0;

	if( Railed() && h2->Railed() ) return 1;
	if( Roaded() && h2->Roaded() ) return 1000;
	if( h2->mask & jungle ) return 9000;
	if( h2->mask & forest ) return 6000;
	if( h2->tile == mount ) return 9000;
	if( h2->tile == hill ) return 6000;
	return 3000;
}

C35::Unit* C35::PlayerHex::DisplayUnit()
{
	if(units.empty()) return 0;
	Unit* up=0;
	int value=0;
	for(Unit* u : units)
	{
		if(!up) up=u;
		if(u->currently) return u;
		int val = u->stats.def * u->stats.hp ;
		if(val > value)
		{
			up=u;
			value = val;
		}
	}
	return up;
}

// ***************
// *** Hexagon ***
// ***************

C35::Hexagon::Hexagon()
{
	city=0;
	owner=0;
	id=0;
	tile=0;
	mask=0;
	x=y=0;
	px=py=0;
	route_cost=route_from=0;
	flavor=0;
	workaction_progress=0;
	obscure=0;
	resource=0;
	goody=0;
	valid=false;
	surf=0;
}

namespace std {
template <typename T>
void operator |= ( std::set<T> & s, const T& v )
{
	s.insert(v);
}

template <typename T>
bool operator & ( const std::set<T> & s, const T& v )
{
	return s.find(v) != s.end();
}
}

std::set<C35::AvailAct> C35::Hexagon::Avail()
{
	std::set<C35::AvailAct> aa;

	aa & wa_road;

	if(city) return aa;

	if( !Roaded() )        aa |= wa_road ;
	else if( !Railed() )   aa |= wa_rail ;
	if( ! (mask&irri) ) 
	{
		bool ok = true;
		if( mask & (forest|jungle) ) ok=false;
		if(ok) switch(tile)
		{
			case floodp:
			case grass:
			case bonus:
			case plains:
			case desert:
				ok=true;
				break;
			default:
				ok=false;
		}
		if(ok)
		{
			ok=River();
			if(!ok) for( Hexagon* h : neighbours )
			{
				if(!h) continue;
				if( h->mask & irri )
				{
					ok=true;
					break;
				}
			}
		}
		if(ok) aa |= wa_irri;
	}
	if( ! (mask&mine) )
	{
		if( ! ( mask & (forest|jungle) ) )
			switch(tile)
			{
				case grass:
				case bonus:
				case plains:
				case hill:
				case mount:
				case desert:
				case tundra:
					aa |= wa_mine;
					break;
				default:
					break;
			}
	}
	if( mask & (forest|jungle) )
	{
		aa |= wa_clear;
	}

	return aa;
}


void C35::Hexagon::workstop()
{
	for(Unit* u : units)
	{
		if( u->orders && u->orders->WorkerJob() )
		{
			Orders* o = u->orders;
			u->orders = u->orders->queue;
			delete o;
			u->SetAnim("idle",270);
		}
	}
}

void C35::Hexagon::AddRoad()
{
	mask |= road;
	workstop();
	Invalidate();
	for(Dir6 d=d6_beg;d!=d6_end;++d)
	{
		Hexagon* n = neighbours[d];
		if(n) n->Invalidate();
	}
}

void C35::Hexagon::AddRail()
{
	mask |= road|rail;
	workstop();
	Invalidate();
	for(Dir6 d=d6_beg;d!=d6_end;++d)
	{
		Hexagon* n = neighbours[d];
		if(n) n->Invalidate();
	}
}

void C35::Hexagon::AddMine()
{
	mask |= mine;
	mask &= ~irri;
	workstop();
	Invalidate();
}

void C35::Hexagon::AddIrri()
{
	mask |= irri;
	mask &= ~mine;
	workstop();
	Invalidate();
}

void C35::Hexagon::AddForest()
{
	mask |= forest;
	mask &= ~(mine|irri);
	workstop();
	Invalidate();
}

void C35::Hexagon::AddBridge()
{
	mask |= bridge;
	workstop();
	Invalidate();

	for( Dir6 d = d6_beg; d!=d6_end; ++d)
	{
		Hexagon* h1 = neighbours[d];
		if(!h1) continue;
		if(!h1->IsSea()) continue;
		Hexagon* h2 = h1->neighbours[d];
		if(!h2) continue;
		if(!h2->IsLand()) continue;
		if(h2->mask & bridge)
		{
			h1->mask |= bridge;
			h1->Invalidate();
			h2->Invalidate();
			break;
		}

	}
}

C35::Dir6 C35::Hexagon::BridgeDir() const
{
	// 1 - find connection
	for( Dir6 d = d6_beg; d!=d6_end; ++d)
	{
		Hexagon* h1 = neighbours[d];
		if(!h1) continue;
		if(!h1->IsRealSea()) continue;
		Hexagon* h2 = h1->neighbours[d];
		if(!h2) continue;
		if(!h2->IsRealLand()) continue;
		if( (h1->mask & bridge) && (h2->mask & bridge) )
		{
			return d;
		}
	}
	// 2 - find possibility
	for( Dir6 d = d6_beg; d!=d6_end; ++d)
	{
		Hexagon* h1 = neighbours[d];
		if(!h1) continue;
		if(!h1->IsRealSea()) continue;
		Hexagon* h2 = h1->neighbours[d];
		if(!h2) continue;
		if(!h2->IsRealLand()) continue;
		return d;
	}
	// 2 - find water
	for( Dir6 d = d6_beg; d!=d6_end; ++d)
	{
		Hexagon* h1 = neighbours[d];
		if(!h1) continue;
		if(!h1->IsRealSea()) continue;
		return d;
	}
	// 3 - give up
	return d6_beg;
}

C35::Dir6 C35::Hexagon::HangDir() const 
{
	for( Dir6 d = (Dir6)0; d!=(Dir6)3; ++d)
	{
		Hexagon* h1 = neighbours[d];
		if(!h1) continue;
		if(!h1->IsRealLand()) continue;
		Hexagon* h2 = neighbours[ (Dir6)(d+3) ];
		if(!h2) continue;
		if(!h2->IsRealLand()) continue;
		if( ( h1->mask & bridge ) && ( h2->mask & bridge ) )
		{
			return Dir6(d%3);
		}
	}
	return d6_beg;
}


/*
C35::IdList C35::Hexagon::Actions()
{
	IdList o;
	if( ! (mask&road) ) o.push_back(Orders::road);
	else if( ! (mask&rail) ) o.push_back(Orders::rail);
	if( mask&(forest|jungle) ) o.push_back(Orders::clear);
	else
	{
		if(!(mask&irri)) o.push_back(Orders::irri);
		if(!(mask&mine)) o.push_back(Orders::mine);
	}
	return o;
}
*/

int C35::Hexagon::travelcost(C35::Dir6 d)
{
	Hexagon* h2 = neighbours[d];
	if(!h2) return 0;
	bool sea_ok = IsSea() && h2->IsSea();
	bool lnd_ok = IsLand() && h2->IsLand();
	if( (!sea_ok) && (!lnd_ok) ) return 0;

	if( IsRealLand() && (!city) && h2->IsRealSea() && !Bridge() ) return 0;
	if( IsRealSea() && h2->IsRealLand() && (!h2->city) && !h2->Bridge() ) return 0;
	//if( 

	//if( Hang() && 

	if( Railed() && h2->Railed() ) return 1;
	if( Roaded() && h2->Roaded() ) return 1000;
	if( h2->mask & jungle ) return 9000;
	if( h2->mask & forest ) return 6000;
	if( h2->tile == mount ) return 9000;
	if( h2->tile == hill ) return 6000;
	return 3000;
}

int C35::Hexagon::WorkMultiplyer()
{
	if( mask & jungle ) return 3;
	if( mask & forest ) return 2;
	if( tile == mount ) return 3;
	if( tile == hill )  return 2;
	return 1;
}

bool C35::Hexagon::River( Dir6 d ) const
{
	auto R = [] (Hexagon* h,Mask m) -> bool { if(!h) return false; return bool(h->mask & m) ; } ;

	switch(d)
	{
	case d6_upright:    return mask & river_upright   ;
	case d6_right:      return mask & river_right     ;
	case d6_downright:  return mask & river_downright ;
	case d6_downleft:   return R ( neighbours [ d6_downleft ] , river_upright   ) ;
	case d6_left:       return R ( neighbours [ d6_left     ] , river_right     ) ;
	case d6_upleft:     return R ( neighbours [ d6_upleft   ] , river_downright ) ;
	default:            return false;
	}
}

bool C35::Hexagon::River() const
{
	for( Dir6 d = d6_beg; d != d6_end; ++d )
	{
		if( River(d) ) return true;
	}
	return false;
}

void C35::Hexagon::River( Dir6 d, bool r )
{
	Invalidate();
	Hexagon* hex;
	int msk;
	switch(d)
	{
		case d6_upright:
			hex = this;
			msk = river_upright;
			break;
		case d6_right:
			hex = this;
			msk = river_right;
			break;
		case d6_downright:
			hex = this;
			msk = river_downright;
			break;
		case d6_downleft:
			hex = neighbours [ d6_downleft ];
			msk = river_upright;
			break;
		case d6_left:
			hex = neighbours [ d6_left ];
			msk = river_right ;
			break;
		case d6_upleft:
			hex = neighbours [ d6_upleft ];
			msk = river_downright;
			break;
		default:
			hex = 0;
	}
	if(!hex) return;
	if(r)
	{
		hex->mask |= msk;
	} else {
		hex->mask &= ~msk;
	}
}

C35::Unit* C35::Hexagon::DisplayUnit()
{
	if(units.empty()) return 0;
	Unit* up=0;
	int value=0;
	for(Unit* u : units)
	{
		if(!up) up=u;
		if(u->currently) return u;
		int val = u->stats.def * u->stats.hp ;
		if(val > value)
		{
			up=u;
			value = val;
		}
	}
	return up;
}

void C35::Hexagon::Owner( Player* p )
{
	owner=p;
	Invalidate();
	//if(p->
	if(p&&Goody())
	{
		p->gold += 10;
		Goody(0);
	}
}

void C35::Hexagon::AddUnit(Unit* u)
{
	units.push_back(u);
	u->x = x;
	u->y = y;
	if( Goody() && !u->owner->isbarb )
	{
		u->owner->gold += 10;
		Goody(0);
	}
}

void C35::Hexagon::RemoveUnit(Unit* u/*,UI* ui*/)
{
	//if(ui->ActiveUnit() == u)
	//	ui->Activate(0,0);
	auto iter = units.begin();
	auto end = units.end();
	for(;iter!=end;++iter)
	{
		if( *iter == u )
		{
			units.erase(iter);
			return;
		}
	}
	// error
}

void C35::Hexagon::ToStream(ostream& o) const
{
	WriteBinary(o, id);
	WriteBinary(o, tile);
	WriteBinary(o, mask);
	WriteBinary(o, resource);
	WriteBinary(o, goody );
	WriteBinary(o, x);
	WriteBinary(o, y);
}

void C35::Hexagon::FromStream(istream& i)
{
	ReadBinary(i, id);
	ReadBinary(i, tile);
	ReadBinary(i, mask);
	ReadBinary(i, resource);
	ReadBinary(i, goody );
	ReadBinary(i, x);
	ReadBinary(i, y);
}

void C35::Hexagon::AllToStream(ostream& o) const
{
	ToStream(o);
	WriteBinary(o,obscure);
	int p = 0;
	if(owner) p=owner->id;
	WriteBinary(o,p);
	City::ToStream(city, o);
	int i,n = units.size();
	WriteBinary(o,n);
	if(n)
	{
		WriteBinary(o,units[0]->owner->id);
	}
	for(i=0;i<n;++i)
	{
		units[i]->ToStream(o);
	}
}

void C35::Hexagon::AllFromStream(istream& i)
{
	FromStream(i);
	ReadBinary(i,obscure);
	int p;
	ReadBinary(i,p);
	if(p)
		owner = Player::Lookup(p);
	else
		owner = nullptr;
	city = City::FromStream(i,owner);
	if(city)
	{
		city->x = x;
		city->y = y;
	}
	units.clear();
	int j,n;
	ReadBinary(i,n);
	Player* uo;
	if(n)
	{
		ReadBinary(i,p);
		assert(p);
		uo = Player::Lookup(p);
	}
	for(j=0;j<n;++j)
	{
		Unit* u = Unit::FromStream(i,uo);
		u->owner = uo;
		AddUnit(u);
	}

}


