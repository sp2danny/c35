
#pragma once

#include "FrameSystem.h"
#include "Anim.h"
#include "Common.h"

namespace C35
{

	class LockedAnimation : public UpdateTarget
	{
	public:
		virtual void Update(int)=0;
		virtual bool Done()=0;
		virtual AnimReflection Refl()=0;
		virtual Pos Position()=0;

	};

	LockedAnimation* NewMovementAnim(Unit*,Dir6);

	LockedAnimation* NewFightAnim(Unit*,Dir6);

}

