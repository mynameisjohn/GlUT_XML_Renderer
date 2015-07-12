#pragma once

#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <glm.hpp>

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
	inline glm::mat4 getProj() { return m_m4Proj; }
	inline glm::mat4 * getProjPtr() { return &m_m4Proj; }
	static inline GLint getProjHandle(){ return s_ProjHandle; }
private:
	Type m_Type;
	glm::mat4 m_m4Proj;
	static GLint s_ProjHandle;
protected:
	static inline void setProjHandle(GLint p){ s_ProjHandle = p; }
};