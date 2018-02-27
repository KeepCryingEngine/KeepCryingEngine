#include "Animator.h"

#include "Transform.h"
#include "ModuleAnim.h"
#include "GameObject.h"
#include "Application.h"

using namespace std;

Animator::Animator() : 
	Component(ComponentType::Animator)
{ }

Animator::~Animator()
{ }

void Animator::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	if(HasValidAnimationInstance())
	{
		for(GameObject* child : gameObject->GetChildren())
		{
			UpdateTransformRecursive(animInstanceId, child);
		}
	}
}

void Animator::DrawUI()
{
	if(ImGui::CollapsingHeader("Animator"))
	{
		static char animatorSetAnimInstanceBuffer[252] = { };
		ImGui::InputText("##setAnimation", animatorSetAnimInstanceBuffer, 252); ImGui::SameLine();
		if(ImGui::Button("Set Animation"))
		{
			string s = "Assets/";
			s += animatorSetAnimInstanceBuffer;

			SetAnimInstance(s.c_str());
		}

		static char animatorPlayAnimInstanceBuffer[252] = {};
		ImGui::InputText("##playAnimation", animatorPlayAnimInstanceBuffer, 252); ImGui::SameLine();
		if(ImGui::Button("Play Animation"))
		{
			PlayAnimInstance(animatorPlayAnimInstanceBuffer);
		}
	}
}

vector<ComponentType> Animator::GetNeededComponents() const
{
	return { ComponentType::Transform };
}

vector<ComponentType> Animator::GetProhibitedComponents() const
{
	return { ComponentType::Animator };
}

unsigned int Animator::GetAnimInstanceId() const
{
	return animInstanceId;
}

void Animator::SetAnimInstance(const char * path)
{
	string tmpPath = path;

	size_t splitIndex = tmpPath.find_last_of("/");

	if(splitIndex != string::npos)
	{
		string basePath = tmpPath.substr(0, splitIndex + 1);
		string fileName = tmpPath.substr(splitIndex + 1, tmpPath.size());

		App->anim->Load(basePath, fileName);
	}
}

void Animator::PlayAnimInstance(const char* name)
{
	if(HasValidAnimationInstance())
	{
		App->anim->Stop(animInstanceId);
	}

	animInstanceId = App->anim->Play(name);
}

bool Animator::HasValidAnimationInstance() const
{
	return animInstanceId != -1;
}

void Animator::UpdateTransformRecursive(unsigned int animInstanceId, GameObject* gameObject)
{
	aiVector3D position;
	aiQuaternion rotation;

	App->anim->GetTransform(animInstanceId, gameObject->GetName().c_str(), position, rotation);

	Transform* transform = gameObject->GetTransform();

	transform->SetLocalPosition(float3(position.x, position.y, position.z));
	transform->SetLocalRotation(Quat(rotation.x, rotation.y, rotation.z, rotation.w));

	for(GameObject* child : gameObject->GetChildren())
	{
		UpdateTransformRecursive(animInstanceId, child);
	}
}