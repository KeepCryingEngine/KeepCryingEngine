#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#define HAVE_M_PI

#include <SDL.h>
#include <float3.h>

#include "Module.h"

struct ILinfo;

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

	bool getMipmap() const;
	void setMipmap(bool mipMap);

	bool getAnisotropicFilter() const;
	void setAnisotropicFilter(bool anisotropicFilter);

	void SetCurrentTexture(uint textureIndex);

private:
	void SetUpTextures();
	void CleanUpTextures();
	void SetUpTexture();
	void SetUpBigArray();
	void SetUpIndicesArray();
	void SetUpSphere(float radius, unsigned int rings, unsigned int sectors);
	void SetUpPlane();
	void DrawCubeDirect(float x, float y, float z) const;
	void DrawCubeBigArray(float x, float y, float z) const;
	void DrawCubeIndices(float x, float y, float z) const;
	void DrawSphere(float x, float y, float z) const;
	void DrawPlane(float x, float y, float z) const;

	void SetUpGrid() const;
	void DrawGrid() const;

	uint LoadTexture(const char* theFileName, ILinfo* imageInfo);

public:
	uint* actualTexture = nullptr;
	uint debugTexture = 0;
	uint lenaTexture = 0;
	uint exodiaTexture = 0;
	uint rockTexture = 0;

private:
	uint bigArrayCube = 0;
	uint bigArrayCubeUV = 0;
	uint vertexArrayBuffer = 0;
	uint indicesArrayBuffer = 0;
	uint uvArrayBuffer = 0;
	uint sphereIndex = 0;
	uint sphereVertex = 0;
	uint sphereUV = 0;
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
	bool mipmap = false;
	bool anisotropicFilter = false;
};

#endif // !_MODULERENDER_H_