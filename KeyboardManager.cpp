#include "KeyboardManager.h"

KeyboardManager KeyboardManager::s_Inst;

KeyboardManager::KeyboardManager() :
vKeyState(512, false)
{}

/*static*/ void KeyboardManager::Init(){
	// Wat do?
}

//public: 
// This should return something useful
/*static*/ void KeyboardManager::HandleKey(unsigned char key, int x, int y){
	// Wat do?
	s_Inst.vKeyState[unsigned int(key)] = !s_Inst.vKeyState[unsigned int(key)];
}

/*static*/ const KeyboardManager * KeyboardManager::Instance(){
	return (const KeyboardManager *)&s_Inst;
}