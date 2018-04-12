#pragma once
#include "Module.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <set>
#include <string>

class Script;

class ModuleScript :
	public Module
{
public:
	ModuleScript();
	virtual ~ModuleScript();

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	void Subscribe(Script* script);
	void Unsubscribe(Script* script);

	void SetClassToScript(Script & script, const std::string &className);

private:
	void UpdateScript(Script* script);

private:
	MonoDomain * domain = nullptr;
	MonoAssembly * assembly = nullptr;
	MonoImage* image = nullptr;

	std::set<Script*> scripts;
};

