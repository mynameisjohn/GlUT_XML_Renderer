#include "Scene.h"
#include "IqmFile.h"

#include <string>
#include <iostream>
using namespace std;

#include <tinyxml2.h>
using namespace tinyxml2;

#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

// TODO : Reserve IqmFile::IQM_T::CUSTOM for extras, make a multimap
using IqmTypeMap = map < IqmFile::IQM_T, GLint > ;

// Implementations below
static string getGeom(XMLElement& elGeom, Geometry& geom);
static IqmTypeMap getShader(XMLElement& elShade, Shader& shader);
static Camera::Type getCamera(XMLElement& elCam, Camera& cam);
static Light::Type getLight(XMLElement& elLight, Light& l, vec3 view);
static void createGPUAssets(IqmTypeMap iqmTypes, Geometry& geom, string fileName);
static void createGPUAssets(GLint handles[4], Light& l);

// tinyxml returns null if not found; my attempt at handling it here
static inline float safeAtoF(XMLElement& el, string query){
	const char * buf = el.Attribute(query.c_str());
	if (buf)
		return atof(buf);
	else{
		cout << "Error: null char ptr passed to atof" << endl;
		// Should I really exit here?
		exit(9);
	}
}

// TODO: Move constructor...
Scene::Scene(){}

Scene::Scene(string XmlSrc){
	XMLDocument doc;
	doc.LoadFile(XmlSrc.c_str());

	auto check = [](string name, XMLElement * parent){
		XMLElement * ret = parent->FirstChildElement(name.c_str());
		if (!ret){
			cout << "XML Element " << name << " not found. " << endl;
			exit(5);
		}
		return ret;
	};

	XMLElement * elScene = doc.FirstChildElement("Scene");
	if (!elScene){
		cout << "XML Root not found. " << endl;
		exit(5);
	}
	XMLElement * elCam = check("Camera", elScene);
	XMLElement * elShade = check("Shader", elScene);
	XMLElement * elGeom = check("Geom", elScene);
	XMLElement * elLight = check("Light", elScene);

	Camera::Type camType = getCamera(*elCam, m_Camera);
	if (camType == Camera::Type::NIL){
		cout << "Error creating Camera" << endl;
		exit(7);
	}

	IqmTypeMap iqmTypes = getShader(*elShade, m_Shader);
	if (iqmTypes.empty()){
		cout << "Error: no attributes found in shader" << endl;
		exit(8);
	}
	// Bind shader, create GPU assets for geometry
	auto sBind = m_Shader.S_Bind();

	for (auto el = elLight->FirstChildElement(); el; el = el->NextSiblingElement()){
		Light l;
		getLight(*el, l, m_Camera.getView());
		string s = "L[i].";
		s[2] = '0' + m_vLights.size();
		GLint handles[4] = { 
			m_Shader[s + "type"],
			m_Shader[s + "pos"],
			m_Shader[s + "dir"],
			m_Shader[s + "intensity"],
		};
		createGPUAssets(handles, l);
		m_vLights.push_back(l);
	}

	// Grab MV, P handles (make this better)
	GLint MVHandle = m_Shader["MV"], PHandle = m_Shader["P"];
	Camera::setProjHandle(PHandle);
	Geometry::setMVHandle(MVHandle);

	for (auto el = elGeom->FirstChildElement(); el; el = el->NextSiblingElement()){
		Geometry g;
		string fileName = getGeom(*el, g);
		createGPUAssets(iqmTypes, g, fileName);
		m_vGeometry.push_back(g);
	}
}

Scene::~Scene()
{
}

int Scene::Draw(){
	auto sBind = m_Shader.S_Bind();
	glUniformMatrix4fv(m_Camera.getProjHandle(), 1, GL_FALSE, (const GLfloat *)m_Camera.getProjPtr());
	for (auto& geom : m_vGeometry){
		glUniformMatrix4fv(Geometry::getMVHandle(), 1, GL_FALSE, (const GLfloat *)geom.getMVPtr());
		glBindVertexArray(geom.getVAO());
		glDrawElements(GL_TRIANGLES, geom.getNumIdx(), GL_UNSIGNED_INT, NULL);
	}
	glBindVertexArray(0);

	return m_vGeometry.size();
}

static string getGeom(XMLElement& elGeom, Geometry& geom){

	// Get Color, Translate, Scale, Rotate from XML - Could easily segfault here...
	vec4 C(safeAtoF(elGeom, "Cr"), safeAtoF(elGeom, "Cg"), safeAtoF(elGeom, "Cb"), safeAtoF(elGeom, "Ca"));
	vec3 T(safeAtoF(elGeom, "Tx"), safeAtoF(elGeom, "Ty"), safeAtoF(elGeom, "Tz"));
	vec3 S(safeAtoF(elGeom, "Sx"), safeAtoF(elGeom, "Sy"), safeAtoF(elGeom, "Sz"));
	vec3 R(safeAtoF(elGeom, "Rx"), safeAtoF(elGeom, "Ry"), safeAtoF(elGeom, "Rz"));
	float rot = safeAtoF(elGeom, ("R"));
	mat4 MV = glm::translate(T) * glm::rotate(rot, R) * glm::scale(S);

	geom.setColor(C);
	geom.leftMultMV(MV);

	// Should I load the file into memory here?
	string iqmFileName = elGeom.GetText();

	return iqmFileName;
}

