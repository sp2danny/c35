
#pragma once

#include <iostream>
#include <vector>
#include <set>

#include "Common.h"
/*
#include "Unit.h"
#include "Orders.h"
#include "UI.h"
*/
namespace std
{
	template <typename T>
		bool operator & ( const std::set<T> & s, const T& v ) ;
}

namespace C35
{

	enum AvailAct { wa_road, wa_rail, wa_mine, wa_irri, wa_forest, wa_clear };

	struct Hexagon;
	struct HexCore
	{
		int id;
		Tile tile;
		Mask mask;
		unsigned char flavor;
		int x,y;
		int px,py;
		unsigned char resource;
		unsigned char goody;
		City* city;
		Player* owner;
		char obscure;

		bool Roaded() const;
		bool Railed() const;
		bool IsLand() const;
		bool IsSea() const;
		bool IsRealLand() const;
		bool IsRealSea() const;
		bool Bridge() const;
		bool Hang() const;

		void clr() { tile=0; mask=0; flavor=resource=goody=0; city=0; owner=0; }

	private:

	};

	struct PlayerHex : public HexCore // the hex as a player sees it
	{
		int route_cost;
		int route_from;

		std::vector<Unit*> units;

		PlayerHex* Neighbours(Dir6);

		int travelcost(Dir6);

		Unit* DisplayUnit();
	private:
		PlayerHex* neighbours[d6_end];
	friend
		class PlayerBoard;
	};

	struct Hexagon : public HexCore // the real hex
	{
		Hexagon();

		std::set<AvailAct> Avail();

		int route_cost;
		int route_from;

		int workaction_progress;

		std::vector<Unit*> units;

		Unit* DisplayUnit();

		void Invalidate() { valid = false; }

		void Obscure ( int o     )  { obscure=o; Invalidate(); }
		void Goody   ( int g     )  { goody=g;   Invalidate(); } 
		void Town    ( City* c   )  { city=c;    Invalidate(); }
		void Owner   ( Player* p )  ;

		int      Obscure () const { return obscure; }
		int      Goody   () const { return goody;   }
		City*    Town    () const { return city;    }
		Player*  Owner   () const { return owner;   }

		bool River( Dir6 ) const;
		bool River() const;
		void River( Dir6, bool );
		Dir6 BridgeDir() const;
		Dir6 HangDir() const;

		void AddUnit(Unit*);
		void RemoveUnit(Unit*);

		void AddRoad(),AddRail(),AddMine(),AddIrri(),AddForest(),AddBridge();

		Hexagon* neighbours[d6_end];

		int travelcost(Dir6);

		int WorkMultiplyer();

		void ToStream(ostream&) const;
		void FromStream(istream&);

		void AllToStream(ostream&) const;
		void AllFromStream(istream&);

		SDL_Surface* Surface();

	private:
		void workstop();

		SDL_Surface* surf;
		bool valid;

	};


}