#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "Component.h"

#include <float4.h>
#include <vector>
#include <experimental/filesystem>

class Texture;

enum class ButtonState
{
	HOVER,
	PRESSED,
	DISABLED,
	NUMBER_STATES
};

enum class  Transition
{
	COLOR,
	SPRITE
};

class GameObject;

class Button : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Button;

	Button();
	~Button();

	void Awake() override;
	void Destroy() override;

	void RealUpdate(float deltaTimeS, float realDeltaTimeS)override;

	void DrawUI() override;

	void OnHovering();
	void OnClick();

	void SetTextureByPath(const std::experimental::filesystem::path& path,ButtonState state);
	void SetColor(float4 newColor,ButtonState state);
	void SetTextGameObject(GameObject& g);

	Texture * GetTexture(ButtonState state) const;
	float4 GetColor(ButtonState state)const;
	GameObject* GetTextGameObject() const;

private:
	ButtonState state = ButtonState::DISABLED;
	Transition transitionType = Transition::COLOR;
	float4 colors[(unsigned int)ButtonState::NUMBER_STATES];
	Texture* textures[(unsigned int)ButtonState::NUMBER_STATES];
	GameObject* textGameObject;
};

#endif // !_BUTTON_H_

