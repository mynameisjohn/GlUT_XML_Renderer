#include "Light.h"

using glm::vec3;

Light::Light()
	:m_Type(Type::NIL)
{}

Light::~Light(){}

Light::Light(vec3 pos, vec3 dir, vec3 intensity)
	: m_Type(Type::DIRECTIONAL),
	m_Pos(pos),
	m_Dir(dir),
	m_Intensity(intensity)
{}