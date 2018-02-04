#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include <GL/glew.h>

class SkyBox
{
public:
	SkyBox();
	~SkyBox();

	void SetUp();
	void CleanUp();
	void Draw();

public:
	GLuint skyboxTexture = 0;
	GLuint skyboxProgram = 0;

	GLuint vbo_cube_vertices = 0;
	GLuint ibo_cube_indices = 0;

private:
	void LoadShaders();
	void LoadVerticesAndIndices();
	void LoadTextures();

};

#endif // !_SKYBOX_H_