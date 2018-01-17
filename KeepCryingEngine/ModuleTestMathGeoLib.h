#ifndef _MODULETESTMATHGEOLIB_H_
#define _MODULETESTMATHGEOLIB_H_

#include "Module.h"

class ModuleTestMathGeoLib : public Module
{
public:

	ModuleTestMathGeoLib();

	virtual ~ModuleTestMathGeoLib();

	bool Start() override;
};

#endif // !_MODULETESTMATHGEOLIB_H_