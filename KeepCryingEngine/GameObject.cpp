#include "GameObject.h"

#include "Globals.h"

#include "Application.h"
#include "ModuleScene.h"

using namespace std;

GameObject::GameObject(const string& name) : name(name)
{
	id = App->scene->GetNewGameObjectId();
}

GameObject::~GameObject()
{ }

GameObject* GameObject::GetParent() const
{
	assert(parent);

	return parent;
}

size_t GameObject::GetChildCount() const
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

GameObject* GameObject::GetChild(unsigned long long int gameObjectId) const
{
	// Check 1st level children first

	for(GameObject* child : children)
	{
		if(child->GetId() == gameObjectId)
		{
			return child;
		}
	}

	// Recursion

	for(GameObject* child : children)
	{
		GameObject* childRecursive = child->GetChild(gameObjectId);
		
		if(childRecursive != nullptr)
		{
			return childRecursive;
		}
	}

	return nullptr;
}

GameObject* GameObject::GetSelfOrChild(unsigned long long int gameObjectId) const
{
	if(id == gameObjectId)
	{
		return (GameObject*)this;
	}

	return GetChild(gameObjectId);
}

const string& GameObject::GetName() const
{
	return name;
}

unsigned long long int GameObject::GetId() const
{
	return id;
}

/* void GameObject::AddChild(GameObject& newChild)
{
	newChild.SetParent(*this);
} */

void GameObject::SetParent(GameObject& newParent)
{
	if(parent != nullptr)
	{
		vector<GameObject*>::iterator childParentIterator = find(parent->children.begin(), parent->children.end(), this);
		assert(childParentIterator != parent->children.end());
		parent->children.erase(childParentIterator);
	}
	
	parent = &newParent;
	newParent.children.push_back(this);
}

void GameObject::Update(float deltaTimeS, float realDeltaTimeS)
{
	CheckToStart();
	CheckToDestroy();

	for(Component* component : components)
	{
		/* if(component->IsEnabled())
		{
			component->Update(deltaTimeS, realDeltaTimeS);
		} */
	}
}

void GameObject::OnDestroy()
{
	for(Component* component : components)
	{
		// component->Destroy();
	}

	for(Component* component : components)
	{
		RELEASE(component);
	}

	components.clear();
}

template<class T>
T* GameObject::AddComponent()
{
	/*

	component->SetGameObject(this);
	component->Awake();

	toStart.push_back(component);

	*/

	return nullptr;
}

//template<class T>
//T* GameObject::GetComponent()
//{
//	
//	//for(Component* component : components)
//	//{
//	//	if(component->type == type)
//	//	{
//	//		return component;
//	//	}
//	//}
//
//	//return nullptr;
//
//	return nullptr;
//}

template<class T>
std::vector<T*> GameObject::GetComponents()
{
	return std::vector<T*>();
}

template<class T>
void GameObject::GetComponents(std::vector<T*>& components)
{

}

/* Component& GameObject::AddComponent(ComponentType type)
{
	Component* component = nullptr;
	//component = ComponentFabric::CreateComponent(type);
	assert(component);
	components.push_back(component);
	return *component;
} */

/* Component* GameObject::GetComponent(ComponentType type)
{
	for(Component* c : components)
	{
		if(c->type == type)
		{
			return c;
		}
	}
	return nullptr;
} */

/* vector<Component*> GameObject::GetComponents(ComponentType type)
{
	vector<Component*> ret;
	for (Component* c : components) 
	{
		if (c->type == type) 
		{
			ret.push_back(c);
		}
	}
	return ret;
} */

void GameObject::CheckToStart()
{
	for(Component* component : toStart)
	{
		// component->Start();

		components.push_back(component);
	}

	toStart.clear();
}

void GameObject::CheckToDestroy()
{
	for(Component* component : toDestroy)
	{
		components.erase(find(components.begin(), components.end(), component));

		// component->Destroy();
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

/* void GameObject::DestroyAndRelease(Component* component) const
{
	// component->Destroy();

	RELEASE(component);
} */