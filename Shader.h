#pragma once

// You should make copying / moving safe

//#define FREEGLUT_STATIC
//#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <map>
using HandleMap = std::map < std::string, GLint >;

class Shader
{
public:
	class ScopedBind {
		friend class Shader;
	protected:
		Shader& m_Shader;
		ScopedBind(Shader& s) : m_Shader(s) { m_Shader.Bind(); }
	public:
		~ScopedBind(){ m_Shader.Unbind(); }
	};
	struct VBOFactory{
		HandleMap m_attrs;
	};
	// Private initializer
	int CompileAndLink();
public:
	Shader();
	Shader(std::string vs, std::string fs);
	~Shader();
	bool Bind();
	bool Unbind();
	bool IsBound() const;
	bool PrintError() const;
	int PrintLog_V() const;
	int PrintLog_F() const;
	int PrintSrc_V() const;
	int PrintSrc_F() const;
	GLint getHandle(const std::string idx);
	GLint operator [] (const std::string idx);
	inline ScopedBind S_Bind(){ return ScopedBind(*this); }
	inline HandleMap getHandleMap() { return m_Handles; }
	VBOFactory getAttrFactory();
private:
	bool m_bIsBound;
	GLuint m_Program, m_hVertShader, m_hFragShader;
	std::string m_VertShaderSrc, m_FragShaderSrc;
	HandleMap m_Handles;
};