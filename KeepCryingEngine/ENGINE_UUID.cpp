#include "ENGINE_UUID.h"

#include <MathGeoLib.h>


ENGINE_UUID::ENGINE_UUID() :
	id(GenerateRandomId())
{
}

ENGINE_UUID::ENGINE_UUID(int32_t id) :
	id(id)
{
}

ENGINE_UUID::~ENGINE_UUID()
{
}

bool ENGINE_UUID::operator==(const ENGINE_UUID & uuid)
{
	return id == uuid.id;
}

uint32_t ENGINE_UUID::GenerateRandomId()
{
	static LCG lcg;
	return (uint32_t)lcg.IntFast();
}