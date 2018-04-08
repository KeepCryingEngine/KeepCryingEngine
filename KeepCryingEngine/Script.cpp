#include "Script.h"

#include "Application.h"
#include "ModuleScript.h"

#include <mono/jit/jit.h>
#include <mono/metadata/object.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

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
	if(strcmp(typeName,"System.Int32") == 0)//Int
	{
		int value = 0;
		mono_field_get_value(instance, field, &value );
		const char * label = mono_field_get_name(field);
		if(ImGui::DragInt(label, &value))
		{
			mono_field_set_value(instance,field,&value);
		}
	}
	if(strcmp(typeName, "System.Single") == 0)//float
	{
		float value = 0;
		mono_field_get_value(instance, field, &value);
		const char * label = mono_field_get_name(field);
		if(ImGui::DragFloat(label, &value))
		{
			mono_field_set_value(instance, field, &value);
		}
	}
	if(strcmp(typeName, "System.Boolean") == 0)//Bool
	{
		bool value = false;
		mono_field_get_value(instance, field, &value);
		const char * label = mono_field_get_name(field);
		if(ImGui::Checkbox(label, &value))
		{
			mono_field_set_value(instance, field, &value);
		}
	}
	//if(strcmp(typeName, "System.Char") == 0)//Char
	//{
	//	static char value = 'A';
	//	mono_field_get_value(instance, field, (void*)&value);
	//	const char * label = mono_field_get_name(field);
	//	ImGui::InputText(label,&value,1);
	//}
	if(strcmp(typeName, "System.String") == 0)//String
	{
		static char value[256];
		MonoString * tempVal;
		mono_field_get_value(instance, field, &tempVal);
		strcpy(value,mono_string_to_utf8(tempVal));
		const char * label = mono_field_get_name(field);
		if(ImGui::InputText(label, value, 256))
		{
			tempVal = mono_string_new_wrapper(value);
			mono_field_set_value(instance, field, tempVal);
		}
	}
	if(strcmp(typeName, "KeepCryingEngine.GameObject") == 0)//Gameobject
	{

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

void Script::AddField(MonoClassField * field)
{
	fields.insert(field);
}
