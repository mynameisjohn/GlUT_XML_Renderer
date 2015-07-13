#include "Scene.h"
#include <iostream>
using namespace std;
// Implement those constructors...
#include <memory>
using ScenePtr = unique_ptr < Scene > ;
ScenePtr S;

#include <MouseManager.h>
#include "KeyboardManager.h"

void redraw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	S->Draw();
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

	S = ScenePtr(new Scene("TestScene.xml"));
	
	glutMainLoop();

	return EXIT_SUCCESS;
}