
#include <string>
#include <vector>

#include "Common.h"
#include "Anim.h"

namespace C35
{

	namespace Resource
	{
		enum Id { none=0, Begin, Horses=Begin, Iron, End };

		IdList FromName(std::string);

		void Maker();

		CIS& Icon(int);
		std::string Name(int);

	}

}
