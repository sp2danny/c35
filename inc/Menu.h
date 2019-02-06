
#pragma once

#include <vector>
#include <string>

#include "Common.h"
#include "Hexagon.h"
#include "Font.h"

struct SDL_Surface;

namespace C35
{

	struct MenuItem;

	struct MenuChoise
	{
		enum { ActivateUnits, SleepUnits, GotoUnits, ZoomCity, RallyPoint, JustClose } kind;

		vector<Unit*> units;
		City* city;
	};

	namespace Menu
	{
		void InitMenu(Font& f);
		void MakeFrom(PlayerHex& h, int mx,int my);
		void Done();

		bool Active();

		void Display(int mx,int my);
		bool ParseClick(int mx,int my, MenuChoise& );

	}

	namespace UnitInfo
	{
		void MakeFrom(Unit* u);
		void Display(int mx,int my);
	}



}