#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#define HAVE_M_PI

#include <SDL.h>
#include <GL/glew.h>
#include <float3.h>
#include <list>
#include <vector>

#include "Module.h"

#define MAX_BONES 100

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
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	void DrawCross(const float3& pos, float scale) const;

	void AddToDrawBuffer(Mesh& mesh, Material& material, GameObject& gameObject, Transform& transform);

	void DrawFrustum(Camera& camera);

	void DrawCube(const float3& offset, const float3& color, float radius) const;

	void DrawRectangularBox(const float3& position, const float3& rotation, const float3& scale, const float3& color, float w, float h, float d) const;

	void DrawSphere(const float3& offset, const float3& color, float radius) const;

private:
	void DrawGrid() const;

	void SetUpLight() const;

	//void DrawFromBuffer(Mesh& mesh);

	void DrawGeometry();
	void Draw(const DrawInfo& drawInfo);

	void DrawLastRay();

public:
	GLfloat globalAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };	

private:	
	SDL_GLContext glcontext = nullptr;
	//std::list<Mesh*> drawBuffer;

	std::vector<DrawInfo> drawBuffer;

	static const float3 LIGHT_DIR;

};

#endif // !_MODULERENDER_H_