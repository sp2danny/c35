
#include "stdafx.h"

// --------------------------------------------

#include "Resource.h"

#include "Anim.h"

namespace C35 { namespace Resource {

	struct Res
	{
		int id;
		std::string name;
		CIS icon;
	};

	std::vector<Res> resources;

	int ToId(std::string);

}}

int C35::Resource::ToId(std::string n)
{
	for( Res& r : resources )
	{
		if( n == r.name ) return r.id;
	}
	return 0;
}

C35::IdList C35::Resource::FromName(std::string name)
{
	IdList vi;
	while(true)
	{
		if(name.empty()) break; 
		int p = name.find(',');
		int id = ToId( name.substr(0,p) );
		if(id)
			vi.push_back(id);
		if(p==string::npos) break;
		name = name.substr( p+1 );
	}
	return vi;
}


C35::CIS& C35::Resource::Icon(int id)
{
	return resources[id].icon;
}

std::string C35::Resource::Name(int id)
{
	return resources[id].name;
}

void C35::Resource::Maker()
{
	resources.resize(3);

	{
		Res& horses = resources[Horses];
		horses.id = Horses;
		horses.name = "Horses";
		horses.icon.LoadExt("gfx/res_1.bmp");
	}

	{
		Res& iron = resources[Iron];
		iron.id = Iron;
		iron.name = "Iron";
		iron.icon.LoadExt("gfx/res_2.bmp");
	}

	for( int id = Begin ; id != End; ++id)
	{
		#ifdef _DEBUG
			resources[id].icon.Skip3();
		#endif
		resources[id].icon.Hot(Pos(-3,-3));
		resources[id].icon.Instance(0);
	}
}


