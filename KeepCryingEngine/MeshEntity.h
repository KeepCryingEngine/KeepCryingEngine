#ifndef _MESHENTITY_H_
#define _MESHENTITY_H_

#include <GL/glew.h>
#include <AABB.h>

class MeshEntity
{
public:
	MeshEntity();
	virtual ~MeshEntity();

	const AABB& GetAABB() const;
	GLuint GetVertexBufferId() const;
	GLuint GetIndicesBufferId() const;
	GLuint GetNormalBufferId() const;
	GLuint GetVerticesNumber() const;

private:
	AABB aabb;
	GLuint vertexBufferId = 0;
	GLuint indicesBufferId = 0;
	GLuint normalBufferId = 0;
	GLuint verticesNumber = 0;

};

#endif // !_MESHENTITY_H_



