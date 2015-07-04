#pragma once

#include "Camera.h"
#include "Shader.h"
#include "Geometry.h"

#include <vector>
using GeomVec = std::vector < Geometry >;

class Scene
{
public:
	Scene();
	Scene(std::string XmlSrc);
	~Scene();
	int Draw();
private:
	Camera m_Camera;
	Shader m_Shader;
	GeomVec m_vGeometry;
};

