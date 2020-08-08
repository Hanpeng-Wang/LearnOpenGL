#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include<iostream>
#include "Filepath.h"
#include "stb_image.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"


double xlast = 400;
double ylast = 300;

bool MouseControl = false;


void WindowResize_Callback(GLFWwindow* window, int width, int height)
{
	Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
	camera->SetPerspective(camera->fov,width,height,camera->near,camera->far);
	glViewport(0, 0, width, height);

}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (MouseControl)
	{
		Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
		camera->Screen2DRotation(xlast, ylast, xpos, ypos);
	}
		xlast = xpos;
		ylast = ypos;
	
}

void ProcessKeyboardInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
	camera->ScreenZoom(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		MouseControl = true;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		MouseControl = false;
}

int main(int argc, char** argv)
{
	//------------------------------------------------------------------------------------
	glfwInit();
	//configure glfw context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// create window
	GLFWwindow* window = glfwCreateWindow(800, 600, "SimpleTriangle", NULL, NULL);
	
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


	// register framebuffer resize callback
	glfwSetFramebufferSizeCallback(window,WindowResize_Callback);

	// register mouse button call back
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	
	//------------------------------------------------------------------------------------
	//loading model
	Model backpack(ModelPath);
	
	//setting camera obj
	//Camera* camera = new FlyCamera(glm::vec3(0, 0, 10));
	Camera* camera = new TrackBall(glm::vec3(0, 0, 10));
	//set pointer to camera in the window system 
	glfwSetWindowUserPointer(window,(void*)camera);
	//glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
	glfwSetCursorPosCallback(window,cursor_position_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);

	camera->SetPerspective(45.0f,800.0f,600.f,0.1f,100.f);

	//------------------------------------------------------------------------------------
	Shader shader(vertPath, fragPath);
	// Set projecttion matrix once outside of main loop 
	shader.Use();
	



	// rendering loop
	while (!glfwWindowShouldClose(window))
	{
		
		glEnable(GL_DEPTH_TEST);

		// rendering
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		shader.Use();

		glm::mat4 model = glm::mat4(1.0f);
		shader.SetUniformMat4("model",GL_FALSE,glm::value_ptr(model));
		shader.SetUniformMat4("view",GL_FALSE,glm::value_ptr(camera->GetViewMatrix()));
		shader.SetUniformMat4("projection", GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
		
		backpack.Draw(shader);
	
		glfwSwapBuffers(window);
		ProcessKeyboardInput(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}