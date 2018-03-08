#include "ModuleGameUI.h"

#include <GL/glew.h>

#include <vector>

#include "Application.h"
#include "ModuleWindow.h"

#include "GameObject.h"
#include "Transform2D.h"
#include "Canvas.h"
#include "Image.h"
#include "Texture.h"

using namespace std;

ModuleGameUI::ModuleGameUI()
{}

ModuleGameUI::~ModuleGameUI()
{}

update_status ModuleGameUI::Update(float deltaTimeS, float realDeltaTimeS)
{
	if(root != nullptr)
	{
		UpdateRecursive(deltaTimeS, realDeltaTimeS, root->gameObject);
	}
	return update_status::UPDATE_CONTINUE;
}

bool ModuleGameUI::CleanUp()
{
	return true;
}

void ModuleGameUI::SetCanvas(Canvas& canvas)
{
	root = &canvas;
}

Canvas * ModuleGameUI::GetCanvas()
{
	return root;
}

void ModuleGameUI::UpdateRecursive(float deltaTimeS, float realDeltaTimeS, GameObject * gameObject)
{
	assert(gameObject != nullptr);

	const vector<Component*>& components = gameObject->GetComponents();
	for(vector<Component*>::const_iterator it = components.cbegin(); it != components.cend(); it++)
	{
		Component * component = *it;
		if(!component->enabled)
		{
			continue;
		}

		UpdateComponent(component);
	}

	const vector<GameObject*>& childs = gameObject->GetChildren();
	for(vector<GameObject*>::const_iterator it = childs.cbegin();it != childs.cend();it++)
	{
		GameObject * child = *it;
		UpdateRecursive(deltaTimeS,realDeltaTimeS, child);
	}
}

void ModuleGameUI::UpdateComponent(Component * component)
{
	switch (component->type)
	{
		case Component::Type::Canvas:
		{
			UpdateCanvas((Canvas*)component);
		}
		break;
		case Component::Type::Text:
		{

		}
		break;
		case Component::Type::Button:
		{

		}
		break;
		case Component::Type::InpuText:
		{

		}
		break;
		case Component::Type::Image:
		{
			UpdateImage((Image*)component);
		}
	}
}

void ModuleGameUI::UpdateCanvas(Canvas * canvas)
{
}

void ModuleGameUI::UpdateImage(Image * image)
{
	Transform2D *transform = image->gameObject->GetComponent<Transform2D>();
	assert(transform != nullptr);

	if (image->GetTexture() == nullptr)
	{
		return;
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, App->configuration.screenWidth, 0.0, App->configuration.screenHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();


	glLoadIdentity();
	glDisable(GL_LIGHTING);


	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, image->GetTexture()->GetId());


	// Draw a textured quad
	float3 min = transform->GetMinPosition();
	float3 max = transform->GetMaxPosition();
	glBegin(GL_QUADS);
	
	glTexCoord2f(0, 0); glVertex3f(min.x, min.y, min.z);
	glTexCoord2f(1, 0);  glVertex3f(max.x, min.y, min.z);
	glTexCoord2f(1, 1); glVertex3f(max.x, max.y, min.z);
	glTexCoord2f(0, 1); glVertex3f(min.x, max.y, min.z);
	glEnd();


	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}
