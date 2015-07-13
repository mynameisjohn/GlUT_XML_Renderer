#pragma once

#include <fwd.hpp>

class MouseManager{
public:
	static void Init();
	static void HandleMouseBtn(int button, int state, int x, int y);
    static glm::fquat HandleMouseMove_B(int x, int y);
    static glm::fquat HandleMouseMove_P(int x, int y);
	static const MouseManager * Instance();
private:
	MouseManager();
    static glm::fquat HandleMouseMove(int x, int y, bool button /*= false*/);
	static MouseManager s_Inst;
	int button;
	int lX;
	int lY;
};