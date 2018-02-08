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
class MeshEntity;
class MaterialEntity;
class GameObject;
class Transform;

class ModuleRender : public Module
{
private:
	struct DrawInfo {
		MeshEntity& mesh;
		MaterialEntity& material;
		GameObject& gameObject;
		Transform& transform;
	};

public:
	ModuleRender();
	virtual ~ModuleRender();

	bool Init() override;
	update_status PreUpdate(float deltaTimeS, float realDeltaTimeS) override;
	update_status Update(float deltaTimeS, float realDeltaTimeS) override;
	update_status PostUpdate(float deltaTimeS, float realDeltaTimeS) override;
	bool CleanUp() override;

	void DrawCross(const float3& pos, float scale) const;

	//void AddToDrawBuffer(Mesh& mesh);
	void AddToDrawBuffer(MeshEntity& mesh, MaterialEntity& material, GameObject& gameObject, Transform& transform);

	void DrawFrustrum(Camera& camera);

private:
	void DrawGrid() const;

	void SetUpLight() const;

	//void DrawFromBuffer(Mesh& mesh);

	void DrawGeometry();
	void Draw(const DrawInfo& drawInfo);

public:
	GLfloat globalAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };	

private:	
	SDL_GLContext glcontext = nullptr;
	//std::list<Mesh*> drawBuffer;

	std::vector<DrawInfo> drawBuffer;

	static const float3 LIGHT_DIR;

};

#endif // !_MODULERENDER_H_