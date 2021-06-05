#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include<glm/gtx/transform.hpp>
#include <iostream>
#include "Filepath.h"
#include "Model.h"
#include "Camera.h"
#include"stb_image.h"



double xlast = 400;
double ylast = 300;

bool MouseControl = false;
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

void WindowResize_Callback(GLFWwindow* window, int width, int height)
{
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

unsigned int LoadGenCubeMap(std::vector<std::string> CubeMapFile);
unsigned int CreateSkybox();

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
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);

	//------------------------------------------------------------------------------------
	// setting up camera
	Camera* camera = new ViewCamera(glm::vec3(0.0f, 0.0f, 3.0f));
	camera->SetPerspective(45.0f, 800.0f, 600.0f, 0.1f, 100.0f);

	glfwSetWindowUserPointer(window, (void*)camera);

	//------------------------------------------------------------------------------------
	//loading model
	Model sphere(sphere_path);
	Shader sphere_shader(sphere_vertPath, sphere_fragPath);

	//------------------------------------------------------------------------------------
	//loading skybox
	unsigned int skyboxVAO = CreateSkybox();
	Shader skybox_shader(skybox_vertPath, skybox_fragPath);

	skybox_shader.Use();
	skybox_shader.SetUniformInt("skybox", 0); // bind shader uniform sampler(skybox) to texture unit 0

	glActiveTexture(GL_TEXTURE0); // loading cubemap texture to texture unit 0
	unsigned int skyboxTex = LoadGenCubeMap(cubeMapPath);
	glActiveTexture(0);

	// create uniform buffer object to store view and projection matrix
	unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 128, (void*)0, GL_STATIC_DRAW); // 128  = 2 * sizeof(glm::mat4)
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices); // bind uniform buffer object to binding point 0;

	// bind uniform blocks in shaders to binding point 
	// Matrices to binding point 0
	glUniformBlockBinding(sphere_shader.ProgramID, glGetUniformBlockIndex(sphere_shader.ProgramID, "Matrices"), 0);
	glUniformBlockBinding(skybox_shader.ProgramID, glGetUniformBlockIndex(skybox_shader.ProgramID, "Matrices"), 0);



	// rendering loop
	while (!glfwWindowShouldClose(window))
	{
		glEnable(GL_DEPTH_TEST);

		// rendering
		glClearColor(0.6f, 0.6f, 0.6f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// update uniform buffer objects
		// uniform block Matrices
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->GetViewMatrix()));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->GetProjectionMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);


		sphere_shader.Use();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		sphere_shader.SetUniformMat4("model", GL_FALSE, glm::value_ptr(ModelMatrix));


		sphere.Draw(sphere_shader);


		skybox_shader.Use();

		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP,skyboxTex);
		glBindVertexArray(skyboxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glDepthFunc(GL_LESS);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

unsigned int LoadGenCubeMap(std::vector<std::string> CubeMapFile)
{
	unsigned int CubeTexture;
	glGenTextures(1, &CubeTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeTexture);

	int width, height, nChannels;
	unsigned char* data;

	//stbi_set_flip_vertically_on_load(true);
	for (int i = 0; i < CubeMapFile.size(); i++)
	{
		data = stbi_load(CubeMapFile[i].c_str(), &width, &height, &nChannels, 0);
		if (data == NULL)
		{
			std::cout << "ERROR::TEXTURE::FAILED TO LOAD TEXTURE: " << CubeMapFile[i] << std::endl;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return CubeTexture;

}
unsigned int CreateSkybox()
{
	unsigned int skyboxVA;
	glGenVertexArrays(1, &skyboxVA);
	glBindVertexArray(skyboxVA);

	unsigned int skyboxVB;
	glGenBuffers(1, &skyboxVB);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	return skyboxVA;
}