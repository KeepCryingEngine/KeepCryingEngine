#ifndef _SHADER_H_
#define _SHADER_H_

#include "Globals.h"

class Shader
{
public:
	Shader(uint id);
	virtual ~Shader();

	uint GetId() const;

private:
	uint id = 0;
};

#endif