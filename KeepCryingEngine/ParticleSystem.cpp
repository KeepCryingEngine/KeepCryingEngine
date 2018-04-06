#include "ParticleSystem.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleCamera.h"
#include "Camera.h"
#include "ModuleFX.h"
#include "GameObject.h"
#include "ModuleRender.h"
#include "ModuleScene.h"

ParticleSystem::ParticleSystem() : Component(ParticleSystem::TYPE)
{
	material = new Material();
	accumElapsedTotal = fallingTime / maxParticles;
	SetMaxParticles(maxParticles);
}

ParticleSystem::~ParticleSystem()
{
	RELEASE(material);
}

void ParticleSystem::RealUpdate()
{
	Camera* cam = App->camera->GetPlayOrEditorCamera();
	if(cam != nullptr)
	{
		Update(*cam);
		Render(*cam);
	}
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

		if(ImGui::DragInt("Max particles", reinterpret_cast<int*>(&maxParticles), 1, 0, 1000000))
		{
			accumElapsedTotal = fallingTime / maxParticles;

			SetMaxParticles(maxParticles);
		}

		ImGui::DragFloat2("Emit area", emitArea.ptr(), 0.1f, 0.0f, 1000000.0f);

		if(ImGui::DragFloat("Falling time", &fallingTime, 0.1f, 0.1f, 1000000.0f))
		{
			accumElapsedTotal = fallingTime / maxParticles;
		}

		ImGui::DragFloat("Falling height", &fallingHeight, 0.1f, 0.1f, 1000000.0f);

		ImGui::DragFloat2("Particle size", particleSize.ptr(), 0.1f, 0.1f, 1000000.0f);

		if(material)
		{
			material->DrawUI();
		}
	}
}

void ParticleSystem::PreLoad(const nlohmann::json& json)
{
	Component::PreLoad(json);

	maxParticles = json["maxParticles"];
	from_json(json["emitArea"], emitArea);
	fallingTime = json["fallingTime"];
	accumElapsedTotal = json["accumElapsedTotal"];
	fallingHeight = json["fallingHeight"];
	from_json(json["particleSize"], particleSize);

	material->SetShaderType(json["material"]["shaderType"]);
	material->SetTextureByPath(json["material"]["texture"]["path"].get<std::string>());
	material->GetTexture()->SetTextureConfiguration(json["material"]["texture"]["jsonConfiguration"]);
}

void ParticleSystem::Save(nlohmann::json& json) const
{
	/*

	Relevant information:

	maxParticles
	emitArea
	fallingTime
	accumElapsedTotal
	fallingHeight
	particleSize
	material

	*/

	Component::Save(json);

	json["maxParticles"] = maxParticles;

	nlohmann::json jsonEmitArea;
	to_json(jsonEmitArea, emitArea);

	json["emitArea"] = jsonEmitArea;

	json["fallingTime"] = fallingTime;
	json["accumElapsedTotal"] = accumElapsedTotal;
	json["fallingHeight"] = fallingHeight;

	nlohmann::json jsonParticleSize;
	to_json(jsonParticleSize, particleSize);

	json["particleSize"] = jsonParticleSize;

	nlohmann::json jsonMaterial;
	jsonMaterial["shaderType"] = material->GetShaderType();

	nlohmann::json jsonTexture;
	jsonTexture["path"] = material->GetTexture()->Identifier().path.string();
	jsonTexture["jsonConfiguration"] = material->GetTexture()->GetTextureConfiguration();

	jsonMaterial["texture"] = jsonTexture;

	json["material"] = jsonMaterial;
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

	accumElapsed += timeS;

	ParticleList::iterator it = alive.begin();

	while(it != alive.end())
	{
		Particle& particle = particles[*it];

		particle.lifetime -= timeS;

		if(particle.lifetime > 0)
		{
			particle.billboard->SetLocalPosition(particle.billboard->GetLocalPosition() + timeS * particle.velocity);
			particle.billboard->SetWorldPosition(particle.billboard->GetWorldPosition() + timeS * particle.velocity);
			++it;
		}
		else
		{
			RELEASE(particles[(*it)].billboard);
			dead.push_back(*it);
			it = alive.erase(it);
		}
	}

	unsigned times = (unsigned)(accumElapsed / accumElapsedTotal);

	for(unsigned i = 0; i < times; ++i)
	{
		if(!CreateParticle(camera))
		{
			break;
		}
	}

	if(times > 0)
	{
		accumElapsed = 0;
	}

	ClearBuffers();

	for(ParticleList::iterator it = alive.begin(); it != alive.end(); ++it)
	{
		particles[(*it)].billboard->ComputeQuadInitial(camera,&vertexPos,&vertexUv,&indices);
	}

	numVertices = vertexUv.size();
	numIndices = indices.size();
	if(numVertices > 0)
	{
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * numVertices, vertexUvPointer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Generate Indices buffer
		const GLushort* indicesPointer = &indices[0];
		glGenBuffers(1, &indicesBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numIndices, indicesPointer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}

void ParticleSystem::Render(const Camera& camera)
{
	RenderBox(camera);

	BufferInfo temp = { numVertices, numIndices, vertexPosBufferId, vertexUvBufferId, indicesBufferId };
	App->fx->AddToDraw(*material, *App->scene->GetRoot()->GetTransform(), temp);
}

void ParticleSystem::RenderBox(const Camera& camera)
{
	const float3& position = gameObject->GetTransform()->GetWorldPosition();
	float3 rotation = gameObject->GetTransform()->GetWorldRotation().ToEulerXYZ();
	const float3& scale = gameObject->GetTransform()->GetWorldScale();
	float3 color { 255, 0, 0 };

	App->renderer->DrawRectangularBox(position, rotation, scale, color, emitArea.x, 0.5f * fallingHeight, emitArea.y);
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

void ParticleSystem::ClearBuffers()
{
	vertexPos.clear();
	vertexUv.clear();
	indices.clear();

	glDeleteBuffers(1, &vertexPosBufferId);
	glDeleteBuffers(1, &vertexUvBufferId);
	glDeleteBuffers(1, &indicesBufferId);
}

bool ParticleSystem::CreateParticle(const Camera& camera)
{
	if(!dead.empty())
	{
		float3 position = gameObject->GetTransform()->GetWorldPosition();
		float3 localPos = float3(RandomFloat(-emitArea.x, emitArea.x), 0.5f * fallingHeight, RandomFloat(-emitArea.y, emitArea.y));

		unsigned index = dead.back();
		dead.pop_back();

		alive.push_back(index);
		Particle& particle = particles[index];

		particle.velocity = float3(0,-fallingHeight/fallingTime,0);
		particle.lifetime = fallingTime;

		Billboard* tempBilboard = new Billboard();
		tempBilboard->SetLocalPosition(position + localPos);
		tempBilboard->SetWorldPosition(position + localPos);
		tempBilboard->SetSize(particleSize);

		particle.billboard = tempBilboard;
		return true;
	}

	return false;
}