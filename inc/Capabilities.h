
#pragma once

#include "Common.h"

#include <vector>

namespace C35
{

	class Capabilities
	{
	public:
		virtual bool CanMove           () { return false; }
		virtual bool CanAttack         () { return false; }
		virtual bool CanDefend         () { return false; }
		virtual bool CaptureOnLoss     () { return false; }
		virtual bool CanBombard        () { return false; }
		virtual bool CanAirdrop        () { return false; }
		virtual bool CanDefendBombard  () { return false; }
		virtual bool CanBuild          () { return false; }
		virtual bool CanMoveCity       () { return false; }
		virtual bool CanMoveLand       () { return false; }
		virtual bool CanMoveWater      () { return false; }
		virtual bool CanSettle         () { return false; }
		virtual bool CanJoin           () { return false; }
		virtual bool CanColony         () { return false; }
		virtual bool CanFortify        () { return false; }

		virtual Capabilities* Clone () { return new Capabilities(); }

		static Capabilities& Default() { static Capabilities def; return def; }

		//Capabilities(const Capabilities&) = default;
	};

	class BasicTroop : public Capabilities
	{
	public:
		virtual bool CanMove      () { return true; }
		virtual bool CanAttack    () { return true; }
		virtual bool CanDefend    () { return true; }
		virtual bool CanMoveCity  () { return true; }
		virtual bool CanMoveLand  () { return true; }
		virtual bool CanFortify   () { return true; }

		virtual Capabilities* Clone () { return new BasicTroop(); }
	};

	class VarCapa : public Capabilities
	{
	public:
		virtual bool CanMove           () { return canMove          ; }
		virtual bool CanAttack         () { return canAttack        ; }
		virtual bool CanDefend         () { return canDefend        ; }
		virtual bool CaptureOnLoss     () { return captureOnLoss    ; }
		virtual bool CanBombard        () { return canBombard       ; }
		virtual bool CanAirdrop        () { return canAirdrop       ; }
		virtual bool CanDefendBombard  () { return canDefendBombard ; }
		virtual bool CanBuild          () { return canBuild         ; }
		virtual bool CanMoveCity       () { return canMoveCity      ; }
		virtual bool CanMoveLand       () { return canMoveLand      ; }
		virtual bool CanMoveWater      () { return canMoveWater     ; }
		virtual bool CanSettle         () { return canSettle        ; }
		virtual bool CanJoin           () { return canJoin          ; }
		virtual bool CanColony         () { return canColony        ; }
		virtual bool CanFortify        () { return canFortify       ; }

		virtual Capabilities* Clone () { return new VarCapa(*this); }
	//protected:
		bool canMove, canAttack, canDefend, captureOnLoss, canBombard, canAirdrop;
		bool canDefendBombard, canBuild, canMoveCity, canMoveLand, canMoveWater;
		bool canSettle, canJoin, canColony, canFortify;
	};

	
}


