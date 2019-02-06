
#pragma once

#include <map>

#include "Anim.h"
#include "Common.h"

namespace C35
{

	template<typename T> class Asset
	{
	public:
		Asset(string);
		operator T& ();

	private:
		static std::map<string,T*> repository;

		T* me;
	};

}