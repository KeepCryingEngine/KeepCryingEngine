#ifndef _MESHRENDERER_H_
#define _MESHRENDERER_H_

#include "Component.h"
class MeshRenderer : public Component
{
public:
	MeshRenderer();
	~MeshRenderer();

	void DrawUI() override;
	std::vector<ComponentType> GetProhibitedComponents() const override;
};

#endif // !_MESHRENDERER_H_



