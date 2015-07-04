#include "Geometry.h"

using glm::mat4;
using glm::vec4;

using namespace std;

Geometry::Geometry()
	:m_uVAO(0),
	m_nIdx(0),
	m_v4Color(1),
	m_m4MV(1)
{}

Geometry::~Geometry(){

}