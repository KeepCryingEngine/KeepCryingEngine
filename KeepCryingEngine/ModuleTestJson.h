#ifndef __MODULETESTJSON_H__
#define __MODULETESTJSON_H__

#include "Module.h"

class ModuleTestJson : public Module
{
public:

	ModuleTestJson();

	virtual ~ModuleTestJson();

	bool Start();
};

#endif