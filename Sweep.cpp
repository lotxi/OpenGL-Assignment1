#include "Sweep.h"

//Dimensions
GLuint WIDTH = 800, HEIGHT = 800;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void Do_Movement();

// Instantiate camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));

// Controls 
bool keys[1024];
bool mouse[2];
GLfloat lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;
bool firstMouse = true;
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


int main()
{
	// Initialize OpenGL Window
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 1", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowSizeCallback(window, framebuffer_size_callback);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwGetWindowSize(window, &width, &height);


	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Create shader program
	Shader shader = Shader("shader.vs", "shader.frag");
	InputReader* input;
	try
	{
		 input = new InputReader("rotational_bowl.txt");
	}
	catch (const char* msg)
	{
		// Error reading input file
		std::cerr << msg << std::endl;
		return -1;
	}
	
	GLfloat* vertices = input->getVertices();
	GLuint* indices = input->getIndices();
	int indexSize = input->getIndicesSize();
	int size = input->getVerticesSize();

	// Output indices
	//for (int i=0; i<indexSize; i++)
	//{
	//	std::cout <<  i << " : " << indices[i] << std::endl;
	//}

			
		//std::cout << "\nEBO size: " << indexSize << " VBO size: " << size << std::endl;

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	
	//// Colour attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
	glBindVertexArray(0); // Unbind VAO
		

	glPointSize(10);

	// Rendering loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		Do_Movement();

		// Clear the colorbuffer
		glClearColor(0.0f, 0.07f, 0.58f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate shader
		shader.Use();

		// Camera transformation
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Projection
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Get Uniform locations
		GLint viewLoc = glGetUniformLocation(shader.Program, "view");
		GLint projectionLoc = glGetUniformLocation(shader.Program, "projection");
		GLint modelLoc = glGetUniformLocation(shader.Program, "model");

		// Pass matrices to shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Draw container
		glBindVertexArray(VAO);
		glm::mat4 model;

		// Process objectrotations as needed
//		model = glm::translate(model, glm::vec3(0.5f, -0.5f, 0.0f));
		model = glm::rotate(model, glm::radians(camera.Rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(camera.Rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(camera.Rotation.z), glm::vec3(0, 0, 1));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Draw the triangles
		glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);

		//Swap Buffers	
		glfwSwapInterval(1); // Limit frame rate to refresh rate
		glBindVertexArray(0); // Unbind VAO

		glfwSwapBuffers(window); // Swap buffers


	}


	// Cleanup
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	
	delete[] vertices;
	delete[] indices;
	delete input;

	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	
if (action == GLFW_PRESS){
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case GLFW_KEY_2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case GLFW_KEY_3:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case GLFW_KEY_F:
		camera.lock = !camera.lock;
		camera.lock ? glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL) :
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
		
	} 

}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}

}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (firstMouse) // this bool variable is initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}


	if (!camera.lock){
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		if (action==GLFW_PRESS)
		{
			mouse[0] = true;
		}
		else if (action== GLFW_RELEASE)
		{
			mouse[0] = false;
		}
		
	}

	if (button == GLFW_MOUSE_BUTTON_2)
	{
		if (action == GLFW_PRESS)
		{
			mouse[1] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			mouse[1] = false;
		}

	}
	
}


void Do_Movement()
{
	// Camera movement
	if (!camera.lock){
	if (keys[GLFW_KEY_W] || mouse[0])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S] || mouse[1])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	

	if (keys[GLFW_KEY_RIGHT])
		camera.Rotation.y += deltaTime *50.0f;
	if (keys[GLFW_KEY_LEFT])
		camera.Rotation.y -= deltaTime *50.0f;
	if (keys[GLFW_KEY_UP])
		camera.Rotation.x += deltaTime *50.0f;
	if (keys[GLFW_KEY_DOWN])
		camera.Rotation.x -= deltaTime *50.0f;
}

