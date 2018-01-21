#include "ModuleTestJson.h"

#include <iostream>
#include <fstream>
#include <MathGeoLib.h>

#include <json.hpp>
#include "json_serializer.h"

using namespace std;
using nlohmann::json;

ModuleTestJson::ModuleTestJson()
{}

ModuleTestJson::~ModuleTestJson()
{}

bool ModuleTestJson::Start()
{
	LOG_DEBUG("ModuleTestJson");

	ifstream jsonFile("Assets/jsonTest.json");

	json json;
	jsonFile >> json;
	
	float3 v3;
	from_json(json, v3);
	
	// float3 v3 = json.get<float3>();

	LOG_DEBUG("%s", v3.ToString().c_str());

	return true;
}