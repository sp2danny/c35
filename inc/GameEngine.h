
#pragma once

#include <deque>

#include "Board.h"
#include "FrameSystem.h"
#include "Player.h"
#include "LockedAnimation.h"

namespace C35
{

	class GameEngine
	{
	public:
		static void Init();

		static void LoadGame(const char*);
		static void LoadMap(const char*);
		static void RandomMap();
		static void SaveGame(const char*);

		static void Update(int);

		static Board& Game();

		// this is the interface the GUI / AI uses

		static Player* CurrentlyPlaying();
		static Player* PresentAtKeyboard();

		static Player* CP()  { return CurrentlyPlaying();  }
		static Player* PAK() { return PresentAtKeyboard(); }

		static Unit* ActiveUnit();
		static Unit* NextUnit();

		static void WasModifyed();

		static void ActivateUnit(Unit*);

		static void AddUnitOrder( Unit* , Orders );
		static void AddStackOrder( vector<Unit*> , Orders );

		static void TurnDone();

		static bool ActionPosition( Pos& );
		static bool InAnimation( AnimReflection& ar, Pos& worldCoord );
		static bool InAnimation();

		static void SetLA(LockedAnimation&);

		static Frame* GUI();
		
	private:

		static void PrepareTurn(bool=true);
		static void Setup(bool=true);
		static Board board;
		static Frame* gui;

		static Unit* active;

		static int ap, hp;
		static std::deque<Pos> ap_stack;
	};

}

