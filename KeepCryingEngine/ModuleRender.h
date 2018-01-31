#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#define HAVE_M_PI

#include <SDL.h>
#include <GL/glew.h>
#include <float3.h>

#include "Module.h"

struct ILinfo;

struct Vertex
{
	GLfloat position[3];
	GLfloat color[4];
	GLfloat uv[2];
};

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init() override;
	update_status PreUpdate(float deltaTimeS, float realDeltaTimeS) override;
	update_status Update(float deltaTimeS, float realDeltaTimeS) override;
	update_status PostUpdate(float deltaTimeS, float realDeltaTimeS) override;
	bool CleanUp() override;

	void DrawCross(const float3& pos, float scale) const;

	int getImageWidth() const;
	int getImageHeight() const;
	int getImageFormat() const;
	int getImageSizeBytes() const;
	int getImagePixelDepth() const;

	uint getWrapModeS() const;
	void setWrapModeS(uint wrapModeS);

	uint getWrapModeT() const;
	void setWrapModeT(uint wrapModeT);

	bool getMagFilter() const;
	void setMagFilter(bool magFilter);

	bool getMinFilter() const;
	void setMinFilter(bool minFilter);

	uint getMagFilterMode() const;
	void setMagFilterMode(uint magFilterMode);

	uint getMinFilterMode() const;
	void setMinFilterMode(uint minFilterMode);

	bool getMipmap() const;
	void setMipmap(bool mipMap);

	bool getAnisotropicFilter() const;
	void setAnisotropicFilter(bool anisotropicFilter);

	void SetCurrentTexture(uint textureIndex);

private:
	// void SetUpShaderStruct() const;
	void FillVerticesData(uint n, const float* positions, const float* colors, const float* texCoords, Vertex* vertices) const;

	void SetUpTextures();
	void CleanUpTextures();
	void SetUpTexture();
	void SetUpBigArray(float3 pos,float size);
	void SetUpIndicesArray(float3 pos, float size);
	void SetUpSphere(float3 pos,float radius, unsigned int rings, unsigned int sectors);
	void SetUpPlane(float3 pos, float size);
	void DrawCubeDirect(float3 pos, float size) const;
	void DrawCubeBigArray() const;
	void DrawCubeIndices() const;
	void DrawSphere() const;
	void DrawPlane() const;

	void SetUpGrid() const;
	void DrawGrid() const;

	void SetUpLight();

	uint LoadTexture(const char* theFileName, ILinfo* imageInfo);

public:
	uint* actualTexture = nullptr;
	uint debugTexture = 0;
	uint lenaTexture = 0;
	uint exodiaTexture = 0;
	uint rockTexture = 0;

	GLfloat globalAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

private:	
	uint bigArrayCube = 0;
	uint vertexArrayBuffer = 0;
	uint indicesArrayBuffer = 0;
	// uint uvArrayBuffer = 0;
	uint sphereIndex = 0;
	uint sphereVertex = 0;
	// uint sphereUV = 0;
	uint planeVertices = 0;
	uint planeUVs = 0;

	int sphereIndicesSize = 0;
	static const uint CHECKERS_HEIGHT;
	static const uint CHECKERS_WIDTH;
	SDL_GLContext glcontext = nullptr;

	ILinfo* currentImageInfo = nullptr;

	ILinfo* lenaImageInfo = nullptr;
	ILinfo* rockImageInfo = nullptr;
	ILinfo* exodiaImageInfo = nullptr;

	uint wrapModeS = 0;
	uint wrapModeT = 0;
	bool magFilter = true;
	bool minFilter = true;
	uint magFilterMode = 0;
	uint minFilterMode = 0;
	bool mipmap = false;
	bool anisotropicFilter = false;
};

#endif // !_MODULERENDER_H_