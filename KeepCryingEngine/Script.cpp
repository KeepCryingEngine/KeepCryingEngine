#include "Script.h"

#include "Application.h"
#include "ModuleScript.h"

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
	}
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