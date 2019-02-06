
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

int _stdcall WinMain(void*,void*,char*,int)
{
	Main();
	return 0;
}

/*
int main(char** argv,int argc)
{
	Main();
	return 0;
}
*/

