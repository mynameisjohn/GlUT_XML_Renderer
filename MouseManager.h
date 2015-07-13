#pragma once

class MouseManager{
public:
	static void Init();
	static void HandleMouseBtn(int button, int state, int x, int y);
	static void HandleMouseMove_B(int x, int y);
	static void HandleMouseMove_P(int x, int y);
	static const MouseManager * Instance();
private:
	MouseManager();
	static void HandleMouseMove(int x, int y, bool button /*= false*/);
	static MouseManager s_Inst;
	int button;
	int lX;
	int lY;
};