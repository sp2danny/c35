
#pragma once

#include "Board.h"
#include "Common.h"
#include "FrameSystem.h"

namespace C35
{
	class GUI : public Frame
	{
	public:
		GUI();
		~GUI();

		virtual void Update(int);
		virtual void Display();
		virtual void ParseInput(SDL_Event&);
		virtual bool Done();
		virtual bool Unload();
		static void HaveAllNow();
	private:
		int delta;
		bool done;
	};
}



