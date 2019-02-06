#pragma once

#include <list>
#include <string>

#include "Common.h"

extern void Main();

namespace C35
{

class UpdateTarget
{
public:
	virtual ~UpdateTarget() {}
	virtual void Update(int) =0;
	virtual bool Done() { return false; }
	virtual bool Unload() { return false; }
};

class InputTarget
{
public:
	virtual ~InputTarget() {}
	virtual void ParseInput(SDL_Event&) =0;
	virtual bool Done() { return false; }
	virtual bool Unload() { return false; }
};

class Frame : public UpdateTarget, InputTarget
{
public:
	virtual ~Frame() {}

	//virtual void Update(int) =0;
	virtual void Display() =0;
	//virtual void ParseInput(SDL_Event&) =0;
	virtual bool Done() =0;
	virtual bool Unload() =0;

	static void PushUnder(Frame*);
	static void Push(Frame*);

	static void Modal(Frame*);
	static void System(Frame*);

	static void AddListener(InputTarget*);
	static void AddActive(UpdateTarget*);

//protected:
	static SDL_Surface* screen;

private:
	static std::list<Frame*> stack;
	static Frame* modal;
	static Frame* system;
	static std::list<UpdateTarget*> actives;
	static std::list<InputTarget*> listeners;

public:
	static void Init(std::string);
	static void Run();

friend
	void Main();

};

}