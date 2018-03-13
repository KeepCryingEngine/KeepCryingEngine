#ifndef _ENGINE_UUID_H_
#define _ENGINE_UUID_H_

#include <cstdint>

class ENGINE_UUID
{
public:
	ENGINE_UUID();
	ENGINE_UUID(int32_t id);
	virtual ~ENGINE_UUID();

public:
	uint32_t id;

private:
	static uint32_t GenerateRandomId();
};

#endif