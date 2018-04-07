#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include "Component.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <string>
#include <set>

class Script :
	public Component
{
public:
	static const Component::Type TYPE = Component::Type::Script;

	Script();
	virtual ~Script();

	void DrawUI() override;


	void SetScriptInstance(MonoObject * scriptInstance);
	MonoObject * GetScriptInstance() const;

	void SetUpdateMethod(MonoMethod * updateMethod);
	MonoMethod * GetUpdateMethod() const;

private:
	void DrawField(MonoClassField* field);

private:
	std::string className;
	MonoObject * instance = nullptr;
	MonoMethod * updateMethod = nullptr;

	std::set<MonoClassField*> fields;
};

#endif