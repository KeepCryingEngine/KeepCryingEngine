#include "Script.h"

#include "Application.h"
#include "GameObject.h"
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
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}

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

void Script::PreLoad(const nlohmann::json & json)
{
	Component::PreLoad(json);
	className = json["className"].get<string>();
	if(!className.empty())
	{
		App->script->SetClassToScript(*this, className);
		for(const nlohmann::json& fieldJson : json["fields"])
		{
			JsonToField(fieldJson);
		}
	}
}

void Script::JsonToField(const nlohmann::json& json)
{
	string label = json["name"].get<string>();
	for(MonoClassField* setField: fields)
	{
		if(strcmp(label.c_str(), mono_field_get_name(setField))==0)
		{
			MonoType * monoType = mono_field_get_type(setField);
			char * typeName = mono_type_get_name(monoType);
			if(strcmp(typeName, "System.Int32") == 0)//Int
			{
				int value = json["value"];
				mono_field_set_value(instance, setField, &value);
			}
			if(strcmp(typeName, "System.Single") == 0)//float
			{
				float value = json["value"];
				mono_field_set_value(instance, setField, &value);
			}
			if(strcmp(typeName, "System.Boolean") == 0)//Bool
			{
				bool value = json["value"].get<bool>();
				mono_field_set_value(instance, setField, &value);

			}
			if(strcmp(typeName, "System.String") == 0)//String
			{
				string value  = json["value"].get<string>();
				MonoString * tempVal = mono_string_new_wrapper(value.c_str());
				mono_field_set_value(instance, setField, tempVal);
			}
			if(strcmp(typeName, "KeepCryingEngine.GameObject") == 0)//Gameobject
			{

			}
		}
	}

}

void Script::Save(nlohmann::json & json) const
{
	Component::Save(json);
	json["className"] = className;
	nlohmann::json fieldsJson;
	for(MonoClassField* field : fields)
	{
		fieldsJson.push_back(FieldToJson(field));
	}
	json["fields"] = fieldsJson;
}

nlohmann::json Script::FieldToJson(MonoClassField* field)const
{
	nlohmann::json ret;
	MonoType * monoType = mono_field_get_type(field);
	char * typeName = mono_type_get_name(monoType);
	const char * label = mono_field_get_name(field);
	ret["name"] = label;
	if(strcmp(typeName, "System.Int32") == 0)//Int
	{
		int value = 0;
		mono_field_get_value(instance, field, &value);
		ret["value"] = value;
	}
	if(strcmp(typeName, "System.Single") == 0)//float
	{
		float value = 0;
		mono_field_get_value(instance, field, &value);
		ret["value"] = value;
	}
	if(strcmp(typeName, "System.Boolean") == 0)//Bool
	{
		bool value = false;
		mono_field_get_value(instance, field, &value);
		ret["value"] = value;

	}
	if(strcmp(typeName, "System.String") == 0)//String
	{
		static char value[256];
		MonoString * tempVal;
		mono_field_get_value(instance, field, &tempVal);
		strcpy_s(value, mono_string_to_utf8(tempVal));
		ret["value"] = value;
	}
	if(strcmp(typeName, "KeepCryingEngine.GameObject") == 0)//Gameobject
	{

	}
	return ret;
}

void Script::DrawField(MonoClassField* field)
{
	MonoType * monoType = mono_field_get_type(field);
	char * typeName = mono_type_get_name(monoType);
	const char * label = mono_field_get_name(field);
	if(strcmp(typeName,"System.Int32") == 0)//Int
	{
		int value = 0;
		mono_field_get_value(instance, field, &value );
		if(ImGui::DragInt(label, &value))
		{
			mono_field_set_value(instance,field,&value);
		}
	}
	if(strcmp(typeName, "System.Single") == 0)//float
	{
		float value = 0;
		mono_field_get_value(instance, field, &value);
		if(ImGui::DragFloat(label, &value,0.1f))
		{
			mono_field_set_value(instance, field, &value);
		}
	}
	if(strcmp(typeName, "System.Boolean") == 0)//Bool
	{
		bool value = false;
		mono_field_get_value(instance, field, &value);
		if(ImGui::Checkbox(label, &value))
		{
			mono_field_set_value(instance, field, &value);
		}
	}
	if(strcmp(typeName, "System.String") == 0)//String
	{
		static char value[256];
		MonoString * tempVal;
		mono_field_get_value(instance, field, &tempVal);
		strcpy_s(value,mono_string_to_utf8(tempVal));
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
