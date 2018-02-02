#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include "Module.h"
#include <list>

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;
	bool CleanUp() override;
	update_status Update(float deltaTimeS, float realDeltaTimeS) override;

	GameObject& GetRoot() const;
	void AddToStartList(GameObject& object);

private:
	GameObject* root;
	std::list<GameObject*> startList;


};

#endif // !_MODULESCENE_H_

