#include "ParticleSystem.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleCamera.h"
#include "Camera.h"
#include "ModuleFX.h"
#include "GameObject.h"

ParticleSystem::ParticleSystem() : Component(ParticleSystem::TYPE)
{
	material = new Material();
}

ParticleSystem::~ParticleSystem()
{
	RELEASE(material);
}

void ParticleSystem::RealUpdate()
{
	Update(*App->camera->camera);
	Render();
}

void ParticleSystem::DrawUI()
{
	if(ImGui::CollapsingHeader("ParticleSystem"))
	{
		ImGui::Checkbox("Active", &enabled);
		
		ImGui::SameLine();

		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}

		if(ImGui::DragInt("Max particles", reinterpret_cast<int*>(&maxParticles), 0.1f, 0, 1000000))
		{
			accumElapsedTotal = (unsigned)((float)maxParticles / (float)fallingTime);

			SetMaxParticles(maxParticles);
		}

		ImGui::DragFloat2("Emit area", emitArea.ptr(), 0.1f, 0.0f, 1000000.0f);

		if(ImGui::DragInt("Falling time", reinterpret_cast<int*>(&fallingTime), 0.1f, 0, 1000000))
		{
			accumElapsedTotal = (unsigned)((float)maxParticles / (float)fallingTime);
		}

		ImGui::DragFloat("Falling height", &fallingHeight, 0.1f, 0.0f, 1000000);

		ImGui::DragFloat2("Particle size", particleSize.ptr(), 0.1f, 0.0f, 1000000.0f);

		if(material)
		{
			material->DrawUI();
		}
	}
}

void ParticleSystem::SetMaxParticles(unsigned maxParticles)
{
	this->maxParticles = maxParticles;

	Clear();

	particles.resize(maxParticles);

	alive.reserve(maxParticles);

	dead.reserve(maxParticles);

	for(unsigned i = 0; i < maxParticles; ++i)
	{
		dead.push_back(i);
	}

	billboards.resize(maxParticles);
}

void ParticleSystem::SetMaterial(Material& material)
{
	this->material = &material;
}

void ParticleSystem::Update(const Camera& camera)
{
	float timeS = App->time->GetDeltaTime();
	int timeMs = (int)(1000 * timeS);

	accumElapsed += timeMs;

	ParticleList::iterator it = alive.begin();

	while(it != alive.end())
	{
		Particle& particle = particles[*it];

		particle.lifetime -= timeMs;

		if(particle.lifetime > 0)
		{
			particle.position += timeS * particle.velocity;
			++it;
		}
		else
		{
			dead.push_back(*it);
			it = alive.erase(it);
		}
	}

	unsigned times = accumElapsed / accumElapsedTotal;

	for(unsigned i = 0; i < times; ++i)
	{
		CreateParticle();
	}

	if(times > 0)
	{
		accumElapsed = 0;
	}
}

void ParticleSystem::Render()
{
	BufferInfo temp = { numVertices, numIndices, vertexPosBufferId, vertexUvBufferId, indicesBufferId };
	App->fx->AddToDraw(*material, *gameObject->GetTransform(), temp);
}

void ParticleSystem::Clear()
{
	particles.clear();
	alive.clear();
	dead.clear();
	billboards.clear();

	vertices.clear();
	textCoords.clear();
	indices.clear();
}

bool ParticleSystem::CreateParticle()
{
	if(!dead.empty())
	{
		float3 position = gameObject->GetTransform()->GetWorldPosition();
		position.y += 0.5f * fallingHeight;

		unsigned index = dead.back();
		dead.pop_back();

		Particle& particle = particles[index];

		particle.position = position;
		particle.velocity = -float3::unitY;
		particle.lifetime = fallingTime;
	}

	return false;
}