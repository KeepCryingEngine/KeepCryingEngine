#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include "Component.h"
#include "Billboard.h"

struct Particle
{
	float3 position;
	float3 velocity;
	int lifetime;
};

class Material;

class ParticleSystem : public Component
{
public:
	static const Component::Type TYPE = Component::Type::ParticleSystem;

	ParticleSystem();
	virtual ~ParticleSystem();

	void RealUpdate() override;

	void DrawUI() override;

private:
	void SetMaxParticles(unsigned maxParticles);

	void SetMaterial(Material& material);

	void Update(const Camera& camera);

	void Render();

	void Clear();

	bool CreateParticle();

private:
	typedef std::vector<Particle> ParticlePool;
	typedef std::vector<unsigned> ParticleList;
	typedef std::vector<Billboard> BillboardList;

	ParticlePool particles;
	ParticleList alive;
	ParticleList dead;
	BillboardList billboards;

	unsigned maxParticles = 0;
	float2 emitArea = float2::zero;
	unsigned fallingTime = 0;
	unsigned accumElapsed = 0;
	unsigned accumElapsedTotal = 0;
	float fallingHeight = 0.0f;
	float2 particleSize = float2::zero;

	Material* material = nullptr;

	int numVertices = 0;
	int numIndices = 0;
	GLuint vertexPosBufferId = 0;
	GLuint vertexUvBufferId = 0;
	GLuint indicesBufferId = 0;

	std::vector<float3> vertices;
	std::vector<float2> textCoords;
	std::vector<GLushort> indices;
};

#endif