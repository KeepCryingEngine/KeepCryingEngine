#include "ModuleScript.h"

#include <assert.h>

#include <iostream>
#include <mono/metadata/debug-helpers.h>


ModuleScript::ModuleScript()
{
}


ModuleScript::~ModuleScript()
{
}

bool ModuleScript::Init()
{
	mono_set_dirs("../external/Mono/lib", "../external/Mono/etc");

	domain = mono_jit_init("Domain_Name");
	if (domain == nullptr) 
	{
		return false;
	}

	//Open a assembly in the domain
	assembly = mono_domain_assembly_open(domain, "./Library/Scripts.dll");
	if (domain == nullptr)
	{
		return false;
	}

	//Get a image from the assembly
	MonoImage* image = mono_assembly_get_image(assembly);
	if (!image)
	{
		std::cout << "mono_assembly_get_image failed" << std::endl;
		system("pause");
		return 1;
	}

	//Get the class
	MonoClass* programClass;
	programClass = mono_class_from_name(image, "", "Test");
	if (!programClass)
	{
		std::cout << "mono_class_from_name failed" << std::endl;
		system("pause");
		return 1;
	}

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

	//Build a method description object
	MonoMethodDesc* updateMethod;
	const char* BarkMethodDescStr = "Test:TestMethod()";
	updateMethod = mono_method_desc_new(BarkMethodDescStr, NULL);
	if (!updateMethod)
	{
		std::cout << "mono_method_desc_new failed" << std::endl;
		system("pause");
		return 1;
	}

	//Search the method in the image
	MonoMethod* method;
	method = mono_method_desc_search_in_image(updateMethod, image);
	if (!method)
	{
		std::cout << "mono_method_desc_search_in_image failed" << std::endl;
		system("pause");
		return 1;
	}

	//Run the method
	std::cout << "Running the method: " << BarkMethodDescStr << std::endl;
	mono_runtime_invoke(method, programInstance, nullptr, nullptr);

	return true;
}

update_status ModuleScript::Update()
{
	return update_status();
}

bool ModuleScript::CleanUp()
{
	return false;
}
