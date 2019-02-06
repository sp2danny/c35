#pragma once

#include "FrameSystem.h"

namespace C35
{

	class AI : public UpdateTarget
	{
	public:
		~AI() {}
		virtual void Update(int);
	};


}