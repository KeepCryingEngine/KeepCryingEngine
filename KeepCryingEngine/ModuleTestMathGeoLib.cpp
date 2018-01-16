#include "ModuleTestMathGeoLib.h"

#include <MathGeoLib.h>

using namespace std;

ModuleTestMathGeoLib::ModuleTestMathGeoLib()
{
}

ModuleTestMathGeoLib::~ModuleTestMathGeoLib()
{
}

bool ModuleTestMathGeoLib::Start()
{
	cout << "ModuleTestMathGeoLib" << endl;

	float3 float3(1.0f, 2.0f, 3.0f);

	cout << "\tfloat3: " << float3 << endl;

	Sphere sphere(float3, 5.0f);

	cout << "\tsphere: " << sphere << endl;

	Quat quat(1.0f, 2.0f, 3.0f, 4.0f);

	cout << "\tquat: " << quat << endl;

	return true;
}