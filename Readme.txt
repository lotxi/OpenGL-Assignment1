COMP 371 Assignment 1
Tim Gottschalk - 25595282

Camera and Shader classes are based on those from the learnopengl.com

Requirements:
OpenGL 3.3+
GLFW3
GLM
GLEW (static)

!!!!!!!!IMPORTANT NOTE!!!!!!!!

Because the the static version of GLEW is used, Please ensure glew32s.lib is present in the project properties under Linker->Input->Additional dependencies. The s is important

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


Instructions: 

To load the different files, modify the filename on line 72 of Sweep.cpp.

In order to allow for easier window resizing, press F to lock the camera and display the mouse cursor.

CONTROLS:
F - Toggle Free Camera / Lock on object
1 - Render mode (Fill)
2 - Render Mode (Wireframe)
3 - Render Mode (Points)

Up/Down Arrows - Rotate Object Vertically
Left/Right Arrows - Rotate Object Horizontally

If Free Camera is enabled:

W or Mouse 1 - Move forward 
S or Mouse 2- Move backward
A- Turn left
D- Turn right
Mouse Movement - Point Camera
