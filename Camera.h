#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm.hpp>

class Camera
{
public:
	Camera();
	~Camera();
	Camera(glm::vec2 X, glm::vec2 Y, glm::vec2 Z);
	Camera(float fovy, float aspect, glm::vec2 nf);
	inline glm::mat4 getProj() { return m_m4Proj; }
	inline glm::mat4 * getProjPtr() { return &m_m4Proj; }
private:
	glm::mat4 m_m4Proj;
};