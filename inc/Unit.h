
#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <map>

#include "Common.h"

#include "Capabilities.h"
#include "Anim.h"
#include "GameEngine.h"
#include "Player.h"

namespace C35
{
	struct Capa {
		bool pillage  : 1;
		bool bombard  : 1;
		bool airdrop  : 1;
		bool def_bomb : 1;
		bool worker   : 1;
		bool settle   : 1;
		void Clear() { pillage=bombard=airdrop=def_bomb=worker=settle=false; }
		Capa() : pillage(false), bombard(false), airdrop(false), def_bomb(false), worker(false), settle(false) {}
		Capa(bool p,bool b,bool a,bool d,bool w,bool s) : pillage(p), bombard(b), airdrop(a), def_bomb(d), worker(w), settle(s) {}
	};

	struct Stats
	{
		int xp,lev;
		int hp, maxhp;
		int off,def;
		int move,maxmove;
		int bombard_str;
		int bombard_range;
		Capa oneoffs;
	};

	struct Movement
	{
		bool land;
		bool sea;
		int movement;
		int remaining;
	};

	void UnitCreator();

	struct UnitType
	{
		int id;
		enum { Off, Def, Bmb, Trn, None } basetype;
		enum { Foot, Mounted, Vehicle, Sea, Air } kind;
		enum { NoPerk, OpenTerrainCombat, MountedDefense, MeleeCombat, FortifiedDefense, CityAssault, CityDefense, HigherGround, VehicleDefense } perk;
		int start_off, start_def, start_bmb;
		int range, move;
		int join_num;
		int load_capa;
		std::string name;
		Capa capa;
		int cost;
		int res;
		bool available;
		int icon_id;

		void LoadBmp(string);
		void LoadAD(string);
		void LoadAC(string);
		void LoadLate(string);
		AnimReflection GetRefl(string,short,Player*);
		AnimReflection GetIcon(UC);
		AnimReflection GetIcon(Player* p) { return GetIcon(p->color); }
		BA& GetIcon();
		void Instance(Player*);
		void MakeWhite();
		void DoneImg();
		void Scale150();

		static UnitType& New();
		static UnitType& Lookup(int);
		static int Size();
		static void Clear();

		static IdList ByName(string);
		static int ToId(string);

		static void DoneAll();
		static void HaveAllNow();

		static Unit* MakeNewFromType(int,Player*);
		static Unit* MakeConscriptFromType(int,Player*);


	private:
		UnitType() {}
		//UnitType(const UnitType& ut) { }
		static int nextId;
		static std::vector<UnitType> types;
		static AD unit_icons;


		AnimCollection ac;
		AnimDir ad;
		CIS cis;
		enum { have_name, have_ac, have_ad, have_cis, have_none } have_gfx;
		string gfxname;

	friend
		void UnitCreator();


	friend
		struct Unit;
	};

	enum class UnitId { none, worker, settler, warrior, curragh, horseman, pikeman, galley, fregatte, barbarian, max };
	extern UnitType*     worker_utp ;
	extern UnitType*    settler_utp ;
	extern UnitType*    warrior_utp ;
	extern UnitType*    curragh_utp ;
	extern UnitType*   horseman_utp ;
	extern UnitType*    pikeman_utp ;
	extern UnitType*     galley_utp ;
	extern UnitType*   fregatte_utp ;
	extern UnitType*  barbarian_utp ;

	struct Unit
	{
		int id;
		int type;

		Stats stats;

		Orders* orders;

		Player* owner;

		bool currently;

		void ClearForTurn();
		void Won();

		static int NewID();
		static void Register(int,Unit*);
		static Unit* Lookup(int);
		static void ClearAll();

		Capabilities& Capa();

		//static Unit* MakeNewFromType(int,Player*);

		bool WantsOrder();
		bool AcceptsOrder();
		void ExecuteOrder(Board*);
		void AbortOrder();

		bool CanLoad(const Unit*) const;
		void DoLoad(Unit*);
		bool CanUnload(int,int) const;
		bool CanUnload(Dir6) const;
		void DoUnload(int,int);
		void DoUnload(Dir6);

		bool CanGo(Dir6) const;
		void DoGo(Dir6);

		Movement MakeMovement();

		bool CanCity();

		void KillUnit();

		void Attack(Dir6);

		void Display(int,int);
		void SetAnim(string,int);
		void Update(int);
		AnimReflection GetIcon();

		int x,y;

		bool visible;

		static Unit* FromStream(istream&,Player*);
		void ToStream(ostream&);

		vector<Unit*> loaded;
		vector<UC> perks;

	friend
		struct UnitType;

	private:
		void OrderDone(bool=false);
		void AutoPilot();
		static int nextId;
		static std::vector<Unit*> units;
		VarCapa capa;
		char buffer[256];
		AnimReflection ar;
	};

}