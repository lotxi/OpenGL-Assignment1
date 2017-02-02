#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
  
#include "../glew/glew.h"	// include GL Extension Wrangler

class Shader
{
public:
  	// The program ID
	GLuint Program;
	// Constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
  	// Use the program
  	void Use();
};
  
