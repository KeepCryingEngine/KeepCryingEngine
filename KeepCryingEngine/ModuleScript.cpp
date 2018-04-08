#include "ModuleScript.h"

#include <assert.h>

#include <iostream>
#include <mono/metadata/debug-helpers.h>

#include "Script.h"


//Hooks
#include "Transform.h"
#include "ModuleLog.h"
#include "GameObject.h"

ModuleScript::ModuleScript()
{
}


ModuleScript::~ModuleScript()
{
}

void AddAllInternalMehtods()
{
	mono_add_internal_call("KeepCryingEngine.Transform::Translate", Transform_Translate);
	mono_add_internal_call("KeepCryingEngine.Debug::Log", Log_Hooker);
	mono_add_internal_call("KeepCryingEngine.GameObject::GetTransform",GameObject_GetTransform);
}

bool ModuleScript::Init()
{
	mono_set_dirs("../external/Mono/lib", "../external/Mono/etc");

	domain = mono_jit_init("Domain_Name");
	assert(domain != nullptr);

	assembly = mono_domain_assembly_open(domain, "./Library/Scripts.dll");
	assert(assembly != nullptr);

	image = mono_assembly_get_image(assembly);
	assert(image != nullptr);

	AddAllInternalMehtods();

	return true;
}

update_status ModuleScript::Update()
{
	for (Script * script : scripts)
	{
		UpdateScript(script);
	}

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScript::CleanUp()
{
	mono_jit_cleanup(domain);

	return true;
}

void ModuleScript::UpdateScript(Script* script)
{
	MonoObject * monoObject = script->GetScriptInstance();
	if (monoObject != nullptr)
	{
		mono_runtime_invoke(script->GetUpdateMethod(), monoObject, nullptr, nullptr);
	}
}

void ModuleScript::Subscribe(Script* script)
{
	scripts.insert(script);
}

void ModuleScript::Unsubscribe(Script* script)
{
	scripts.erase(script);
}

void output_fields(MonoClass* klass) {
	MonoClassField *field;
	void * iter = NULL;

	while ((field = mono_class_get_fields(klass, &iter))) {
		MonoType * monoType = mono_field_get_type(field);
		char * typeName = mono_type_get_name(monoType);
		LOG_DEBUG("%s", typeName);

		LOG_DEBUG("%i", mono_type_get_type(monoType));

		LOG_DEBUG("Field: %s, flags 0x%x\n", mono_field_get_name(field),
			mono_field_get_flags(field));
	}
}

void ModuleScript::SetClassToScript(Script & script, const std::string &className)
{
	MonoClass * scriptClass = mono_class_from_name(image, "", className.c_str());
	if (scriptClass != nullptr)
	{
		MonoMethod* updateMethod = mono_class_get_method_from_name(scriptClass, "Update", 0);
		assert(updateMethod != nullptr);
		script.SetUpdateMethod(updateMethod);

		
		//output_fields(scriptClass);

		MonoClassField *field;
		void * iter = NULL;

		while((field = mono_class_get_fields(scriptClass, &iter)))
		{
			script.AddField(field);
		}

		MonoObject* instance = mono_object_new(domain, scriptClass);
		assert(instance != nullptr);
		mono_runtime_object_init(instance);
		script.SetScriptInstance(instance);

		//Set gameobject where the script is placed
		MonoClass * componentClass = mono_class_from_name(image, "KeepCryingEngine", "Component");
		assert(componentClass != nullptr);
		MonoMethod* initMethod = mono_class_get_method_from_name(componentClass, "InitMonoBehaviour", 1);
		assert(initMethod != nullptr); //TODO, arreglar esto, no encuentra la funcion de init

		void* args[1];
		args[0] = &script.gameObject;

		mono_runtime_invoke(initMethod, instance, args, nullptr);
	}
	else 
	{
		LOG_DEBUG("CLASS NOT FOUND");
	}
}
