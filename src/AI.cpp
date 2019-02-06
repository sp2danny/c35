
#include "stdafx.h"

// --------------------------------------------

#include "AI.h"

#include "GameEngine.h"

void C35::AI::Update(int ms)
{
	static int accu =0;

	if( GameEngine::CP()->pak )
	{
		accu = 0;
		return;
	}


	accu += ms;
	if( accu > 800 )
	{
		accu=0;
		GameEngine::TurnDone();
	}
}

