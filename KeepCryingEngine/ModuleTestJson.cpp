#include "ModuleTestJson.h"

#include "Vector3.h"
#include <json.hpp>
#include <iostream>
#include <fstream>
#include "json_serializer.h"

using namespace std;
using nlohmann::json;

ModuleTestJson::ModuleTestJson()
{
}

ModuleTestJson::~ModuleTestJson()
{
}

bool ModuleTestJson::Start()
{
	LOG_DEBUG("ModuleTestJson");

	ifstream jsonFile("Assets/jsonTest.json");

	json json;

	jsonFile >> json;

	Vector3 v3 = json.get<Vector3>();

	LOG_DEBUG("%s", to_string(v3).c_str());

	return true;
}