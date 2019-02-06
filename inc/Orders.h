
#pragma once

#include "Common.h"

namespace C35
{

	struct Orders
	{
		enum { move, move1, explore, road, autopilot, roadto, rail, mine, irri, bridge, clear, attack, fortify, wake  };
		int action;
		bool WorkerJob() { return action >= road && action <= clear ; }
		union {
			struct {
				int tx,ty;
			};
			struct {
				int dir;
				int data;
			};
		};
		Orders* queue;

		Orders(int a)       : action(a),tx(0),ty(0),queue(0) {}
		Orders()            : action(0),tx(0),ty(0),queue(0) {}
		Orders(int x,int y) : action(0),tx(x),ty(y),queue(0) {}
		Orders(Orders* q)   : action(0),tx(0),ty(0),queue(q) {}

	};
}