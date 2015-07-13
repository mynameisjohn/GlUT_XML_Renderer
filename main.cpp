#include "Scene.h"
#include <iostream>
using namespace std;

Scene g_Scene;
Camera g_Camera;
Shader g_Shader;

#include <MouseManager.h>
#include "KeyboardManager.h"

// TODO
// I moved some stuff back in here to avoid a monolith,
// but things will have to be restructured

void redraw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto sBind = g_Shader.S_Bind();
    glUniformMatrix4fv(g_Camera.getProjHandle(), 1, GL_FALSE, (const GLfloat *)g_Camera.getProjPtr());
    glUniform3f(g_Shader["u_Color"], 1,0,1); //I should be able to ditch these with textures
	g_Scene.Draw();
	glutSwapBuffers();
}

void initGL(int argc, char ** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
#ifndef __APPLE__
	glutInitContextVersion(3, 0);
#endif
	glutInitWindowSize(600, 600);
    glutInitWindowPosition(500, 500);
	glutCreateWindow("GLUT XML Renderer");
	
	// Callbacks
	glutDisplayFunc(redraw);
	glutMouseFunc(MouseManager::HandleMouseBtn);
	glutMotionFunc(MouseManager::HandleMouseMove_B);
	glutPassiveMotionFunc(MouseManager::HandleMouseMove_P);
	glutKeyboardFunc(KeyboardManager::HandleKey);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK){
		cout << "Problem initializing GLEW:\n " << glewGetErrorString(err) << endl;
	}

	// My z is inverted...
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	return;
}

int main(int argc, char ** argv){
	initGL(argc, argv);
    g_Scene = Scene("TestScene.xml", g_Shader, g_Camera);
	glutMainLoop();

	return EXIT_SUCCESS;
}