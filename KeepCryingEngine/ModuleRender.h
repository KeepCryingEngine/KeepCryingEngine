#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#define HAVE_M_PI

#include <SDL.h>
#include <GL/glew.h>
#include <float3.h>
#include <list>
#include <vector>

#include "Module.h"

class Mesh;
class Camera;
class Mesh;
class Material;
class GameObject;
class Transform;

class ModuleRender : public Module
{
private:
	struct DrawInfo {
		Mesh& mesh;
		Material& material;
		GameObject& gameObject;
		Transform& transform;
	};

public:
	ModuleRender();
	virtual ~ModuleRender();

	bool Init() override;
	bool Start() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	void DrawCross(const float3& pos, const float3& color, float scale) const;

	void AddToDrawBuffer(Mesh& mesh, Material& material, GameObject& gameObject, Transform& transform);

	void DrawFrustum(Camera& camera);

	void DrawLightFrustum();

	void DrawCube(const float3& offset, const float3& color, float radius) const;

	void DrawRectangularBox(const float3& position, const float3& rotation, const float3& scale, const float3& color, float w, float h, float d) const;

	void DrawSphere(const float3& offset, const float3& color, float radius) const;

	void DrawOBB(const OBB& obb, const float3& color) const;

	void DrawAABB(const AABB& aabb, const float3& color) const;

private:
	void DrawGrid() const;

	void SetUpLight() const;

	//void DrawFromBuffer(Mesh& mesh);

	void DrawGeometry();
	void Draw(const DrawInfo& drawInfo);

	void DrawLastRay();

	void DrawShadowTexture();
	void DrawShadowTexture(const DrawInfo& drawInfo);

public:
	GLfloat globalAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };	
	GLuint shadowTextureId = 0;
private:	
	SDL_GLContext glcontext = nullptr;
	//std::list<Mesh*> drawBuffer;

	std::vector<DrawInfo> drawBuffer;
	GLuint uniformCameraBufferId = 0;
	GLuint shadowFrameBufferId = 0;
	

	static const float3 LIGHT_DIR;

};

#endif // !_MODULERENDER_H_