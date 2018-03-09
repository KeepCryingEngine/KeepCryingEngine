#ifndef _GAMEUI_H_
#define _GAMEUI_H_

class GameUI
{
public:
	GameUI(){};
	virtual ~GameUI() {};

	virtual void OnHovering(){};
	virtual void OnClick(){};
	virtual void OnFocus(){};
};

#endif // !_GAMEUI_H_

