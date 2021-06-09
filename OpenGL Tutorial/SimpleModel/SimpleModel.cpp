#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include<iostream>
#include "Filepath.h"
#include "stb_image.h"
//#include "Shader.h"
#include "Model.h"
#include "Camera.h"


// Key B: switch between Phong lighting model and Blin-Phong lightning model
// Key N: show normal vector
// Key S: show specular light

// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
float quadVertices[] = { 
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
};

double xlast = 400;
double ylast = 300;

int window_w = 800;
bool MouseControl = false;
bool ShowNormal = false;
bool Blin_Phone = true;
bool Showspec = true;


void WindowResize_Callback(GLFWwindow* window, int width, int height)
{
	Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
	camera->SetPerspective(camera->fov,width,height,camera->near,camera->far);
	glViewport(0, 0, width, height);

	window_w = width;

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

void key_callback(GLFWwindow* window, int button, int scancode, int action, int mods)
{
	if(button == GLFW_KEY_N && action == GLFW_PRESS)
		if (ShowNormal) ShowNormal = false;
		else ShowNormal = true;

	if (button == GLFW_KEY_B && action == GLFW_PRESS)
		if (Blin_Phone) Blin_Phone = false;
		else Blin_Phone = true;

	if (button == GLFW_KEY_S && action == GLFW_PRESS)
		if (Showspec) Showspec = false;
		else Showspec = true;

}

int main(int argc, char** argv)
{
	//------------------------------------------------------------------------------------
	glfwInit();
	//configure glfw context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
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


	// register framebuffer resize callback
	glfwSetFramebufferSizeCallback(window,WindowResize_Callback);

	// register mouse button call back
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	

	//------------------------------------------------------------------------------------
	//loading model
	Model model(ModelPath);
	
	//setting camera obj
	TrackBall CameraControl(glm::vec3(0, 1, 5));
	CameraControl.SetRotationCenter(glm::vec3(0,1,0));
	Camera* camera = &CameraControl;
	//set pointer to camera in the window system 
	glfwSetWindowUserPointer(window,(void*)camera);
	//glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
	glfwSetCursorPosCallback(window,cursor_position_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);

	camera->SetPerspective(45.0f,800.0f,600.f,0.1f,100.f);



	// setting up lights
	LightSource pointLight(glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(1.0f));
	pointLight.SetPointlight(glm::vec3(3.0f, 3.0f, 3.0f), 1.0f, 0.09f, 0.032f);

	LightSource DirLight(glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f));
	DirLight.SetDirlight(glm::vec3(0, -1.0f, 0.0f));
	
	//------------------------------------------------------------------------------------
	Shader shader(vertPath, fragPath);

	shader.Use();
	/*shader.SetUniformInt("PointlightNum", 1);
	shader.SetUniformVec3("pointlights[0].lightPos",glm::value_ptr(pointLight.lightPos));
	shader.SetUniformVec3("pointlights[0].ambient",glm::value_ptr(pointLight.ambient));
	shader.SetUniformVec3("pointlights[0].diffuse",glm::value_ptr(pointLight.diffuse));
	shader.SetUniformVec3("pointlights[0].specular",glm::value_ptr(pointLight.specular));
	shader.SetUniformf("pointlights[0].constant", pointLight.constant);
	shader.SetUniformf("pointlights[0].linear", pointLight.linear);
	shader.SetUniformf("pointlights[0].quadratic", pointLight.quadratic);*/

	shader.SetUniformInt("DirlightNum", 1);
	shader.SetUniformVec3("Dirlights[0].lightDir",glm::value_ptr(DirLight.lightDir));
	shader.SetUniformVec3("Dirlights[0].ambient", glm::value_ptr(DirLight.ambient));
	shader.SetUniformVec3("Dirlights[0].diffuse", glm::value_ptr(DirLight.diffuse));
	shader.SetUniformVec3("Dirlights[0].specular", glm::value_ptr(DirLight.specular));

	//------------------------------------------------------------------------------------
	Shader Nview_shader(Nview_vertpath,Nview_fragpath);
	Nview_shader.SetUpGeometryShader(Nview_geompath);

	Shader ScreenQuad(Screen_vertpath,Screen_fragpath);
	ScreenQuad.Use();
	ScreenQuad.SetUniformInt("QuadTexture",0);

	//------------------------------------------------------------------------------------
	// configure framebuffer obj
	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER,hdrFBO);

	// color buffer
	unsigned int colorbuffer;
	glGenTextures(1,&colorbuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorbuffer);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB16F,800,600,0,GL_RGB,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,colorbuffer,0);
	glBindTexture(GL_TEXTURE_2D, 0);
	 
	//depth and stencil buffer
	unsigned int DepthRBO;
	glGenRenderbuffers(1, &DepthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER,DepthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,800,600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,DepthRBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	unsigned int quadVAO;
	unsigned int quadVBO;
	glGenVertexArrays(1,&quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quadVertices),quadVertices,GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);



	// rendering loop
	while (!glfwWindowShouldClose(window))
	{
		
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_MULTISAMPLE);
		glEnable(GL_FRAMEBUFFER_SRGB);

		// rendering
		glClearColor(0.6f, 0.6f, 0.6f, 0.0f);

		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//glViewport(0,0,800,600);

		shader.Use();
		shader.SetUniformInt("Blin",Blin_Phone);
		shader.SetUniformInt("w",window_w);
		shader.SetUniformInt("ifspec",Showspec);
		shader.SetUniformVec3("wCameraPos",glm::value_ptr(camera->CameraPos));
		shader.SetUniformMat4("model",GL_FALSE,glm::value_ptr(camera->GetModelMatrix()));
		shader.SetUniformMat4("view",GL_FALSE,glm::value_ptr(camera->GetViewMatrix()));
		shader.SetUniformMat4("projection", GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
		
		model.Draw(shader);
		//------------------------------------------------------------------------------------
		if (ShowNormal)
		{
			Nview_shader.Use();
			Nview_shader.SetUniformMat4("model", GL_FALSE, glm::value_ptr(camera->GetModelMatrix()));
			Nview_shader.SetUniformMat4("view", GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
			Nview_shader.SetUniformMat4("projection", GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

			model.Draw(Nview_shader);
		}

		/*
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		int f_w, f_h;
		glfwGetFramebufferSize(window,&f_w,&f_h);
		glViewport(0, 0, f_w, f_h);
		

		ScreenQuad.Use();
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorbuffer);
		glDrawArrays(GL_TRIANGLES,0,6);
		*/
	
		glfwSwapBuffers(window);
		ProcessKeyboardInput(window);
		glfwPollEvents();
	}


	
	glfwTerminate();
	return 0;
}