static IqmTypeMap getShader(XMLElement& elShade, Shader& shader){
	IqmTypeMap ret;

	// Also check to see if all variables described in XML are present
	string vSrc(elShade.Attribute("vert")), fSrc(elShade.Attribute("frag"));
	shader = Shader(vSrc, fSrc);
	auto sBind = shader.S_Bind();

	// Check all shader variables
	for (auto el = elShade.FirstChildElement(); el; el = el->NextSiblingElement()){
		string type(el->Value());
		if (type.compare("Position") == 0){
			string var(el->GetText());
			GLint handle = shader[var];
			if (handle < 0){
				cout << "Something bad" << endl;
				exit(6);
			}
			// Should I have the shader ensure it's an attribute?
			ret[IqmFile::IQM_T::POSITION] = handle;
		}
		else if (type.compare("Normal") == 0){
			string var(el->GetText());
			GLint handle = shader[var];
			if (handle < 0){
				cout << "Something bad" << endl;
				exit(6);
			}
			// Should I have the shader ensure it's an attribute?
			ret[IqmFile::IQM_T::NORMAL] = handle;
		}
	}

	return ret;
}

static Camera::Type getCamera(XMLElement& elCam, Camera& cam){
	// See if ortho
	auto test = elCam.Attribute("left");
	if (!test){
		test = elCam.Attribute("fovy");
		if (!test)
			return Camera::Type::NIL;

		// Handle persp case
		float fovy = safeAtoF(elCam, "fovy");
		float aspect = safeAtoF(elCam, "aspect");
		vec2 NF(safeAtoF(elCam, "near"), safeAtoF(elCam, "far"));
		cam = Camera(fovy, aspect, NF);
		return Camera::Type::PERSP;
	}

	// Get scene dims, set up camera
	vec2 LR(safeAtoF(elCam, "left"), safeAtoF(elCam, "right"));
	vec2 BT(safeAtoF(elCam, "bottom"), safeAtoF(elCam, "top"));
	vec2 NF(safeAtoF(elCam, "near"), safeAtoF(elCam, "far"));
	cam = Camera(LR, BT, NF);

	return Camera::Type::ORTHO;
}

static Light::Type getLight(XMLElement& elLight, Light& l, vec3 view){
	Light::Type ret(Light::Type::NIL);
	vec3 pos, dir(1), intensity;

	string lType(elLight.Value());
	if (lType.compare("Directional") == 0){
		dir = vec3(safeAtoF(elLight, "dX"), safeAtoF(elLight, "dY"), safeAtoF(elLight, "dZ"));
		intensity = vec3(safeAtoF(elLight, "iR"), safeAtoF(elLight, "iG"), safeAtoF(elLight, "iB"));
		ret = Light::Type::DIRECTIONAL;
	}
	if (lType.compare("Point") == 0){
		pos = vec3(safeAtoF(elLight, "pX"), safeAtoF(elLight, "pY"), safeAtoF(elLight, "pZ"));
		intensity = vec3(safeAtoF(elLight, "iR"), safeAtoF(elLight, "iG"), safeAtoF(elLight, "iB"));
		ret = Light::Type::POINT;
	}
	if (lType.compare("Ambient") == 0){
		intensity = vec3(safeAtoF(elLight, "iR"), safeAtoF(elLight, "iG"), safeAtoF(elLight, "iB"));
		ret = Light::Type::AMBIENT;
	}

	l = Light(ret, pos, dir, intensity);
	return ret;
}

// Caller must bind shader (SBind could be an arg...) (does it have to be bound?)
static void createGPUAssets(IqmTypeMap iqmTypes, Geometry& geom, string fileName){
	IqmFile iqmFile(fileName);

	auto makeVBO = []
		(GLuint buf, GLint handle, void * ptr, GLsizeiptr numBytes, GLuint dim, GLuint type){
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, numBytes, ptr, GL_STATIC_DRAW);
		glEnableVertexAttribArray(handle);
		glVertexAttribPointer(handle, dim, type, 0, 0, 0);
		//Disable?
	};
	GLuint VAO(0), bIdx(0), nIndices(0);
	vector<GLuint> bufVBO(iqmTypes.size() + 1);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(bufVBO.size(), bufVBO.data());

	auto idx = iqmFile.Indices();
	nIndices = idx.count();

	for (auto it = iqmTypes.cbegin(); it != iqmTypes.cend(); ++it){
		switch (it->first){
		case IqmFile::IQM_T::POSITION:
		{
			auto pos = iqmFile.Positions();
			makeVBO(bufVBO[bIdx++], it->second, pos.ptr(), pos.numBytes(), pos.nativeSize() / sizeof(float), GL_FLOAT);
		}
		break;
		case IqmFile::IQM_T::NORMAL:
		{
			auto nrm = iqmFile.Normals();
			makeVBO(bufVBO[bIdx++], it->second, nrm.ptr(), nrm.numBytes(), nrm.nativeSize() / sizeof(float), GL_FLOAT);
		}
		break;
		case IqmFile::IQM_T::TEXCOORD:
		{
			auto tex = iqmFile.TexCoords();
			makeVBO(bufVBO[bIdx++], it->second, tex.ptr(), tex.numBytes(), tex.nativeSize() / sizeof(float), GL_FLOAT);
		}
		break;
		default:
			it = iqmTypes.erase(it);
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufVBO[bIdx]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.numBytes(), idx.ptr(), GL_STATIC_DRAW);

	geom.setNumIndices(nIndices);
	geom.setVAO(VAO);

	glBindVertexArray(0);
}

static void createGPUAssets(GLint handles[4], Light& l){
	glUniform1i(handles[0], (int)l.m_Type);
	glUniform3f(handles[1], l.m_Pos[0], l.m_Pos[1], l.m_Pos[2]);
	glUniform3f(handles[2], l.m_Dir[0], l.m_Dir[1], l.m_Dir[2]);
	glUniform3f(handles[3], l.m_Intensity[0], l.m_Intensity[1], l.m_Intensity[2]);
}
