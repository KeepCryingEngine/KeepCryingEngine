#include "Script.h"

#include "Application.h"
#include "ModuleScript.h"

using namespace std;

Script::Script() : Component(TYPE)
{
	App->script->Subscribe(this);
}

Script::~Script()
{
	App->script->Unsubscribe(this);
}

void Script::DrawUI()
{
	if (ImGui::CollapsingHeader("Script"))
	{
		static char classNameCString[180] = "TranslateObjectOnUpdate";
		ImGui::InputText("##Class", classNameCString, sizeof(classNameCString));

		if (ImGui::Button("SetClass"))
		{
			className = classNameCString;
			App->script->SetClassToScript(*this, className);
		}

		for (MonoClassField* field : fields)
		{
			DrawField(field);
		}
	}
}

void Script::DrawField(MonoClassField* field)
{
	MonoType * monoType = mono_field_get_type(field);
	char * typeName = mono_type_get_name(monoType);
	LOG_DEBUG("%s", typeName);
}


void Script::SetScriptInstance(MonoObject * scriptInstance)
{
	this->instance = scriptInstance;
}

MonoObject * Script::GetScriptInstance() const
{
	return instance;
}

void Script::SetUpdateMethod(MonoMethod * updateMethod)
{
	this->updateMethod = updateMethod;
}

MonoMethod * Script::GetUpdateMethod() const
{
	return updateMethod;
}