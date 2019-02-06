
#include "stdafx.h"

// --------------------------------------------

#include "FrameSystem.h"

#include "GameEngine.h"

std::list<C35::Frame*> C35::Frame::stack;
std::list<C35::UpdateTarget*> C35::Frame::actives;
std::list<C35::InputTarget*> C35::Frame::listeners;

C35::Frame* C35::Frame::modal = 0;

C35::Frame* C35::Frame::system = 0;

SDL_Surface* C35::Frame::screen = 0;

#ifndef _DEBUG
#include <Windows.h>
#endif

namespace {
	#ifdef _DEBUG
		const int WW = 1024;
		const int HH =  768;
		//const int WW = 1680;
		//const int HH = 1050;

	#else
		//const int WW = 1680;
		//const int HH = 1050;

		//const int WW = 1024;
		//const int HH =  768;

		//const int WW = 1280;
		//const int HH = 1024;

		RECT r;
		BOOL ok = GetWindowRect( GetDesktopWindow(), &r );

		/*const*/ int WW = ok ? r.right  : 1280 ;
		/*const*/ int HH = ok ? r.bottom : 1024 ;

	#endif
}

void C35::Frame::Init(std::string name)
{
	stack.clear();
	listeners.clear();
	actives.clear();
	modal=0;
	system=0;
	SDL_Init(SDL_INIT_EVERYTHING);

	int mask = SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF;
	#ifdef NDEBUG
	mask |= SDL_FULLSCREEN;
	#endif

	screen = SDL_SetVideoMode(WW,HH,24, mask );

	assert(screen);

	SDL_WM_SetCaption( name.c_str(), 0 );

	srand(SDL_GetTicks());

	TTF_Init();
}

void C35::Frame::AddListener(InputTarget* it)
{
	listeners.push_back(it);
}

void C35::Frame::AddActive(UpdateTarget* ut)
{
	actives.push_back(ut);
}

void C35::Frame::Run()
{
	int last = SDL_GetTicks();
	unsigned long int black = SDL_MapRGB( screen->format, 0,0,0 );
	SDL_Event lst_mm_e; lst_mm_e.type=SDL_MOUSEMOTION; lst_mm_e.motion.x=lst_mm_e.motion.y=0;
	for(;;)
	{
		if( stack.empty() ) break;

		// --- Input ---
		SDL_Event e;
		int i = SDL_PollEvent(&e);
		if(i)
		{
			if( e.type == SDL_MOUSEMOTION ) lst_mm_e=e;
			if( e.type == SDL_QUIT ) return;
			if(modal)
				modal->ParseInput(e);
			else
				stack.back()->ParseInput(e);
			if(system)
				system->ParseInput(e);
			for( auto it : listeners ) it->ParseInput(e);
		}

		// --- Update ---
		int now = SDL_GetTicks();
		GameEngine::Update(now-last);
		stack.back()->Update(now-last);
		if(modal)
			modal->Update(now-last);
		if(system)
			system->Update(now-last);
		for( auto ut : actives ) ut->Update(now-last);
		last = now;

		// --- Display ---
		SDL_FillRect(screen,0,black);
		stack.back()->Display();
		if(modal)
			modal->Display();
		if(system)
			system->Display();
		SDL_Flip(screen);

		// --- Frame Phase ---
		if( system && system->Done() ) return;
		if( modal )
		{
			if(modal->Done())
			{
				if(modal->Unload()) delete modal;
				modal=0;
			}
		} else {
			if( stack.back()->Done() )
			{
				if( stack.back()->Unload() ) delete stack.back();
				stack.pop_back();
				if(!stack.empty())
					stack.back()->ParseInput(lst_mm_e);
			}
		}
		for( auto iter = listeners.begin(); iter != listeners.end(); )
		{
			if( (*iter)->Done() )
			{
				if( (*iter)->Unload() ) delete (*iter);
				iter = listeners.erase(iter);
			} else {
				++iter;
			}
		}
		for( auto iter = actives.begin(); iter != actives.end(); )
		{
			if( (*iter)->Done() )
			{
				if( (*iter)->Unload() ) delete (*iter);
				iter = actives.erase(iter);
			} else {
				++iter;
			}
		}
	}
}

void C35::Frame::PushUnder(Frame* f)
{
	auto itr = stack.end();
	--itr;
	if( itr == stack.begin() )
		stack.push_front(f);
	else
		stack.insert(itr,f);
}

void C35::Frame::Push(Frame* f)
{
	stack.push_back(f);
}

void C35::Frame::Modal(Frame* m)
{
	modal=m;
}

void C35::Frame::System(Frame* s)
{
	system=s;
}


