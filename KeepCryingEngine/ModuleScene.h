#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include <list>

#include "Module.h"

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene();
	virtual ~ModuleScene();

	bool Init() override;
	
	update_status PreUpdate(float deltaTimeS, float realDeltaTimeS) override;
	update_status Update(float deltaTimeS, float realDeltaTimeS) override;

	bool CleanUp() override;

	unsigned long long int GetNewGameObjectId();

	GameObject* GetRoot() const;

	GameObject* Get(unsigned long long int gameObjectId) const;

	GameObject* AddEmpty(GameObject& parent, const char* name = "Empty");

	GameObject* AddCube(GameObject& parent);
	GameObject* AddSphere(GameObject& parent);

	void Add(GameObject& gameObject);
	void Destroy(GameObject& gameObject);

private:
	void CheckToStart();
	void CheckToDestroy();

	void Update(GameObject* gameObject, float deltaTimeS, float realDeltaTimeS) const;

	void DestroyAndRelease(GameObject* gameObject) const;

private:
	GameObject* root = nullptr;

	std::list<GameObject*> toStart;
	std::list<GameObject*> toDestroy;

	unsigned long long int currentGameObjectId = 0;
};

#endif // !_MODULESCENE_H_