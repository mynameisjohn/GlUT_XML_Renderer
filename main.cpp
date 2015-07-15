#include "Scene.h"
#include <MouseManager.h>
#include "KeyboardManager.h"

#include <gtx/quaternion.hpp>
#include <gtx/transform.hpp>

#include <iostream>
using namespace std;

const unsigned int WIDTH = 600;
const unsigned int HEIGHT = 600;
const unsigned int FPS = 30;

Scene g_Scene;
Camera g_Camera;
Shader g_Shader;

// TODO
// I moved some stuff back in here to avoid a monolith,
// but things will have to be restructured

void redraw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto sBind = g_Shader.S_Bind();
    auto proj = g_Camera.getMat();
    glUniformMatrix4fv(g_Camera.getProjHandle(), 1, GL_FALSE, (const GLfloat *)&proj);
    glUniform3f(g_Shader["u_Color"], 1,0,1); //I should be able to ditch these with textures
	g_Scene.Draw();
	glutSwapBuffers();}

void MouseBtnFunc(int button, int state, int x, int y){
    MouseManager::HandleMouseBtn(button, state, x, y);
}

void MouseMotionFunc(int x, int y){
    g_Camera.rotate(MouseManager::HandleMouseMove_B(x, y));
}

void MousePassiveFunc(int x, int y){
    g_Camera.rotate(MouseManager::HandleMouseMove_P(x, y));
}

void KeyboardFunc(unsigned char k, int x, int y){
    KeyboardManager::HandleKey(k, x, y); // Kind of useless right now
    const float T(5.f);
    switch (k){
            case 'w':
            g_Camera.translate(T*glm::vec3(0,0,1));
            break;
            case 's':
            g_Camera.translate(T*glm::vec3(0,0,-1));
            break;
            case 'd':
            g_Camera.translate(T*glm::vec3(-1,0,0));
            break;
            case 'a':
            g_Camera.translate(T*glm::vec3(-1,0,0));
            break;
        default:
            break;
    }
}

// Val can be the case of several registered callbacks... but I don't care
void onTimer(int val){
    switch(val){
        case 0: // right now all I care about is redrawing at FPS
        default:
            redraw();
            glutTimerFunc(FPS/1000.f,onTimer,0);
            break;
    }
}

void initGL(int argc, char ** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
#ifndef __APPLE__
	glutInitContextVersion(3, 0);
#endif
	glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(250, 1000);
	glutCreateWindow("GLUT XML Renderer");
	
	// Callbacks
	glutDisplayFunc(redraw);
    glutMouseFunc(MouseBtnFunc);
	glutMotionFunc(MouseMotionFunc);
	glutPassiveMotionFunc(MousePassiveFunc);
    glutKeyboardFunc(KeyboardFunc);
    glutTimerFunc(FPS/1000.f, onTimer, 0);

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