#include "Transform.h"

#include <MathGeoLib.h>
#include <ImGuizmo.h>

#include "GameObject.h"

using namespace std;

Transform::Transform() : 
	Component(ComponentType::Transform),
	localPosition(float3::zero),
	localRotation(Quat::identity),
	eulerLocalRotation(float3::zero),
	localScale(float3::one),
	worldPosition(float3::zero),
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
		}
	}
}

vector<ComponentType> Transform::GetProhibitedComponents() const
{
	return { ComponentType::Transform };
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

/*void Transform::SetLocalMatrix(const float4x4 & localMatrix)
{
	if (!gameObject->IsStatic())
	{
		SetDirty();
		localMatrix.Decompose(localPosition, localRotation, localScale);
		localRotation.Normalize();
	}
}*/

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

/*void Transform::SetModelMatrix(const float4x4 & matrix)
{
	if (!gameObject->IsStatic())
	{
		float3 position, scale;
		Quat rotation;
		matrix.Decompose(position, rotation, scale);
		
		modelMatrix = matrix;
		SetWorldPosition(position);
		SetWorldRotation(rotation);
		SetWorldScale(scale);
	}
}*/

/*
	Set local variables instead of world ones
	Then we calculate the world variables from the local ones when they are asked for
*/
void Transform::SetWorldPosition(const float3 & position)
{
	if(!gameObject->IsStatic())
	{
		SetDirty();
		float3 worldPositionDelta = position - worldPosition;
		localPosition = localPosition + worldPositionDelta;
	}
}

void Transform::SetWorldRotation(const Quat & rotation)
{
	if(!gameObject->IsStatic())
	{
		SetDirty();
		Quat worldRotationDelta = rotation.Mul(worldRotation.Inverted());
		worldRotationDelta.Normalize();
		worldRotation = localRotation.Mul(worldRotationDelta);
		worldRotation.Normalize();
	}
}

void Transform::SetWorldScale(const float3 & scale)
{
	if(!gameObject->IsStatic())
	{
		SetDirty();
		float3 worldScaleDelta = scale - worldScale;
		localScale = localScale + worldScaleDelta;
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

void Transform::GuizmoSetModelMatrix(const float4x4 & setmodelMatrix, const float3& position, const float3& rotation, const float3& scale)
{
	if(!gameObject->IsStatic())
	{
		modelMatrix = setmodelMatrix;

		SetLocalPosition(position);

		eulerLocalRotation = rotation;
		float3 radAngles = DegToRad(rotation);
		SetLocalRotation(Quat::FromEulerXYZ(radAngles.x, radAngles.y, radAngles.z));
		eulerLocalRotation = rotation;

		SetLocalScale(scale);
	}
}

void Transform::Recalculate()
{
	SetDirty();
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
		
		/*modelMatrix.Decompose(worldPosition, worldRotation, worldScale);
		worldRotation.Normalize();
		eulerLocalRotation = RadToDeg(localRotation.ToEulerXYZ());*/

		/*float3 eulerWorldRotation;
		ImGuizmo::DecomposeMatrixToComponents(modelMatrix.Transposed().ptr(), worldPosition.ptr(), eulerWorldRotation.ptr(), worldScale.ptr());
		eulerWorldRotation = DegToRad(eulerWorldRotation);
		worldRotation = Quat::FromEulerXYZ(eulerWorldRotation.x, eulerWorldRotation.y, eulerWorldRotation.z);*/

		dirty = false;
	}
}

float4x4 Transform::CalculateModelMatrix() const
{
	float4x4 localMatrix = GetLocalMatrix();
	const float4x4& parentMatrix = GetParentMatrix();
	return parentMatrix * localMatrix;
}

float3 Transform::CalculateWorldPosition() const
{
	const float3* worldPosition = &localPosition;
	GameObject* parent = gameObject->GetParent();
	if (parent)
	{
		Transform* parentTransform = parent->GetTransform();
		float3 calculatedWorldPosition = parentTransform->GetModelMatrix().Mul(localPosition.ToPos4()).xyz();
		worldPosition = &calculatedWorldPosition;
	}
	return *worldPosition;
}

Quat Transform::CalculateWorldRotation() const
{
	const Quat* worldRotation = &localRotation;
	GameObject* parent = gameObject->GetParent();
	if (parent)
	{
		Transform* parentTransform = parent->GetTransform();
		Quat calculatedWorldRotation = parentTransform->GetWorldRotation().Mul(localRotation);
		worldRotation = &calculatedWorldRotation;
	}
	return *worldRotation;
}

float3 Transform::CalculateWorldScale() const
{
	const float3* worldScale = &localScale;
	GameObject* parent = gameObject->GetParent();
	if (parent)
	{
		Transform* parentTransform = parent->GetTransform();
		float3 calculatedWorldScale = parentTransform->GetWorldScale() + localScale;
		worldScale = &calculatedWorldScale;
	}
	return *worldScale;
}
