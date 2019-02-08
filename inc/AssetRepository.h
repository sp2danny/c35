
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

template <typename T>
std::map<std::string,T*> C35::Asset<T>::repository;

template <typename T>
C35::Asset<T>::Asset(std::string fn)
{
	auto itr = repository.find(fn);
	if( itr != repository.end() )
	{
		me = itr->second;
		return;
	}

	T* t = new T();
	t->LoadExt(fn);

	repository[fn] = t;

	me=t;
}

template <typename T>
C35::Asset<T>::operator T& ()
{
	return *me;
}


