#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "Filepath.h"
#include "Model.h"
#include "Camera.h"

void WindowResize_Callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}




int main(int argc, char** argv)
{
	//------------------------------------------------------------------------------------
	glfwInit();
	//configure glfw context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GL_SAMPLES, 4);

	// create window
	GLFWwindow* window = glfwCreateWindow(800, 600, "SimpleModel", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	//initialization glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, WindowResize_Callback);

	//------------------------------------------------------------------------------------
	// setting up camera
	Camera* camera = new ViewCamera(glm::vec3(0.0f, 0.0f, 3.0f));
	camera->SetPerspective(45.0f, 800.0f, 600.0f, 0.1f, 100.0f);


	//------------------------------------------------------------------------------------
	//loading model
	Model sphere(sphere_path);

}