#pragma once

#include "Common.h"

namespace C35
{

class UI
{
public:
	UI(Board*,Player*);
	~UI();
	Unit* ActiveUnit();
	Unit* NextUnit();
	Unit* PrevUnit();
	Hexagon* OnHex();
	void Activate(Unit*,Hexagon*);

	void ReadyTurn();
	void FinishTurn();
	bool AllDone();
	Unit* NextNotDone();
	void ReObsc();

	void SetHuman(Player* p) { player=p; }

	void EnterFight();

	Player* GetHuman() { return player; }

private:
	Unit* active;
	Board* board;
	Player* player;
};

}
