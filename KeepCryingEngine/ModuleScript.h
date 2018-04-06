#pragma once
#include "Module.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

class ModuleScript :
	public Module
{
public:
	ModuleScript();
	virtual ~ModuleScript();

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

private:
	MonoDomain * domain = nullptr;
	MonoAssembly * assembly = nullptr;
	MonoImage* image = nullptr;
};

