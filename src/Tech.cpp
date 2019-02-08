
#include "stdafx.h"

// --------------------------------------------

#include "Tech.h"

#include "Unit.h"
#include "City.h"
#include "Anim.h"
#include "Resource.h"

using namespace std;

// ****************
// *** TechTree ***
// ****************

bool C35::TechTree::Have(int t)
{
	auto itr = find( have.begin(), have.end(), t );
	return ( itr != have.end() ) ;

	/*
	for( int tt : have )
		if(t==tt)
			return true;
	return false;
	*/
}

bool C35::TechTree::CanStart(int t)
{
	TechType& tt = TechType::Lookup(t);
	for( int r : tt.requiresTech )
	{
		if(!Have(r)) return false;
	}
	return true;
}

void C35::TechTree::Start(int t)
{
	assert( CanStart(t) );
	working_on = working_towards = t;
}

void C35::TechTree::StartTowards(int t)
{
	IdList path = PathTo(t);
	if(path.empty()) return;
	working_on=path[0];
	working_towards=t;
}

C35::IdList C35::TechTree::PathTo(int t)
{
	IdList list; list.clear();
	if( Have(t) ) return list;
	if( CanStart(t) )
	{
		list.push_back(t);
		return list;
	}
	TechTree copy = *this;
	copy.path_internal(list,t);
	return list;
}

void C35::TechTree::Display(Surface& s)
{
}

void C35::TechTree::StartFresh()
{
	have.clear();
	working_on=working_towards=0;
}

C35::IdList C35::TechTree::Available()
{
	IdList list;

	for(TechType& tt : TechType::techList)
	{
		if( Have(tt.Id) ) continue;
		if( CanStart(tt.Id) )
			list.push_back(tt.Id);
	}
	return list;
}

void C35::TechTree::path_internal( IdList& list, int t)
{
	if( CanStart(t) )
	{
		list.push_back(t);
		have.push_back(t);
		return;
	} else {
		TechType& tt = TechType::Lookup(t);
		for(int r : tt.requiresTech)
		{
			path_internal(list,r);
		}
		assert( CanStart(t) );
		list.push_back(t);
		have.push_back(t);
		return;
	}
}

// ****************
// *** TechType ***
// ****************

vector<C35::TechType> C35::TechType::techList;

C35::TechType& C35::TechType::Lookup(int id)
{
	assert( id >= 1 );
	assert( id <= techList.size() );
	assert( id == techList[id-1].Id );
	return techList[id-1];
}

C35::TechType& C35::TechType::Lookup(string name)
{
	for( TechType& tt : techList )
		if( tt.Name == name )
			return tt;
	assert(false);
	throw "no such tech";
}

int C35::TechType::NextID()
{
	return techList.size() + 1;
}

void C35::TechType::Register(const TechType& tt)
{
	assert( tt.Id == NextID() );
	techList.push_back(tt);
}

void C35::TechType::Load(string fn)
{
}

int C35::TechType::ToId(string name)
{
	for( TechType& tt : techList )
		if( tt.Name == name )
			return tt.Id;
	return 0;
}

C35::IdList C35::TechType::ByName(string name)
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

const char* resN[] = { "", "Horses", "Iron" };

string C35::TechType::Description() const
{
	string dd;
	dd += Name + "\n";
	int i,n;

	if( (n=requiresTech.size()) )
	{
		dd += "Requires ";
		for(i=0;i<n;++i)
		{
			dd += Lookup(requiresTech[i]).Name;
			/**/ if( i < (n-2) ) dd += ", ";
			else if( i < (n-1) ) dd += " and ";
		}
		dd += " to research.\n";
	}

	if( (n=enablesResource.size()) )
	{
		dd += "New resource";
		if(n>1) dd+="s";
		dd += " ";
		for(i=0;i<n;++i)
		{
			dd += resN[ enablesResource[i] ];
			/**/ if( i < (n-2) ) dd += ", ";
			else if( i < (n-1) ) dd += " and ";
		}
		dd += " appears on the map.\n";
	}

	if( (n=enablesUnit.size()) )
	{
		dd += "New unit";
		if(n>1) dd+="s";
		dd += " ";
		for(i=0;i<n;++i)
		{
			dd += UnitType::Lookup( enablesUnit[i] ).name;
			/**/ if( i < (n-2) ) dd += ", ";
			else if( i < (n-1) ) dd += " and ";
		}
		dd += " are now available.\n";
	}

	if( (n=enablesImprovement.size()) )
	{
		dd += "New city improvment";
		if(n>1) dd+="s";
		dd += " ";
		for(i=0;i<n;++i)
		{
			dd += ImprovementType::Lookup(enablesImprovement[i]).name;
			/**/ if( i < (n-2) ) dd += ", ";
			else if( i < (n-1) ) dd += " and ";
		}
		dd += " are now available.\n";
	}

	if( ! rfea ) {
		dd += "Note: you dont have to research this tech to advance to the next era\n";
	}

	return dd;
}

