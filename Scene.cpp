#include "Scene.h"
#include <gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <tinyxml2.h>
using namespace tinyxml2;
using namespace std;

#include "IqmFile.h"

#include <set>

Scene::Scene(){}

Scene::Scene(string XmlSrc){
	XMLDocument doc;
	doc.LoadFile(XmlSrc.c_str());
	map<IqmFile::IQM_T, string> IqmTypes;

	auto check = [](string name, XMLElement * parent){
		XMLElement * ret = parent->FirstChildElement(name.c_str());
		if (!ret){
			cout << "XML Element " << name << " not found. " << endl;
			exit(5);
		}
		return ret;
	};

	auto makeVBO = []
		(GLuint buf, GLint handle, void * ptr, GLsizeiptr numBytes, GLuint dim, GLuint type){
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, numBytes, ptr, GL_STATIC_DRAW);
		glEnableVertexAttribArray(handle);
		glVertexAttribPointer(handle, dim, type, 0, 0, 0);
		//Disable?
	};

	XMLElement * elScene = doc.FirstChildElement("Scene");
	if (!elScene){
		cout << "XML Root not found. " << endl;
		exit(5);
	}
	XMLElement * elCam = check("Camera", elScene);
	XMLElement * elShade = check("Shader", elScene);
	XMLElement * elGeom = check("Geom", elScene);
	
	// First compile and link shader, if that fails what's the point
	// Also check to see if all variables described in XML are present
	
	string vSrc(elShade->Attribute("vert")), fSrc(elShade->Attribute("frag"));
	m_Shader = Shader(vSrc, fSrc);
	m_Shader.Bind();

	// Check all shader variables
	for (auto el = elShade->FirstChildElement(); el; el = el->NextSiblingElement()){
		string type(el->Value()); 
		if (type.compare("Position") == 0){
			string var(el->GetText());
			if (m_Shader[var] < 0){
				cout << "Something bad" << endl;
				exit(6);
			}
			IqmTypes[IqmFile::IQM_T::POSITION] = var;
		}
	}

	// Get scene dims, set up camera
	m_Camera = Camera(3.14f / 4.f, 1.333f, { 1.f, 100.f });

	// Set up geometry
	for (auto el = elGeom->FirstChildElement(); el; el = el->NextSiblingElement()){
		GLuint VAO(0), bIdx(0);
		vector<GLuint> buffers(IqmTypes.size());

		glGenVertexArrays(1, &VAO);
		glGenBuffers(buffers.size(), buffers.data());

		string type(el->Value());
		if (type.compare("IqmFile") == 0){
			string fileName(el->GetText());
			IqmFile iqm(fileName);
			// Do something about all this scoped enum nonsense
			if (IqmTypes.find(IqmFile::IQM_T::POSITION) != IqmTypes.end()){
				auto attr = iqm.getAttr<float>(IqmFile::IQM_T::POSITION);
				makeVBO(buffers[bIdx++], 
					m_Shader[IqmTypes[IqmFile::IQM_T::POSITION]], 
					attr.ptr(), attr.numBytes(), attr.numElems(), GL_FLOAT);
			}
		}
	}
}


Scene::~Scene()
{
}

int Scene::Draw(){
	m_Shader.Bind();
	glUniformMatrix4fv(m_Shader["P"], 1, GL_FALSE, (const GLfloat *)m_Camera.getProjPtr());
	for (auto& geom : m_vGeometry){
		glUniformMatrix4fv(m_Shader["MV"], 1, GL_FALSE, (const GLfloat *)geom.getMVPtr());
		glBindVertexArray(geom.getVAO());
		glDrawElements(GL_TRIANGLES, geom.getNumIdx(), GL_UNSIGNED_INT, NULL);
	}
	glBindVertexArray(0);
	m_Shader.Unbind();

	return m_vGeometry.size();
}
