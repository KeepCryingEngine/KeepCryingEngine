#include "GameObject.h"

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ComponentFabric.h"
#include "Transform.h"
#include "ModuleEditorUI.h"

using namespace std;

GameObject::GameObject(const string& name) : name(name)
{
	id = App->scene->GetNewGameObjectId();
	transform = AddComponent<Transform>();
	App->scene->AddToDinamicGameobjectList(this);

	aabb.SetNegativeInfinity();
}

GameObject::~GameObject()
{ }

GameObject* GameObject::GetParent() const
{
	return parent;
}

void GameObject::SetParent(GameObject& newParent)
{
	if (parent != nullptr)
	{
		parent->RemoveChild(*this);
	}

	parent = &newParent;
	newParent.children.push_back(this);
}

void GameObject::RemoveChild(GameObject& child)
{
	vector<GameObject*>::iterator childParentIterator = find(children.begin(), children.end(), &child);
	assert(childParentIterator != children.end());
	children.erase(childParentIterator);
	child.parent = nullptr;
}


size_t GameObject::ChildCount() const
{
	return children.size();
}

GameObject* GameObject::GetChild(size_t index) const
{
	assert(children.size() > index);

	return children[index];
}

const std::vector<GameObject*>& GameObject::GetChildren() const
{
	return children;
}

GameObject* GameObject::GetById(unsigned long long int gameObjectId) const
{
	GameObject* gameObject = nullptr;
	if(id == gameObjectId)
	{
		gameObject = (GameObject*)this;
	}
	else
	{
		for (GameObject* child : children)
		{
			gameObject = child->GetById(gameObjectId);

			if (gameObject != nullptr)
			{
				break;
			}
		}
	}
	return gameObject;
}

void GameObject::DeleteChild(GameObject & childToRemove)
{
	vector<GameObject*>::iterator childParentIterator = find(this->children.begin(), this->children.end(), &childToRemove);
	if(childParentIterator != this->children.end())
	{
		this->children.erase(childParentIterator);
	}
}

const string& GameObject::GetName() const
{
	return name;
}

int GameObject::GetId() const
{
	return id;
}

const bool GameObject::IsEnabled() const
{
	return enable;
}

const bool GameObject::IsStatic() const
{
	return isStatic;
}

void GameObject::SetStatic(bool value)
{
	if(isStatic == value)
	{
		return;
	}

	isStatic = value;

	ModuleScene* scene = App->scene;

	if(value)
	{
		scene->AddStatic(this);

		GameObject* root = scene->GetRoot();
		GameObject* actualParent = parent;

		if(parent != App->scene->GetRoot())
		{
			parent->SetStatic(value);
		}
	}
	else
	{
		scene->RemoveStatic(this);

		for(GameObject* g : children)
		{
			g->SetStatic(value);
		}
	}
}

void GameObject::Update()
{
	CheckToStart();
	CheckToDestroy();

	if(enable)
	{
		for(Component* component : components)
		{
			component->Update();
		}
	}
}

void GameObject::OnDestroy()
{
	//if(isStatic)
	//{
	//	App->scene->RemoveStatic(this);
	//}

	for(Component* component : components)
	{
		component->Destroy();
	}

	for(Component* component : components)
	{
		RELEASE(component);
	}

	components.clear();
}

void GameObject::SetAABB(const AABB & newAABB)
{
	aabb = newAABB;
}

AABB & GameObject::GetAABB()
{
	return aabb;
}

bool GameObject::GetVisible() const
{
	return visible;
}

void GameObject::SetVisible(bool visible)
{
	this->visible = visible;
}

void GameObject::CheckIfFocuseableUI()
{
	for(Component* const c : GetComponents())
	{
		if(c->IsFocuseableUI())
		{
			isFocuseableUI = true;
			return;
		}
	}
	isFocuseableUI = false;
}

void GameObject::SetFocuseableUI(bool value)
{
	isFocuseableUI = value;
}

bool GameObject::IsFocuseableUI() const
{
	return isFocuseableUI;
}

void GameObject::CheckIfHovereableUI()
{
	for(Component* const c : GetComponents())
	{
		if(c->IsHovereableUI())
		{
			isHovereableUI = true;
			return;
		}
	}
	isHovereableUI = false;
}

