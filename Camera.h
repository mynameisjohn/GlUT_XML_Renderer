#pragma once

#include "GL_Includes.h"

#include <fwd.hpp>
#include <mat4x4.hpp>
#include <vec3.hpp>
#include <gtx/quaternion.hpp>

class Camera
{
	friend class Scene;
public:
	enum Type {
		ORTHO,
		PERSP,
		NIL
	};
	Camera();
	~Camera();
	Camera(glm::vec2 X, glm::vec2 Y, glm::vec2 Z);
	Camera(float fovy, float aspect, glm::vec2 nf);
	glm::vec3 getView();
    void translate(glm::vec3 T);
    void rotate(glm::fquat Q);
    glm::mat4 getMat();
	inline glm::mat4 getProj() { return m_m4Proj; }
	inline glm::mat4 * getProjPtr() { return &m_m4Proj; }
	static inline GLint getProjHandle(){ return s_ProjHandle; }
private:
	Type m_Type;
    glm::vec3 m_v3Pos;
    glm::fquat m_qRot;
	glm::mat4 m_m4Proj;
	static GLint s_ProjHandle;
protected:
	static inline void setProjHandle(GLint p){ s_ProjHandle = p; }
};