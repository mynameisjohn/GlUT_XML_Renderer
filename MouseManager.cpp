#include "MouseManager.h"

#include <math.h>
#include <glm.hpp>
#include <gtc/quaternion.hpp>

MouseManager MouseManager::s_Inst;

MouseManager::MouseManager() :
lX(-1),
lY(-1)
{}

/*static*/ void MouseManager::Init(){
	// Wat do?
}

//public: 
// This should return something useful
/*static*/ void MouseManager::HandleMouseBtn(int button, int state, int x, int y){
	// What do?
	int dX = abs(x - s_Inst.lX);
	int dY = abs(x - s_Inst.lY);
	auto fn = [](float x){
		return 0.5 * x;
	};

	glm::fquat rX(cos(fn(y)), sin(fn(y)), 0, 0);
	glm::fquat rY(cos(fn(x)), 0, sin(fn(x)), 0);
	glm::fquat R = rX*rY;

	s_Inst.lX = x;
	s_Inst.lY = y;
}
/*static*/ void MouseManager::HandleMouseMove_B(int x, int y){
	return HandleMouseMove(x, y, true);
}

/*static*/ void MouseManager::HandleMouseMove_P(int x, int y){
	return HandleMouseMove(x, y, false);
}

/*static*/ void MouseManager::HandleMouseMove(int x, int y, bool button /*= false*/){
	// What do?
	int dX = abs(x - s_Inst.lX);
	int dY = abs(x - s_Inst.lY);
	auto fn = [](float x){
		return 0.5 * x;
	};

	glm::fquat rX(cos(fn(y)), sin(fn(y)), 0, 0);
	glm::fquat rY(cos(fn(x)), 0, sin(fn(x)), 0);
	glm::fquat R = rX*rY;

	s_Inst.lX = x;
	s_Inst.lY = y;
}

/*static*/ const MouseManager * MouseManager::Instance() {
	return (const MouseManager *)&s_Inst;
}