#pragma once

#include <vector>

class KeyboardManager{
public:
	static void Init();
	static void HandleKey(unsigned char key, int x, int y);
	static const KeyboardManager * Instance();
private:
	KeyboardManager();
	static KeyboardManager s_Inst;
	std::vector<bool> vKeyState;
};