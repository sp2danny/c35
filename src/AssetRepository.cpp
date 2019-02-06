
#include "stdafx.h"

// --------------------------------------------

#include "AssetRepository.h"

using namespace std;

template <typename T>
map<string,T*> C35::Asset<T>::repository;

template <typename T>
C35::Asset<T>::Asset(string fn)
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


void Dummy()
{
	using namespace C35;
	CIS& cis = Asset<CIS>  ( "TemplateInstanciate1" );
	AC&  ac  = Asset<AC >  ( "TemplateInstanciate2" );
	AD&  ad  = Asset<AD >  ( "TemplateInstanciate3" );
	BA&  ba  = Asset<BA >  ( "TemplateInstanciate4" );
}

