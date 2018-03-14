#include "Transform.h"

#include <MathGeoLib.h>
#include <ImGuizmo.h>

#include "Application.h"
#include "ModuleTime.h"
#include "GameObject.h"
#include "json_serializer.h"

using namespace std;

Transform::Transform() : 
	Component(Transform::TYPE),
	localPosition(float3::zero),
	localRotation(Quat::identity),
	eulerLocalRotation(float3::zero),
	localScale(float3::one),
	worldPosition(float3::zero),
	previousWorldPosition(float3::zero),
	worldRotation(Quat::identity),
	worldScale(float3::one),
	modelMatrix(float4x4::identity),
	dirty(true)
{ 
}

Transform::~Transform()
{ }

void Transform::DrawUI()
{
	if(!gameObject->IsStatic()){
		if(ImGui::CollapsingHeader("Transform"))
		{
			if (ImGui::Button("Reset"))
			{
				SetLocalTRS(float3::zero,Quat::identity,float3::one);
			}

			//Position
			float3 localPosition = this->localPosition;
			if(ImGui::DragFloat3(" Position", localPosition.ptr(), 0.1f))
			{
				SetLocalPosition(localPosition);
			}

			//Rotation
			if(ImGui::DragFloat3(" Rotation", eulerLocalRotation.ptr(), 0.1f))
			{
				float3 radAngles = DegToRad(eulerLocalRotation);
				SetLocalRotation(Quat::FromEulerXYZ(radAngles.x, radAngles.y, radAngles.z));
			}

			//Scale
			float3 localScale = this->localScale;
			if(ImGui::DragFloat3(" Scale", localScale.ptr(), 0.1f))
			{
				//This is just a temporal fix to prevent the program from crashing
				for(size_t i = 0; i < 3; i++)
				{
					localScale[i] = max(localScale[i], 0.1f);
				}
				SetLocalScale(localScale);
			}

			//DEBUG WORLD SETTERS
			ImGui::DragFloat3(" Current World Position", float3(worldPosition).ptr(), 0);
			static float3 newWorldPosition = float3::zero;
			ImGui::DragFloat3(" New World Position", newWorldPosition.ptr(), 0.1f);
			if (ImGui::Button("Set World Position"))
			{
				SetWorldPosition(newWorldPosition);
			}


			ImGui::DragFloat3(" Current World Rotation", RadToDeg(worldRotation.ToEulerXYZ()).ptr(), 0);
			static float3 newWorldRotation = float3::zero;
			ImGui::DragFloat3(" New World Rotation", newWorldRotation.ptr(), 0.1f);
			if (ImGui::Button("Set World Rotation"))
			{
				float3 radAngles = DegToRad(newWorldRotation);
				SetWorldRotation(Quat::FromEulerXYZ(radAngles.x, radAngles.y, radAngles.z));
			}

			ImGui::DragFloat3(" Current World Scale", float3(worldScale).ptr(), 0);
			static float3 newWorldScale = float3::one;
			ImGui::DragFloat3(" New World Scale", newWorldScale.ptr(), 0.1f);
			if (ImGui::Button("Set World Scale"))
			{
				SetWorldScale(newWorldScale);
			}

		}
	}
}

vector<Component::Type> Transform::GetProhibitedComponents() const
{
	return { Transform::TYPE };
}

void Transform::RealUpdate()
{
	UpdateVelocity();
}

float4x4 Transform::GetLocalMatrix() const
{
	return float4x4::FromTRS(localPosition, localRotation, localScale);
}

const float3 & Transform::GetLocalPosition() const
{
	return localPosition;
}

const Quat & Transform::GetLocalRotation() const
{
	return localRotation;
}

const float3 & Transform::GetEulerLocalRotation() const
{
	return eulerLocalRotation;
}

const float3 & Transform::GetLocalScale() const
{
	return localScale;
}

