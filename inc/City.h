
#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "Common.h"
#include "Anim.h"

namespace C35
{

	struct ImprovementType
	{
		ImprovementType() {}

		int Id;

		string name;

		bool   visible           ;

		int    shields           ;

		bool   enableCity        ;
		bool   enableMetro       ;
		bool   enableLuxBonus    ;
		bool   enableNoPopPoll   ;
		bool   enableNoImpPoll   ;
		bool   enableSeaTrade    ;
		bool   enableAirTrade    ;

		bool   canHurry          ;
		bool   canTourist        ;
			  
		int    landXpBonus       ;
		int    seaXpBonus        ;
		int    airXpBonus        ;
			  
		int    cost              ;
		int    pollution         ;
		int    content           ;
		int    culture           ;

		float  wealthFactor      ;
		float  shieldsFactor     ;
		float  researchFactor    ;

		int    requiresImp       ;
		int    requiresTech      ;
		int    disablesImp       ;

		bool   needRiver         ;
		bool   needSea           ;

		bool   reqStrategic[16]  ;

		bool   oneOffCiv         ;
		bool   oneOffWorld       ;

		int    giveCivPerk       ;

		static ImprovementType& Lookup(int);
		static void Add(ImprovementType&);
		static int Count() { return list.size(); }
		static void Clear() { list.clear(); }

		static IdList ByName(string);
		static int ToId(string);

		AnimReflection Refl();
		BA& Icon();

		int icon_id;
		static AD icons;

	private:
		static vector<ImprovementType> list;
	
	};

	struct Laborer
	{
		int x,y;
		enum Action { wa_work, wa_happy, wa_gold, wa_research, wa_shields, wa_police } action;
	};

	struct CityImprovement
	{
		int imp_id;
		float effective_culture;
		int was_build;

	};

	struct City
	{
		std::string name;
		Player* owner;
		int x,y;
		bool building_unit;
		bool building_improvement;
		int build_id;
		int size;
		int foodbox;
		int shieldbox;
		int culture;

		int research_percentage;
		int happy_percentage;

		bool resources[16];
		void CalcRes();

		IdList AvailImp();
		IdList AvailUnit();

		enum CurrentlyBuilding { cbUnit, cbImprovement, cbWealth };
		CurrentlyBuilding currently_building;

		void Tick();
		int MaxFood();
		int MaxShield();

		bool Coastal();
		bool Capital();

		string BuildingName();

		int Influence();

		vector<Laborer> population;
		vector<CityImprovement> buildings;

		void AddImp(int);

		bool rallypoint;
		int rallypoint_x;
		int rallypoint_y;

		static void ToStream( City*, std::ostream& );
		static City* FromStream( std::istream& , Player* );

	};


	void ImprovementCreator();
	void ImprovementLoader(string);

	extern int river_id;

}