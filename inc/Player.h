
#pragma once

#include <vector>
#include <iostream>

#include "Common.h"

namespace C35
{

	struct Available
	{
		bool road,rail_road,mine,irrigate,fort,forest;
		bool aqueduct, hospital, nuclear;
		bool techtrade, maptrade, mpp, rop, contact;
		bool bridge, seatravel, oceantravel;
		bool auto_reveal;
	};

	struct Player
	{
		char* name;
		Color color;

		int id;

		City* capital;

		Available avail;

		static int NewID();
		static void Register(int,Player*);
		static Player* Lookup(int);
		static Player* MakeNew();
		static void AllToStream(ostream&);
		static void AllFromStream(istream&);
		static int Count() { return nextId; }

		vector<const Player*> Met();

		PlayerBoard* board;
		Board* game;

		vector<Player*> met;

		int gold;
		bool isbarb;
		bool pak;

		static vector<Player*> All() { return vector<Player*>( players.begin()+1, players.end() ); }

	private:
		static int nextId;
		static vector<Player*> players;
	};

}