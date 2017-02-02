#pragma once
#include <iostream>
#include <vector>
// GLM
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#define GLEW_STATIC
#include "../glew/glew.h"	// include GL Extension Wrangler
#include "../glfw/glfw3.h"	// include GLFW helper library
// Shaders
#include "Shader.h"
// Camera
#include "Camera.h"
#include "InputReader.h"