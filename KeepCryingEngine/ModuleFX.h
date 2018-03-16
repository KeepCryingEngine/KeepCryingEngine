#ifndef _MODULEFX_H_
#define _MODULEFX_H_

#include <vector>

#include "Module.h"
#include "Component.h"
#include "Material.h"
#include "Transform.h"

struct BufferInfo
{
	GLuint vertexPosId;
	GLuint vertexUvId;
	GLuint indicesId;
	int nVertices;
	int nIndices;
};

struct DrawInfo
{
	Material material;
	Transform transform;
	BufferInfo buffer;
};

class ModuleFX : public Module
{
public:
	ModuleFX();
	~ModuleFX();

	update_status Update() override;
	void Draw(const DrawInfo & drawInfo);

	void AddToDraw(Material& material,Transform& trans, BufferInfo& info);

private:
	std::vector<DrawInfo> effectsToDraw;
};

#endif // !_MODULEFX_H_

