
#pragma once

#include "Common.h"

namespace C35
{

	namespace UnitAction
	{

		// gui interface
		void MakeFrom( Unit*, int x, int y );
		void DrawIcons( int mx, int my );
		bool ProcessClick( int mx, int my );
		bool ProcessKeypress( SDL_Event* );

		// ai interface
		void DoSettle  (Unit*);
		void DoRoad    (Unit*);
		void DoMine    (Unit*);
		void DoIrri    (Unit*);
		void DoRail    (Unit*);
		void DoClear   (Unit*);
		void GoTo      (Unit*,Pos);
		void Go1       (Unit*,Dir6);
		void DoFortify (Unit*);
		void DoWake    (Unit*);
		void DoWaste   (Unit*);
		void DoPillage (Unit*);
		void DoBombard (Unit*,Pos);

	};


}

