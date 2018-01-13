#ifndef _MODULETESTJSON_H_
#define _MODULETESTJSON_H_

#include "Module.h"

class ModuleTestJson : public Module
{
public:

	ModuleTestJson();

	virtual ~ModuleTestJson();

	bool Start();
};

#endif // !_MODULETESTJSON_H_