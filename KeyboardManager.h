#pragma once

#include <vector>

#include <fwd.hpp>

class KeyboardManager{
public:
	static void Init();
    static glm::vec3 HandleKey(unsigned char key, int x, int y);
	static const KeyboardManager * Instance();
private:
	KeyboardManager();
	static KeyboardManager s_Inst;
	std::vector<bool> vKeyState;
};