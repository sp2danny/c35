#pragma once

#include "FrameSystem.h"
#include "Common.h"
#include "Font.h"

namespace C35
{

class CityView : public Frame
{
public:

	virtual void Update(int);
	virtual void Display();
	virtual void ParseInput(SDL_Event&);
	virtual bool Done();
	virtual bool Unload();

	static void Init( );
	static void ViewCity(City*);
};

}


