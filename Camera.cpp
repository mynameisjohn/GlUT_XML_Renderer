#include "Camera.h"
#include <gtx/transform.hpp>

GLint Camera::s_ProjHandle(-1);

using glm::normalize;
using glm::vec3;
using glm::vec4;
using glm::mat4;

Camera::Camera()
	: m_Type(Type::NIL),
	m_m4Proj(1)
{
}

Camera::~Camera()
{
}

Camera::Camera(glm::vec2 X, glm::vec2 Y, glm::vec2 Z)
	: m_Type(Type::ORTHO),
	m_m4Proj(glm::ortho(X[0], X[1], Y[0], Y[1], Z[0], Z[1]))
{
}

Camera::Camera(float fovy, float aspect, glm::vec2 nf)
	: m_Type(Type::PERSP),
	m_m4Proj(glm::perspective(fovy, aspect, nf[0], nf[1]))
{
}

glm::vec3 Camera::getView(){
	return normalize(vec3(m_m4Proj * vec4(0, 0, 1, 1)));
}