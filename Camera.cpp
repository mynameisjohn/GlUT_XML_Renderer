#include "Camera.h"
#include <gtx/transform.hpp>

GLint Camera::s_ProjHandle(-1);

using glm::normalize;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::fquat;

Camera::Camera()
	: m_Type(Type::NIL),
	m_m4Proj(1),
m_v3Pos(0),
m_qRot(1,0,0,0)
{
}

Camera::~Camera()
{
}

Camera::Camera(glm::vec2 X, glm::vec2 Y, glm::vec2 Z)
	: m_Type(Type::ORTHO),
m_m4Proj(glm::ortho(X[0], X[1], Y[0], Y[1], Z[0], Z[1])),
m_v3Pos(0),
m_qRot(1,0,0,0)
{
}

Camera::Camera(float fovy, float aspect, glm::vec2 nf)
	: m_Type(Type::PERSP),
m_m4Proj(glm::perspective(fovy, aspect, nf[0], nf[1])),
m_v3Pos(0,0,0),
m_qRot(1,0,0,0)
{
}

vec3 Camera::getView(){
    return vec3(m_m4Proj*vec4(0,0,1,1));
}

// TODO bring QuatVecs back, you need a TRT

mat4 Camera::getMat(){
    // I have no idea why this works
    return m_m4Proj*glm::mat4_cast(m_qRot)*glm::translate(m_v3Pos);//*glm::translate(-m_v3Pos);
}

void Camera::rotate(fquat Q){
    m_qRot = glm::normalize(Q * m_qRot);
}
#include <iostream>
using namespace std;
void Camera::translate(vec3 T){
    // The deal here is to take a cartesian translation
    // and make it in our space (front of the camera is +z?)
    // Not gonna lie I don't understand why its the inverse
    // The rotation applied to the camera is the inverse
    // of that applied to the world... ? Whatever, it's expensive
    vec3 Tp(glm::mat4_cast(glm::inverse(m_qRot))*vec4(T,1));
    m_v3Pos += Tp;
}