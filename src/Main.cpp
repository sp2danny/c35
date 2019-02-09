
#include "stdafx.h"

// --------------------------------------------

#include "FrameSystem.h"
#include "FrameForwards.h"


void Main()
{
	C35::Frame::Init( "C35" );
	C35::Frame::Push( GetLoadScreen() );
	C35::Frame::Run();
}

/* */
int _stdcall WinMain(void*,void*,char*,int)
{
	Main();
	return 0;
}
/* */

/*
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	Main();
	return 0;
}
*/

