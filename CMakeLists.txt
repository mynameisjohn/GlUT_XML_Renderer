cmake_minimum_required(VERSION 2.8)

project(GLUT_XML_Renderer)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

add_definitions(-DFREEGLUT_STATIC)
add_definitions(-DGLEW_STATIC)
add_definitions(-DGLM_FORCE_RADIANS)

find_package(OpenGL)
find_package(GLEW)
find_package(GLUT)

if(!GLUT_FOUND)
	set(GLUT_LIBRARIES $ENV{SDK}/freeglut3/lib/Debug/freeglut_staticd.lib)
	set(GLUT_INCLUDE_DIR $ENV{SDK}/freeglut3/include)
endif(!GLUT_FOUND)

if(!GLEW_FOUND)
	set(GLUT_LIBRARIES $ENV{SDK}/glew/lib/Win32/glew32s.lib )
	set(GLUT_INCLUDE_DIR $ENV{SDK}/glew/include )
endif(!GLEW_FOUND)

if (WIN32)
	set(GLM_DIR $ENV{SDK}/glm)
else (WIN32)
	set(GLM_DIR /usr/include/glm)
endif (WIN32)

set(SOURCES tinyxml2.cpp Shader.cpp Scene.cpp main.cpp Geometry.cpp Camera.cpp IqmFile.cpp Light.cpp
							Shader.h Scene.h Geometry.h Camera.h IqmFile.h Light.h
							build/vert.glsl build/frag.glsl build/TestScene.xml)
set(GL_LIBS ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLUT_LIBRARIES})
set(GL_INC ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GL_INC} ${GLM_DIR})

add_executable(GlutXMLRenderer ${SOURCES})
target_link_libraries(GlutXMLRenderer LINK_PUBLIC ${GL_LIBS})
