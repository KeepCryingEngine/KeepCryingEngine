#include "Shader.h"

Shader::Shader(uint id) :
	id(id)
{}

Shader::~Shader()
{}

uint Shader::GetId() const
{
	return id;
}