#include "Renderer.h"



// Function prototypes






int Renderer::Render(Sweep sweep)
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 1", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, Renderer::key_callback);
	glfwSetCursorPosCallback(window, Renderer::mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	//	int width, height;

	//	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, WIDTH, HEIGHT);

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	Shader testShader = Shader("test.vs", "test.frag");

	InputReader test = InputReader("translational_hollow_box.txt");

	std::vector<glm::vec3> *pCurve = test.getProfileCurves();
	int pSize = pCurve->size();

	if (test.getTranslational())
	{
		std::vector<glm::vec3> *tCurve = test.getTrajectoryCurves();
		int tSize = tCurve->size();

		std::cout << "Psize: " << pSize << std::endl;
		std::cout << "Tsize: " << tSize << std::endl;

		//std::vector<glm::vec3> vContainer;
		GLuint size = pSize * tSize * 6;
		GLuint indexSize = (pSize - 1)*(tSize - 1) * 6;
		int *indices = new int[indexSize];
		int indexIndices = 0;
		GLfloat *vertices = new GLfloat[size];

		int k = 0; // K is used to track indices in the vertices array

		for (int i = 0; i<pSize; i++)
		{
			// Copy the initial profile curve into the vertices array
			vertices[k++] = (*pCurve)[i].x;
			vertices[k++] = (*pCurve)[i].y;
			vertices[k++] = (*pCurve)[i].z;
			vertices[k++] = 0;
			vertices[k++] = 0;
			vertices[k++] = (*pCurve)[i].z;
		}

		for (int i = 0; i<tSize - 1; i++)
		{
			for (int j = 0; j < pSize; j++)
			{
				// Translate each vertex from the previous iteration of P by the matching value in vector (t_(i+1)-t_(i))
				glm::vec4 temp = glm::vec4(vertices[k - 6 * pSize], vertices[k - 6 * pSize + 1], vertices[k - 6 * pSize + 2], 1.0f);
				glm::mat4 translation = glm::translate(glm::mat4(), (*tCurve)[i + 1] - (*tCurve)[i]);
				temp = translation * temp;

				// Copy the translated vertices to vertices array
				vertices[k++] = temp.x;
				vertices[k++] = temp.y;
				vertices[k++] = temp.z;
				vertices[k++] = 0;
				vertices[k++] = 0;
				vertices[k++] = temp.z;

				// Calculate the indices of the vertices that will form triangles
				if (j != pSize - 1)
				{
					// First triangle
					indices[indexIndices] = k / 6 - 1;
					indices[indexIndices + 1] = k / 6;
					indices[indexIndices + 2] = k / 6 - pSize;
					// Second triangle
					indices[indexIndices + 3] = k / 6 - 1;
					indices[indexIndices + 4] = k / 6 - pSize;
					indices[indexIndices + 5] = k / 6 - pSize - 1;

					indexIndices += 6;
				}
			}
		}

		int count = 0;
		for (int i = 0; i< size; i++)
		{
			int k = 0;

			if (i % 6 == 0 && i != 0)
			{
				count++;
			}
			std::cout << i << " : " << count << " : ";
			int n = (i % 6);
			switch (n)
			{
			case 0:
				std::cout << "X: ";
				break;
			case 1:
				std::cout << "Y: ";
				break;
			case 2:
				std::cout << "Z: ";
				break;
			case 3:
				std::cout << "R: ";
				break;
			case 4:
				std::cout << "G: ";
				break;
			case 5:
				std::cout << "B: ";
				break;
			default:
				break;
			}
			std::cout << " : " << vertices[i] << std::endl;
		}


		for (int i = 0; i<indexSize; i++)
		{
			std::cout << "Indices[" << i << "]=" << indices[i] << std::endl;
		}


		std::cout << "\nEBO size: " << indexSize << " VBO size: " << size << std::endl;

	}













	/*int count = 0;
	for (int i = 0; i< size; i++)
	{
	int k = 0;

	if (i % 6 == 0 && i != 0)
	{
	count++;
	}
	std::cout << i << " : " << count << " : ";
	int n = (i % 6);
	switch (n)
	{
	case 0:
	std::cout << "X: ";
	break;
	case 1:
	std::cout << "Y: ";
	break;
	case 2:
	std::cout << "Z: ";
	break;
	case 3:
	std::cout << "R: ";
	break;
	case 4:
	std::cout << "G: ";
	break;
	case 5:
	std::cout << "B: ";
	break;
	default:
	break;
	}
	std::cout << " : " << vertices[i] << std::endl;
	}
	*/

	//	std::cout << "\nEBO size: " << indexSize << " VBO size: " << size << std::endl;



	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*size, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices)*indexSize, indices, GL_STATIC_DRAW);


	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO

	glm::vec3 triangle_scale = glm::vec3(1.0f); //shorthand, initializes all 4 components to 1.0f

												//GLuint modelLoc = glGetUniformLocation(testShader.Program, "model");
												//GLuint viewLoc = glGetUniformLocation(testShader.Program, "view");
												//GLuint projectionLoc = glGetUniformLocation(testShader.Program, "projection");

												//GLuint texture1;
												//GLuint texture2;
												//// ====================
												//// Textures 
												//// ====================
												////Texture 1
												//glGenTextures(1, &texture1);
												//glBindTexture(GL_TEXTURE_2D, texture1); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
												//										// Set our texture parameters
												//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
												//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
												//// Set texture filtering
												//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
												//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
												//// Load, create texture and generate mipmaps
												//int width, height;
												//unsigned char* image = SOIL_load_image("container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
												//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
												//glGenerateMipmap(GL_TEXTURE_2D);
												//SOIL_free_image_data(image);
												//glBindTexture(GL_TEXTURE_2D, 0);

												//// Texture 2
												//glGenTextures(1, &texture2);
												//glBindTexture(GL_TEXTURE_2D, texture2);
												//// Set our texture parameters
												//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
												//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
												//// Set texture filtering
												//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
												//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
												//// Load, create texture and generate mipmaps
												//image = SOIL_load_image("dickbutt.png", &width, &height, 0, SOIL_LOAD_RGB);
												//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
												//glGenerateMipmap(GL_TEXTURE_2D);
												//SOIL_free_image_data(image);
												//glBindTexture(GL_TEXTURE_2D, 0);

	GLfloat rotation = 9.0f;
	glPointSize(10);

	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		Do_Movement();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate shader
		testShader.Use();


		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, triangle_scale);


		// Camera view
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Projection
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.0f, 100.0f);

		// Get Uniform locations
		GLint viewLoc = glGetUniformLocation(testShader.Program, "view");
		GLint projectionLoc = glGetUniformLocation(testShader.Program, "projection");
		GLint modelLoc = glGetUniformLocation(testShader.Program, "model");

		// Pass matrices to shader
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//// Bind Textures using texture units
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture1);
		//glUniform1i(glGetUniformLocation(testShader.Program, "ourTexture1"), 0);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, texture2);
		//glUniform1i(glGetUniformLocation(testShader.Program, "ourTexture2"), 1);


		// Draw container
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_POINTS, 0, size);
		glBindVertexArray(0);

		//Swap Buffers	
		glfwSwapInterval(1);
		glfwSwapBuffers(window);
	}



	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();

	return 0;
}

void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;


	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}


}

void Renderer::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (firstMouse) // this bool variable is initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


void Renderer::Do_Movement()
{
	// Camera movement
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

