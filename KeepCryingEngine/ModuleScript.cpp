#include "ModuleScript.h"

#include <assert.h>

#include <iostream>
#include <mono/metadata/debug-helpers.h>

#include "Script.h"

#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Transform.h"
#include "float3.h"
#include <iostream>

ModuleScript::ModuleScript()
{
}


ModuleScript::~ModuleScript()
{
}

void AddAllInternalMehtods()
{
	mono_add_internal_call("Transform::Translate", Transform_Translate);
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



bool ModuleScript::Start()
{	
	/*
	//Create a instance of the class
	MonoObject* programInstance;
	programInstance = mono_object_new(domain, programClass);
	if (!programInstance)
	{
		std::cout << "mono_object_new failed" << std::endl;
		system("pause");
		return 1;
	}

	//Call its default constructor
	mono_runtime_object_init(programInstance);

	
	GameObject * gameobject = App->scene->AddEmpty(*App->scene->GetRoot(), "MAGICK");
	Transform * transform = gameobject->GetTransform();
		
	void* args[1];
	args[0] = &transform; // Esto le pasa un puntero del puntero porque lo que queremos pasar es el puntero del transform y no el transform en si
	
	std::cout << "c++ " << transform << std::endl;

	std::cout << transform->GetWorldPosition().x << std::endl;

	//Run the method
	std::cout << "Running the method: " << std::endl;
	mono_runtime_invoke(method, programInstance, args, nullptr);

	std::cout << transform->GetWorldPosition().x << std::endl;*/
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

void ModuleScript::SetClassToScript(Script & script, const std::string &className)
{
	MonoClass * scriptClass = mono_class_from_name(image, "", className.c_str());
	if (scriptClass != nullptr)
	{
		MonoMethod* updateMethod = mono_class_get_method_from_name(scriptClass, "Update", 0);
		assert(updateMethod != nullptr);
		script.SetUpdateMethod(updateMethod);


		MonoObject* instance = mono_object_new(domain, scriptClass);
		assert(instance != nullptr);
		mono_runtime_object_init(instance);
		script.SetScriptInstance(instance);
	}
	else 
	{
		LOG_DEBUG("CLASS NOT FOUND");
	}
}
