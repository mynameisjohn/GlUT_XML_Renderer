#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm.hpp>

class Geometry
{
public:
	Geometry();
	~Geometry();
	inline GLuint getVAO(){ return m_uVAO; }
	inline GLuint getNumIdx(){ return m_nIdx; }
	inline glm::vec4 getColor(){ return m_v4Color; }
	inline glm::mat4 getMV(){ return m_m4MV; }
	inline glm::mat4 * getMVPtr(){ return &m_m4MV; }
private:
	GLuint m_uVAO;
	GLuint m_nIdx;
	glm::vec4 m_v4Color;
	glm::mat4 m_m4MV;
};