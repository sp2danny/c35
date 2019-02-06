
#pragma once

#include "Common.h"
#include "FrameSystem.h"

namespace C35
{
	struct TechTree;

	struct TechType
	{
		int Id;

		IdList requiresTech;
		IdList enablesResource;
		IdList enablesUnit;
		IdList enablesImprovement;
		IdList enablesGovernment;
		IdList enablesUnitAction;
		IdList givePerk;

		bool visible, given;

		int posX,posY;

		string Name;
		int cost;
		int era;
		bool rfea;
		enum { Sharp, Slope } ArrowStyle;

		string Description() const;

		static TechType& Lookup(int);
		static TechType& Lookup(string);
		static int NextID();
		static void Register(const TechType&);

		static int ToId(string);
		static IdList ByName(string);

		static void Load(string);
		static void Creator();

		static vector<TechType> const& All() ;

	private:

		static vector<TechType> techList;

	friend
		struct TechTree;
	};

	struct Surface;

	struct TechTree
	{
		IdList have;
		int working_on;
		int working_towards;

		bool Have(int);
		bool CanStart(int);

		void Start(int);
		void StartTowards(int);

		IdList PathTo(int);

		void Display(Surface& );

		void StartFresh();

		void Award(int t) { if(!Have(t)) have.push_back(t); }

		IdList Available();

	private:
		void path_internal( IdList& , int );
	};

	//extern void TechScreenStart(TechTree*,Player*);
	//extern void TechScreenDisplay();
	//extern bool TechScreenParseInput();

}