void Transform::SetLocalModelMatrix(const float4x4 & localModelMatrix)
{
	if (!gameObject->IsStatic())
	{
		SetDirty();
		Decompose(localModelMatrix,localPosition, localRotation, localScale);
	}
}

void Transform::SetLocalPosition(const float3 & position)
{
	if(!gameObject->IsStatic())
	{
		SetDirty();
		localPosition = position;
	}
}

void Transform::SetLocalRotation(const Quat & rotation)
{
	if(!gameObject->IsStatic())
	{
		SetDirty();
		localRotation = rotation;
		localRotation.Normalize();
	}
}

void Transform::SetLocalScale(const float3 & scale)
{
	if(!gameObject->IsStatic())
	{
		SetDirty();
		localScale = scale;
	}
}

void Transform::SetLocalTRS(const float3 & translation, const Quat & rotation, const float3 & scale)
{
	SetLocalPosition(translation);
	SetLocalRotation(rotation);
	SetLocalScale(scale);
}

const float3 & Transform::GetWorldPosition() const
{
	RecalculateIfNecessary();
	return worldPosition;
}

const Quat & Transform::GetWorldRotation() const
{
	RecalculateIfNecessary();
	return worldRotation;
}

const float3 & Transform::GetWorldScale() const
{
	RecalculateIfNecessary();
	return worldScale;
}

void Transform::SetModelMatrix(const float4x4 & matrix)
{
	if (!gameObject->IsStatic())
	{
		float3 position, scale;
		Quat rotation;
		Decompose(matrix, position, rotation, scale);
		SetWorldTRS(position, rotation, scale);
	}
}

/*
	Set local variables instead of world ones
	Then we calculate the world variables from the local ones when they are asked for
*/
void Transform::SetWorldPosition(const float3 & position)
{
	if(!gameObject->IsStatic())
	{
		SetDirty();

		localPosition = position - ParentWorldPosition();
		/*float3 worldPositionDelta = position - worldPosition;
		localPosition = localPosition + worldPositionDelta;*/
	}
}

void Transform::SetWorldRotation(const Quat & rotation)
{
	if(!gameObject->IsStatic())
	{
		SetDirty();
		localRotation = rotation.Mul(ParentWorldRotation().Inverted());
	}
}

void Transform::SetWorldScale(const float3 & scale)
{
	if(!gameObject->IsStatic())
	{
		SetDirty();
		localScale = scale.Div(ParentWorldScale());
	}
}

void Transform::SetWorldTRS(const float3 & translation, const Quat & rotation, const float3 & scale)
{
	SetWorldPosition(translation);
	SetWorldRotation(rotation);
	SetWorldScale(scale);
}

const float4x4 & Transform::GetModelMatrix() const
{
	RecalculateIfNecessary();
	return modelMatrix;
}

void Transform::Recalculate()
{
	SetDirty();
}

void Transform::Decompose(const float4x4 & matrix, float3 & position, Quat & rotation, float3 & scale) const
{
	// Math Geo Lib decomposition
	//matrix.Decompose(position, rotation, scale);

	// -----------------

	// Imguizmo decomposition
	float3 eulerRotation;
	ImGuizmo::DecomposeMatrixToComponents(matrix.Transposed().ptr(), position.ptr(), eulerRotation.ptr(), scale.ptr());
	eulerRotation = DegToRad(eulerRotation);
	rotation = Quat::FromEulerXYZ(eulerRotation.x, eulerRotation.y, eulerRotation.z);

	rotation.Normalize();
}

void Transform::UpdateVelocity()
{
	velocity = (GetWorldPosition() - previousWorldPosition).Div(App->time->GetDeltaTime());
	previousWorldPosition = GetWorldPosition();
}

void Transform::SetDirty() const
{
	if (!dirty)
	{
		dirty = true;

		for (GameObject* child : gameObject->GetChildren())
		{
			child->GetTransform()->SetDirty();
		}
	}
}

