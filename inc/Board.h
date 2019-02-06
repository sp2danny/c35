
#pragma once

#include <vector>
#include <iostream>

#include "Common.h"


#include "Player.h"

#include "Hexagon.h"
//#include "Controller.h"
#include "Unit.h"
#include "Orders.h"
#include "GameEngine.h"

namespace C35
{
	class Board;

	class PlayerBoard // this is a board as a player sees it
	{
	public:

		PlayerHex& Get(Pos);
		PlayerHex& Get(int x,int y) { return Get(Pos(x,y)); }
		PlayerHex& Get(int);

		bool Inside(Pos) const;
		bool Inside(int x,int y) const { return Inside(Pos(x,y)); }

		void PathFrom(Pos,const Movement&);
		void PathFromTarget(Pos,const Movement&,Pos);

		int Width() const;
		int Height() const;
		int Size() const;

		Unit* DisplayUnit( Pos );
		Unit* DisplayUnit( int x,int y ) { return DisplayUnit(Pos(x,y)); }

	private:

		void AddUnitOrder( Unit* , Orders );
		void AddStackOrder( vector<Unit*> , Orders );

		PlayerBoard(Board*);

		void AllToStream(ostream&) const;
		void AllFromStream(istream&);

		int width,height;
		int turn;
		std::vector<PlayerHex> map;

		Board* owner;

	// internal

		void add_n( PlayerHex&, Dir6, int,int, bool );
		void do_neighbours(bool);

	friend
		class Board;
	friend
		class GameEngine;
	};

	class Board
	{
	public:
		Board();

		void MakeEmptyMap(int,int,bool);

		Hexagon& Get(Pos);
		Hexagon& Get(int x,int y) { return Get(Pos(x,y)); }
		Hexagon& Get(int);

		bool Inside(Pos) const;
		bool Inside(int x,int y) const { return Inside(Pos(x,y)); }

		void PathFrom(Pos,const Movement&);
		void PathFromTarget(Pos,const Movement&,Pos);

		int Width() const { return width; }
		int Height() const { return height; }

		void ToStream(ostream&) const;
		void FromStream(istream&); // loads a map

		void AllToStream(ostream&) const;
		void AllFromStream(istream&); // loads a game

		Player* AddHumanControlled(UC hue);
		Player* AddComputerControlled(UC hue);
		Player* AddRemoteControlled(UC hue);

		void PreparePlayerBoard(Player*);

		int HexCnt() const { return map.size(); }

		int TurnNumber() const { return turn; }

		Player& GetPlayer(int i) { return *players[i]; }

		Unit* DisplayUnit( Pos );

		void UpdateInflu();

		std::vector<UC> AllPlayerColors();

	private:
		int width,height;
		int turn;
		std::vector<Hexagon> map; // this is how it really looks
		std::vector<Player*> players;
		Player barb_controller;

	// internal

		void add_n( Hexagon&, Dir6, int,int, bool );

		void do_neighbours(bool);

	friend
		class GameEngine;


	};

}