void GameObject::SetHovereableUI(bool value)
{
	isHovereableUI = value;
}

bool GameObject::IsHovereableUI() const
{
	return isHovereableUI;
}

const ENGINE_UUID & GameObject::UUID() const
{
	return uuid;
}

Component* GameObject::GetComponent(Component::Type type) const
{
	for (Component* component : components)
	{
		if (component->type == type)
		{
			return component;
		}
	}

	for (Component* component : toStart)
	{
		if (component->type == type)
		{
			return component;
		}
	}

	return nullptr;
}

std::vector<Component*> GameObject::GetComponents(Component::Type type) const
{
	std::vector<Component*> ret;

	for (Component* component : components)
	{
		if (component->type == type)
		{
			ret.push_back(component);
		}
	}

	for (Component* component : toStart)
	{
		if (component->type == type)
		{
			ret.push_back(component);
		}
	}

	return ret;
}

std::vector<Component*> GameObject::GetComponentsInChildren(Component::Type type) const
{
	vector<Component*> components = GetComponents(type);
	for (GameObject* child : children)
	{
		vector<Component*> childComponents = child->GetComponentsInChildren(type);
		components.insert(components.end(),childComponents.begin(), childComponents.end());
	}
	return components;
}

Component* GameObject::AddComponent(Component::Type type)
{
	Component* component = ComponentFabric::CreateComponent(type);
	assert(component);

	if(CanAttach(*component))
	{
		AddInternalComponent(component);

		for (Component::Type neededComponent : component->GetNeededComponents())
		{
			if (GetComponent(neededComponent) == nullptr)
			{
				AddComponent(neededComponent);
			}
		}
	}
	else
	{
		RELEASE(component);
	}

	return component;
}

void GameObject::RemoveComponent(Component * component)
{
	/*if(component->type == ComponentType::MeshRenderer)
	{
		Component* mat = GetComponent(ComponentType::MeshFilter);
		assert(mat);
		vector<Component*>::iterator it = find(components.begin(), components.end(), mat);
		if(it != components.end())
		{
			toDestroy.push_back(mat);
		}
	}*/

	vector<Component*>::iterator it = find(components.begin(), components.end(), component);
	if (it != components.end()) 
	{
		toDestroy.push_back(component);
	}
}

const std::vector<Component*>& GameObject::GetComponents() const
{
	return components;
}

Transform * GameObject::GetTransform() const
{
	return transform;
}

void GameObject::DrawUI()
{
	if(ImGui::Checkbox("", &enable))
	{
		for(Component* c : components)
		{
			c->SetEnable(enable);
		}
	}

	ImGui::SameLine();

	static char buffer[180] = {};
	strcpy_s(buffer, name.c_str());
	if(ImGui::InputText("##label", buffer, sizeof(buffer)))
	{
		name = buffer;
	}

	ImGui::SameLine();

	bool tempStatic = IsStatic();
	if(ImGui::Checkbox("Static", &tempStatic))
	{
		SetStatic(tempStatic);
	}
	
	ImGui::NewLine();

	for(Component* c : components)
	{
		c->DrawUI();
	}

	ImGui::NewLine();
}

void GameObject::CheckToStart()
{
	for(Component* component : toStart)
	{
		component->Start();

		components.push_back(component);
	}

	toStart.clear();
}

void GameObject::CheckToDestroy()
{
	for(Component* component : toDestroy)
	{
		vector<Component*>::iterator it = find(components.begin(), components.end(), component);
		assert(it != components.end());
		components.erase(it);

		component->Destroy();
	}

	for(Component* component : toDestroy)
	{
		RELEASE(component);
	}

	/* for(Component* component : toDestroy)
	{
		components.erase(find(components.begin(), components.end(), component));

		DestroyAndRelease(component);
	} */

	toDestroy.clear();
}

void GameObject::AddInternalComponent(Component * component)
{
	component->gameObject = this;
	component->Awake();

	toStart.push_back(component);
}

bool GameObject::CanAttach(const Component& component) const
{
	for(Component::Type componentType : component.GetProhibitedComponents())
	{
		if(GetComponent(componentType) != nullptr)
		{
			return false;
		}
	}

	return true;
}

/* void GameObject::DestroyAndRelease(Component* component) const
{
	// component->Destroy();

	RELEASE(component);
} */