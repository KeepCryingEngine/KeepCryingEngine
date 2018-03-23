#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include "Component.h"
#include "Billboard.h"

struct Particle
{
	float3 velocity;
	float lifetime;
	Billboard* billboard;
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

	void Render(const Camera& camera);

	void RenderBox(const Camera& camera);

	void Clear();

	void ClearBuffers();

	bool CreateParticle(const Camera& camera);

private:
	typedef std::vector<Particle> ParticlePool;
	typedef std::vector<unsigned> ParticleList;

	ParticlePool particles;
	ParticleList alive;
	ParticleList dead;

	unsigned maxParticles = 1000;
	float2 emitArea = float2::one;
	float fallingTime = 1;
	float accumElapsed = 0;
	float accumElapsedTotal = 0;
	float fallingHeight = 1.0f;
	float2 particleSize = float2::one;

	Material* material = nullptr;

	int numVertices = 0;
	int numIndices = 0;
	GLuint vertexPosBufferId = 0;
	GLuint vertexUvBufferId = 0;
	GLuint indicesBufferId = 0;

	std::vector<float3> vertexPos;
	std::vector<float2> vertexUv;
	std::vector<GLushort> indices;
};

#endif