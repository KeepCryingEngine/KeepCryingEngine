#include "ModuleGameUI.h"

#include <GL/glew.h>

#include <vector>
#include <SDL.h>

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "Transform2D.h"
#include "Canvas.h"
#include "Image.h"
#include "Button.h"
#include "Text.h"
#include "InputText.h"
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
		CheckUIStatus();
		glDisable(GL_DEPTH_TEST);
		UpdateRecursivePreOrder(deltaTimeS, realDeltaTimeS, root->gameObject);
		glEnable(GL_DEPTH_TEST);
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

GameObject * ModuleGameUI::GetFocusGameObject()
{
	return focus;
}

GameObject * ModuleGameUI::GetHoveringGameObject()
{
	return hovering;
}

void ModuleGameUI::UpdateRecursivePreOrder(float deltaTimeS, float realDeltaTimeS, GameObject * gameObject)
{
	assert(gameObject != nullptr);

	if(gameObject->ChildCount() == 0)
	{
		UpdateNode(deltaTimeS, realDeltaTimeS, gameObject);
		return;
	}
	UpdateNode(deltaTimeS, realDeltaTimeS, gameObject);

	const vector<GameObject*>& childs = gameObject->GetChildren();
	for(vector<GameObject*>::const_iterator it = childs.cbegin();it != childs.cend();it++)
	{
		GameObject * child = *it;
		UpdateRecursivePreOrder(deltaTimeS,realDeltaTimeS, child);
	}
}

void ModuleGameUI::UpdateNode(float deltaTimeS, float realDeltaTimeS, GameObject * g)
{
	assert(g != nullptr);
	const vector<Component*>& components = g->GetComponents();
	for(vector<Component*>::const_iterator it = components.cbegin(); it != components.cend(); it++)
	{
		Component * component = *it;
		if(!component->enabled)
		{
			continue;
		}

		UpdateComponent(component);
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
			UpdateText((Text*)component);
		}
		break;
		case Component::Type::Button:
		{
			UpdateButton((Button*)component);
		}
		break;
		case Component::Type::InpuText:
		{
			UpdateInputText((InputText*)component);
		}
		break;
		case Component::Type::Image:
		{
			UpdateImage((Image*)component);
		}
	}
}

void ModuleGameUI::CheckUIStatus()
{
	pressed = nullptr;
	hovering = nullptr;

	PreOrdenZCheck(root->gameObject);
	//Tab functionality
	if(focus != nullptr && App->input->UIGetKey(SDL_SCANCODE_TAB) == KeyState::KEY_DOWN)
	{
		if(!alreadyPressed)
		{
			NextFocus();
			alreadyPressed = true;
		}
	}
	else if(App->input->UIGetKey(SDL_SCANCODE_TAB) == KeyState::KEY_UP)
	{
		alreadyPressed = false;
	}

	if(hovering != nullptr)
	{
		if(App->input->GetMouseButtonDown(SDL_BUTTON_LEFT))
		{
			if(hovering->IsFocuseableUI())
			{
				focus = hovering;
				pressed = hovering;
			}
			else
			{
				focus = nullptr;
			}
		}
	}
	//We are still on UI, then we absorb input
	if(hovering != nullptr || focus != nullptr)
	{
		App->input->SetOverUI(true);
	}
}

void ModuleGameUI::PreOrdenZCheck(GameObject * currentNode)
{
	if(currentNode->ChildCount() == 0)
	{
		if(currentNode->IsHovereableUI())
		{
			if(CheckIfMouseOver(currentNode))
			{
				hovering = currentNode;
			}
		}
		return;
	}
	if(currentNode->IsHovereableUI())
	{
		if(CheckIfMouseOver(currentNode))
		{
			hovering = currentNode;
		}
	}
	vector<GameObject*> children = currentNode->GetChildren();
	for(GameObject* g : children)
	{
		PreOrdenZCheck(g);
	}
}

bool ModuleGameUI::CheckIfMouseOver(GameObject * g)
{
	float2 mousePos = App->input->GetMousePosition();
	float3 maxPos = g->GetComponent<Transform2D>()->GetMaxPosition();
	float3 minPos = g->GetComponent<Transform2D>()->GetMinPosition();

	bool xHit = true;
	bool yHit = true;

	if(mousePos.x > maxPos.x || mousePos.x < minPos.x)
	{
		xHit = false;
	}

	if(mousePos.y > maxPos.y || mousePos.y < minPos.y)
	{
		yHit = false;
	}

	return (xHit && yHit);
}

void ModuleGameUI::NextFocus()
{
	firstFocusAvailable = nullptr;
	firstFocusAvailableFlag = true;
	nextItsFocusFlag = false;
	if(!NextFocusPreOrderZCheck(root->gameObject))
	{
		focus = firstFocusAvailable;
	}
}

bool ModuleGameUI::NextFocusPreOrderZCheck(GameObject * currentNode)
{
	if(currentNode->ChildCount() == 0)
	{
		if(firstFocusAvailableFlag)
		{
			firstFocusAvailable = currentNode;
			firstFocusAvailableFlag = false;
		}

		if(currentNode->IsFocuseableUI())
		{
			if(focus == currentNode)
			{
				nextItsFocusFlag = true;
			}
			else if(nextItsFocusFlag)
			{
				focus = currentNode;
				return true;
			}
		}
		return false;
	}
	if(currentNode->IsFocuseableUI())
	{
		if(firstFocusAvailableFlag)
		{
			firstFocusAvailable = currentNode;
			firstFocusAvailableFlag = false;
		}

		if(focus == currentNode)
		{
			nextItsFocusFlag = true;
		}
		else if(nextItsFocusFlag)
		{
			focus = currentNode;
			return true;
		}
	}
	bool ret = false;
	vector<GameObject*> children = currentNode->GetChildren();
	for(GameObject* g : children)
	{
		ret = ret || NextFocusPreOrderZCheck(g);
		if(ret)
		{
			return ret;
		}
	}
	return ret;
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

	float4 color = image->GetColor();
	glColor4f(color.x,color.y,color.z,color.w);
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

	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}

void ModuleGameUI::UpdateButton(Button * button)
{
	if(button->gameObject == pressed)
	{
		button->OnClick();
	}
	else if(button->gameObject == hovering || button->gameObject == focus)
	{
		button->OnHovering();
	}
}

void ModuleGameUI::UpdateText(Text * text)
{
	Transform2D *transform = text->gameObject->GetComponent<Transform2D>();
	assert(transform != nullptr);

	if(text->GetTexture() == nullptr)
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

	//float4 color = image->GetColor();
	//glColor4f(color.x, color.y, color.z, color.w);
	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, text->GetTexture()->GetId());


	// Draw a textured quad
	float3 min = transform->GetMinPosition();
	float3 max = transform->GetMaxPosition();
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0); glVertex3f(min.x, min.y, min.z);
	glTexCoord2f(1, 0);  glVertex3f(max.x, min.y, min.z);
	glTexCoord2f(1, 1); glVertex3f(max.x, max.y, min.z);
	glTexCoord2f(0, 1); glVertex3f(min.x, max.y, min.z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}

void ModuleGameUI::UpdateInputText(InputText * inputText)
{
	if(inputText->gameObject == focus)
	{
		inputText->OnFocus();
	}
}
