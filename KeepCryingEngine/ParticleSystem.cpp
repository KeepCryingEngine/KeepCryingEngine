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

	for(ParticleList::iterator it = alive.begin(); it != alive.end(); ++it)
	{
		particles[(*it)].billboard->ComputeQuadInitial(camera,&vertexPos,&vertexUv,&indices);
	}

	numVertices = vertexUv.size();
	numIndices = indices.size();

	//Generate Vertex Pos buffer
	const float3 * vertexPosPointer = &vertexPos[0];
	glGenBuffers(1, &vertexPosBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPosBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * numVertices, vertexPosPointer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate Vertex Uv buffer
	const float2 * vertexUvPointer = &vertexUv[0];
	glGenBuffers(1, &vertexUvBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexUvBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * numVertices, vertexUvPointer, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate Indices buffer
	const GLushort* indicesPointer = &indices[0];
	glGenBuffers(1, &indicesBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numIndices, indicesPointer, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

	vertexPos.clear();
	vertexUv.clear();
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

		Billboard* tempBilboard = new Billboard();
		tempBilboard->SetLocalPosition(float3(0, 0.5f * fallingHeight,0));//TODO: change
		tempBilboard->SetWorldPosition(position);
		tempBilboard->SetSize(particleSize);

		particle.billboard = tempBilboard;
	}

	return false;
}