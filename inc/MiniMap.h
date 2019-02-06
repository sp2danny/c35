
#pragma once

#include "Board.h"

namespace C35
{


	class MiniMap
	{
	public:
		static void Init();
		static void Update(PlayerBoard&);
		static void Display(SDL_Surface*, int,int);
		static bool ParseClick( int,int, float&,float& );

	};


}
