
#include "stdafx.h"

// --------------------------------------------

#include "AssetRepository.h"

using namespace std;

/*
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
*/

void Dummy()
{
	using namespace C35;
	[[maybe_unused]] CIS& cis = Asset<CIS>  ( "TemplateInstanciate1" );
	[[maybe_unused]] AC&  ac  = Asset<AC >  ( "TemplateInstanciate2" );
	[[maybe_unused]] AD&  ad  = Asset<AD >  ( "TemplateInstanciate3" );
	[[maybe_unused]] BA&  ba  = Asset<BA >  ( "TemplateInstanciate4" );
}