vector<C35::TechType> const& C35::TechType::All()
{
	return techList;
}

#define UTBN(x) UnitType::ByName(x)
#define ITBN(x) ImprovementType::ByName(x)
#define TTBN(x) ByName(x)
#define RTBN(x) Resource::FromName(x)

void C35::TechType::Creator()
{
	techList.clear();

	{
		TechType BronzeWorking;

		BronzeWorking.Id = NextID();

		BronzeWorking.requiresTech       . clear();
		BronzeWorking.enablesResource    . clear();
		BronzeWorking.enablesUnit        = UTBN("Spearman");
		BronzeWorking.enablesImprovement = ITBN("The Colossus");
		BronzeWorking.enablesGovernment  . clear();
		BronzeWorking.enablesUnitAction  . clear();
		BronzeWorking.givePerk           . clear();

		BronzeWorking.visible = true;
		BronzeWorking.given   = false;

		BronzeWorking.posX = 88;
		BronzeWorking.posY = 83;

		BronzeWorking.Name = "Bronze Working";
		BronzeWorking.cost = 10;
		BronzeWorking.era  = 1;
		BronzeWorking.rfea = true;
		BronzeWorking.ArrowStyle = Slope;

		Register(BronzeWorking);
	}

	{
		TechType Masonry;

		Masonry.Id = NextID();

		Masonry.requiresTech       . clear();
		Masonry.enablesResource    . clear();
		Masonry.enablesUnit        . clear();
		Masonry.enablesImprovement = ITBN("Palace,Walls,The Pyramids");
		Masonry.enablesGovernment  . clear();
		Masonry.enablesUnitAction  . clear(); // ATBN("Outpost");
		Masonry.givePerk           . clear();

		Masonry.visible = true;
		Masonry.given   = false;

		Masonry.posX = 91;
		Masonry.posY = 193;

		Masonry.Name = "Masonry";
		Masonry.cost = 10;
		Masonry.era  = 1;
		Masonry.rfea = true;
		Masonry.ArrowStyle = Slope;

		Register(Masonry);
	}

	{
		TechType Alphabet;

		Alphabet.Id = NextID();

		Alphabet.requiresTech       . clear();
		Alphabet.enablesResource    . clear();
		Alphabet.enablesUnit        = UTBN("Curragh");
		Alphabet.enablesImprovement . clear();
		Alphabet.enablesGovernment  . clear();
		Alphabet.enablesUnitAction  . clear();
		Alphabet.givePerk           . clear();

		Alphabet.visible = true;
		Alphabet.given   = false;

		Alphabet.posX = 93;
		Alphabet.posY = 304;

		Alphabet.Name = "Alphabet";
		Alphabet.cost = 10;
		Alphabet.era  = 1;
		Alphabet.rfea = true;
		Alphabet.ArrowStyle = Sharp;

		Register(Alphabet);
	}

	{
		TechType Pottery;

		Pottery.Id = NextID();

		Pottery.requiresTech       . clear();
		Pottery.enablesResource    . clear();
		Pottery.enablesUnit        . clear();
		Pottery.enablesImprovement = ITBN("Granary");
		Pottery.enablesGovernment  . clear();
		Pottery.enablesUnitAction  . clear();
		Pottery.givePerk           . clear();

		Pottery.visible = true;
		Pottery.given   = false;

		Pottery.posX = 93;
		Pottery.posY = 405;

		Pottery.Name = "Pottery";
		Pottery.cost = 10;
		Pottery.era  = 1;
		Pottery.rfea = true;
		Pottery.ArrowStyle = Slope;

		Register(Pottery);
	}

	{
		TechType TheWheel;

		TheWheel.Id = NextID();

		TheWheel.requiresTech       . clear();
		TheWheel.enablesResource    = RTBN("Horses");
		TheWheel.enablesUnit        = UTBN("Chariot");
		TheWheel.enablesImprovement . clear();
		TheWheel.enablesGovernment  . clear();
		TheWheel.enablesUnitAction  . clear();
		TheWheel.givePerk           . clear();

		TheWheel.visible = true;
		TheWheel.given   = false;

		TheWheel.posX = 93;
		TheWheel.posY = 488;

		TheWheel.Name = "The Wheel";
		TheWheel.cost = 10;
		TheWheel.era  = 1;
		TheWheel.rfea = true;
		TheWheel.ArrowStyle = Slope;

		Register(TheWheel);
	}

	{
		TechType WarriorCode;

		WarriorCode.Id = NextID();

		WarriorCode.requiresTech       . clear();
		WarriorCode.enablesResource    . clear();
		WarriorCode.enablesUnit        = UTBN("Archer");
		WarriorCode.enablesImprovement . clear();
		WarriorCode.enablesGovernment  . clear();
		WarriorCode.enablesUnitAction  . clear();
		WarriorCode.givePerk           . clear();

		WarriorCode.visible = true;
		WarriorCode.given   = false;

		WarriorCode.posX = 93;
		WarriorCode.posY = 569;

		WarriorCode.Name = "Warrior Code";
		WarriorCode.cost = 10;
		WarriorCode.era  = 1;
		WarriorCode.rfea = true;
		WarriorCode.ArrowStyle = Slope;

		Register(WarriorCode);
	}

	{
		TechType CeremonialBurial;

		CeremonialBurial.Id = NextID();

		CeremonialBurial.requiresTech       . clear();
		CeremonialBurial.enablesResource    . clear();
		CeremonialBurial.enablesUnit        . clear();
		CeremonialBurial.enablesImprovement = ITBN("Temple");
		CeremonialBurial.enablesGovernment  . clear();
		CeremonialBurial.enablesUnitAction  . clear();
		CeremonialBurial.givePerk           . clear();

		CeremonialBurial.visible = true;
		CeremonialBurial.given   = false;

		CeremonialBurial.posX = 93;
		CeremonialBurial.posY = 652;

		CeremonialBurial.Name = "Ceremonial Burial";
		CeremonialBurial.cost = 10;
		CeremonialBurial.era  = 1;
		CeremonialBurial.rfea = true;
		CeremonialBurial.ArrowStyle = Slope;

		Register(CeremonialBurial);
	}

	{
		TechType IronWorking;

		IronWorking.Id = NextID();

		IronWorking.requiresTech       = TTBN("Bronze Working");
		IronWorking.enablesResource    = RTBN("Iron");
		IronWorking.enablesUnit        = UTBN("Pikeman,Swordman");
		IronWorking.enablesImprovement . clear();
		IronWorking.enablesGovernment  . clear();
		IronWorking.enablesUnitAction  . clear();
		IronWorking.givePerk           . clear();

		IronWorking.visible = true;
		IronWorking.given   = false;

		IronWorking.posX = 287;
		IronWorking.posY = 90;

		IronWorking.Name = "Iron Working";
		IronWorking.cost = 10;
		IronWorking.era  = 1;
		IronWorking.rfea = true;
		IronWorking.ArrowStyle = Slope;

		Register(IronWorking);
	}

	{
		TechType Mathematics;

		Mathematics.Id = NextID();

		Mathematics.requiresTech       = TTBN("Masonry,Alphabet");
		Mathematics.enablesResource    . clear();
		Mathematics.enablesUnit        = UTBN("Catapult");
		Mathematics.enablesImprovement = ITBN("Statue of Zeuves");
		Mathematics.enablesGovernment  . clear();
		Mathematics.enablesUnitAction  . clear();
		Mathematics.givePerk           . clear();

		Mathematics.visible = true;
		Mathematics.given   = false;

		Mathematics.posX = 333;
		Mathematics.posY = 183;

		Mathematics.Name = "Mathematics";
		Mathematics.cost = 10;
		Mathematics.era  = 1;
		Mathematics.rfea = true;
		Mathematics.ArrowStyle = Slope;

		Register(Mathematics);
	}

	{
		TechType Writing;

		Writing.Id = NextID();

		Writing.requiresTech       = TTBN("Alphabet");
		Writing.enablesResource    . clear();
		Writing.enablesUnit        . clear();
		Writing.enablesImprovement . clear();
		Writing.enablesGovernment  . clear();
		Writing.enablesUnitAction  . clear();
		Writing.givePerk           . push_back(1);

		Writing.visible = true;
		Writing.given   = false;

		Writing.posX = 266;
		Writing.posY = 346;

		Writing.Name = "Writing";
		Writing.cost = 10;
		Writing.era  = 1;
		Writing.rfea = true;
		Writing.ArrowStyle = Sharp;

		Register(Writing);
	}

	{
		TechType Mysticism;

		Mysticism.Id = NextID();

		Mysticism.requiresTech       = TTBN("Ceremonial Burial");
		Mysticism.enablesResource    . clear();
		Mysticism.enablesUnit        . clear();
		Mysticism.enablesImprovement = ITBN("The Oracle");
		Mysticism.enablesGovernment  . clear();
		Mysticism.enablesUnitAction  . clear();
		Mysticism.givePerk           . clear();

		Mysticism.visible = true;
		Mysticism.given   = false;

		Mysticism.posX = 284;
		Mysticism.posY = 647;

		Mysticism.Name = "Mysticism";
		Mysticism.cost = 10;
		Mysticism.era  = 1;
		Mysticism.rfea = true;
		Mysticism.ArrowStyle = Slope;

		Register(Mysticism);
	}

	{
		TechType Philosophy;

		Philosophy.Id = NextID();

		Philosophy.requiresTech       = TTBN("Writing");
		Philosophy.enablesResource    . clear();
		Philosophy.enablesUnit        . clear();
		Philosophy.enablesImprovement = ITBN("Mausoleum of Mausollos");
		Philosophy.enablesGovernment  . clear();
		Philosophy.enablesUnitAction  . clear();
		Philosophy.givePerk           . clear();

		Philosophy.visible = true;
		Philosophy.given   = false;

		Philosophy.posX = 508;
		Philosophy.posY = 231;

		Philosophy.Name = "Philosophy";
		Philosophy.cost = 10;
		Philosophy.era  = 1;
		Philosophy.rfea = true;
		Philosophy.ArrowStyle = Sharp;

		Register(Philosophy);
	}

	{
		TechType CodeOfLaws;

		CodeOfLaws.Id = NextID();

		CodeOfLaws.requiresTech       = TTBN("Writing");
		CodeOfLaws.enablesResource    . clear();
		CodeOfLaws.enablesUnit        . clear();
		CodeOfLaws.enablesImprovement = ITBN("Courthouse");
		CodeOfLaws.enablesGovernment  . clear();
		CodeOfLaws.enablesUnitAction  . clear();
		CodeOfLaws.givePerk           . clear();

		CodeOfLaws.visible = true;
		CodeOfLaws.given   = false;

		CodeOfLaws.posX = 478;
		CodeOfLaws.posY = 313;

		CodeOfLaws.Name = "Code of Laws";
		CodeOfLaws.cost = 10;
		CodeOfLaws.era  = 1;
		CodeOfLaws.rfea = true;
		CodeOfLaws.ArrowStyle = Sharp;

		Register(CodeOfLaws);
	}

	{
		TechType Literature;

		Literature.Id = NextID();

		Literature.requiresTech       = TTBN("Writing");
		Literature.enablesResource    . clear();
		Literature.enablesUnit        . clear();
		Literature.enablesImprovement = ITBN("Library,The Great Library");
		Literature.enablesGovernment  . clear();
		Literature.enablesUnitAction  . clear();
		Literature.givePerk           . clear();

		Literature.visible = true;
		Literature.given   = false;

		Literature.posX = 476;
		Literature.posY = 391;

		Literature.Name = "Literature";
		Literature.cost = 10;
		Literature.era  = 1;
		Literature.rfea = false;
		Literature.ArrowStyle = Sharp;

		Register(Literature);
	}

	{
		TechType MapMaking;

		MapMaking.Id = NextID();

		MapMaking.requiresTech       = TTBN("Pottery,Writing");
		MapMaking.enablesResource    . clear();
		MapMaking.enablesUnit        = UTBN("Galley,Fregatte");
		MapMaking.enablesImprovement = ITBN("Harbour,The Great Lighthouse");
		MapMaking.enablesGovernment  . clear();
		MapMaking.enablesUnitAction  . clear();
		MapMaking.givePerk           . push_back( 2 );

		MapMaking.visible = true;
		MapMaking.given   = false;

		MapMaking.posX = 476;
		MapMaking.posY = 470;

		MapMaking.Name = "Map Making";
		MapMaking.cost = 10;
		MapMaking.era  = 1;
		MapMaking.rfea = true;
		MapMaking.ArrowStyle = Sharp;

		Register(MapMaking);
	}

	{
		TechType HorsebackRiding;

		HorsebackRiding.Id = NextID();

		HorsebackRiding.requiresTech       = TTBN("The Wheel,Warrior Code");
		HorsebackRiding.enablesResource    . clear();
		HorsebackRiding.enablesUnit        = UTBN("Horseman");
		HorsebackRiding.enablesImprovement . clear();
		HorsebackRiding.enablesGovernment  . clear();
		HorsebackRiding.enablesUnitAction  . clear();
		HorsebackRiding.givePerk           . clear();

		HorsebackRiding.visible = true;
		HorsebackRiding.given   = false;

		HorsebackRiding.posX = 476;
		HorsebackRiding.posY = 554;

		HorsebackRiding.Name = "Horseback Riding";
		HorsebackRiding.cost = 10;
		HorsebackRiding.era  = 1;
		HorsebackRiding.rfea = true;
		HorsebackRiding.ArrowStyle = Sharp;

		Register(HorsebackRiding);
	}

	{
		TechType Polytheism;

		Polytheism.Id = NextID();

		Polytheism.requiresTech       = TTBN("Mysticism");
		Polytheism.enablesResource    . clear();
		Polytheism.enablesUnit        . clear();
		Polytheism.enablesImprovement = ITBN("The Hanging Gardens");
		Polytheism.enablesGovernment  . clear();
		Polytheism.enablesUnitAction  . clear();
		Polytheism.givePerk           . clear();

		Polytheism.visible = true;
		Polytheism.given   = false;

		Polytheism.posX = 476;
		Polytheism.posY = 652;

		Polytheism.Name = "Polytheism";
		Polytheism.cost = 10;
		Polytheism.era  = 1;
		Polytheism.rfea = true;
		Polytheism.ArrowStyle = Sharp;

		Register(Polytheism);
	}

	{
		TechType Construction;

		Construction.Id = NextID();

		Construction.requiresTech       = TTBN("Iron Working,Mathematics");
		Construction.enablesResource    . clear();
		Construction.enablesUnit        . clear();
		Construction.enablesImprovement = ITBN("Aqueduct,Colloseum,The Great Wall");
		Construction.enablesGovernment  . clear();
		Construction.enablesUnitAction  . clear(); // ATBN("Fortress");
		Construction.givePerk           . clear();

		Construction.visible = true;
		Construction.given   = false;

		Construction.posX = 619;
		Construction.posY = 84;

		Construction.Name = "Construction";
		Construction.cost = 10;
		Construction.era  = 1;
		Construction.rfea = true;
		Construction.ArrowStyle = Sharp;

		Register(Construction);
	}

	{
		TechType Currency;

		Currency.Id = NextID();

		Currency.requiresTech       = TTBN("Mathematics");
		Currency.enablesResource    . clear();
		Currency.enablesUnit        . clear();
		Currency.enablesImprovement = ITBN("Marketplace");
		Currency.enablesGovernment  . clear();
		Currency.enablesUnitAction  . clear();
		Currency.givePerk           . clear();

		Currency.visible = true;
		Currency.given   = false;

		Currency.posX = 644;
		Currency.posY = 179;

		Currency.Name = "Currency";
		Currency.cost = 10;
		Currency.era  = 1;
		Currency.rfea = true;
		Currency.ArrowStyle = Sharp;

		Register(Currency);
	}

	{
		TechType TheRepublic;

		TheRepublic.Id = NextID();

		TheRepublic.requiresTech       = TTBN("Philosophy,Code of Laws");
		TheRepublic.enablesResource    . clear();
		TheRepublic.enablesUnit        . clear();
		TheRepublic.enablesImprovement . clear();
		TheRepublic.enablesGovernment  . clear(); // GTBN("The Republic");
		TheRepublic.enablesUnitAction  . clear();
		TheRepublic.givePerk           . clear();

		TheRepublic.visible = true;
		TheRepublic.given   = false;

		TheRepublic.posX = 687;
		TheRepublic.posY = 293;

		TheRepublic.Name = "The Republic";
		TheRepublic.cost = 10;
		TheRepublic.era  = 1;
		TheRepublic.rfea = false;
		TheRepublic.ArrowStyle = Sharp;

		Register(TheRepublic);
	}

	{
		TechType Monarchy;

		Monarchy.Id = NextID();

		Monarchy.requiresTech       = TTBN("Warrior Code,Polytheism");
		Monarchy.enablesResource    . clear();
		Monarchy.enablesUnit        . clear();
		Monarchy.enablesImprovement = ITBN("The Hanging Gardens");
		Monarchy.enablesGovernment  . clear();
		Monarchy.enablesUnitAction  . clear();
		Monarchy.givePerk           . clear();

		Monarchy.visible = true;
		Monarchy.given   = false;

		Monarchy.posX = 689;
		Monarchy.posY = 615;

		Monarchy.Name = "Monarchy";
		Monarchy.cost = 10;
		Monarchy.era  = 1;
		Monarchy.rfea = false;
		Monarchy.ArrowStyle = Sharp;

		Register(Monarchy);
	}

}