const float4x4& Transform::GetParentMatrix() const
{
	const float4x4* parentMatrix = &float4x4::identity;
	GameObject* parent = gameObject->GetParent();
	if (parent)
	{
		Transform* parentTransform = parent->GetTransform();
		parentMatrix = &parentTransform->GetModelMatrix();
	}
	return *parentMatrix;
}

/*
	Const function that changes mutable data
	It needs to be const because it must update the matrix only when necessary
*/
void Transform::RecalculateIfNecessary() const
{
	if (dirty) 
	{
		modelMatrix = CalculateModelMatrix();
		worldPosition = CalculateWorldPosition();
		worldScale = CalculateWorldScale();
		worldRotation = CalculateWorldRotation();
		eulerLocalRotation = RadToDeg(localRotation.ToEulerXYZ());

		right = worldRotation.Mul(float3::unitX).Normalized();
		up = worldRotation.Mul(float3::unitY).Normalized();
		forward = worldRotation.Mul(float3::unitZ).Normalized();

		dirty = false;
	}
}

float4x4 Transform::CalculateModelMatrix() const
{
	return GetParentMatrix().Mul(GetLocalMatrix());
}

float3 Transform::CalculateWorldPosition() const
{
	return ParentModelMatrix().Mul(localPosition.ToPos4()).xyz();
}

Quat Transform::CalculateWorldRotation() const
{
	return ParentWorldRotation().Mul(localRotation).Normalized();
}

float3 Transform::CalculateWorldScale() const
{
	return ParentWorldScale().Mul(localScale);
}

float4x4 Transform::ParentModelMatrix() const
{
	const float4x4* parentModelMatrix = &float4x4::identity;
	GameObject* parent = gameObject->GetParent();
	if (parent)
	{
		Transform* parentTransform = parent->GetTransform();
		parentModelMatrix = &parentTransform->GetModelMatrix();
	}
	return *parentModelMatrix;
}

float3 Transform::ParentWorldPosition() const
{
	const float3* parentWorldPosition = &float3::zero;
	GameObject* parent = gameObject->GetParent();
	if (parent)
	{
		Transform* parentTransform = parent->GetTransform();
		parentWorldPosition = &parentTransform->GetWorldPosition();
	}
	return *parentWorldPosition;
}

Quat Transform::ParentWorldRotation() const
{
	const Quat* parentWorldRotation = &Quat::identity;
	GameObject* parent = gameObject->GetParent();
	if (parent)
	{
		Transform* parentTransform = parent->GetTransform();
		parentWorldRotation = &parentTransform->GetWorldRotation();
	}
	return parentWorldRotation->Normalized();
}

float3 Transform::ParentWorldScale() const
{
	const float3* parentWorldScale = &float3::one;
	GameObject* parent = gameObject->GetParent();
	if (parent)
	{
		Transform* parentTransform = parent->GetTransform();
		parentWorldScale = &parentTransform->GetWorldScale();
	}
	return *parentWorldScale;
}

const float3 & Transform::Up() const
{
	RecalculateIfNecessary();
	return up;
}

const float3 & Transform::Forward() const
{
	RecalculateIfNecessary();
	return forward;
}

const float3 & Transform::Right() const
{
	RecalculateIfNecessary();
	return right;
}

const float3 & Transform::Velocity() const
{
	return velocity;
}

void Transform::Load(const nlohmann::json & json)
{

}

void Transform::Save(nlohmann::json& json) const
{
	/*

	Relevant information:

	type
	localPosition
	localRotation
	localScale

	*/

	json["type"] = type;

	nlohmann::json jsonPosition;
	to_json(jsonPosition, localPosition);

	json["position"] = jsonPosition;

	nlohmann::json jsonRotation;
	to_json(jsonRotation, localRotation);

	json["rotation"] = jsonRotation;

	nlohmann::json jsonScale;
	to_json(jsonScale, localScale);
	
	json["scale"] = jsonScale;
}