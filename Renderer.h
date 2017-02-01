#pragma once
#include <iostream>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW 
#include <GLFW/glfw3.h>
// Shaders
#include "Shader.h"
// Camera
#include "Camera.h"
#include "InputReader.h"
#include "Sweep.h"

class Renderer{
public:
	int Render(Sweep sweep);
	static Renderer* Renderer::instance();
	
private: 
	Renderer::Renderer();
	const GLuint WIDTH, HEIGHT;
	// Function prototypes
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void Do_Movement();
	Camera camera;
	bool keys[1024];
	GLfloat lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;
	bool firstMouse = true;
};