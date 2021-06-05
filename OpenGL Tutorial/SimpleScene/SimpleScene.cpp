#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/gtx/transform.hpp>
#include<iostream>
#include"Camera.h"
#include"Filepath.h"
#include"stb_image.h"
#include"Model.h"


// Key W: switch between windowed mode and full screen mode

bool MouseControl = false;
double xlast = 400;
double ylast = 300;
bool Fullwindow = false;

int windowpos_x;
int windowpos_y;

int windowsize_x;
int windowsize_y;

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

unsigned int LoadGenCubeMap(std::vector<std::string> CubeMapFile);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0,width,height);
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
    camera->SetPerspective(camera->fov,width,height,camera->near, camera->far);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        MouseControl = true;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        MouseControl = false;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (MouseControl)
    {
        Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
        camera->Screen2DRotation(xlast,ylast,xpos,ypos);
    }
    xlast = xpos;
    ylast = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
    camera->ScreenZoom(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode,int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        if (!Fullwindow)
        {
            glfwGetWindowPos(window,&windowpos_x,&windowpos_y);
            glfwGetWindowSize(window,&windowsize_x,&windowsize_y);
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, 60);
            Fullwindow = true;
        }
        else
        {
            glfwSetWindowMonitor(window, NULL, windowpos_x, windowpos_y, windowsize_x, windowsize_y, 60);
            Fullwindow = false;
        }
    }
}

int main(int argc, char** argv)
{
	//------------------------------------------------------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_SAMPLES,4);

	GLFWwindow* window = glfwCreateWindow(800,600,"SimpleScene",NULL,NULL);
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

    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window,cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
	//------------------------------------------------------------------------------------
    // setting up camera
    Camera* camera = new ViewCamera(glm::vec3(0.0f, 0.0f, 5.0f));
    camera->SetPerspective(45.0f,800.0f,600.0f,0.1f,100.0f);

    glfwSetWindowUserPointer(window,(void*)camera);

    //------------------------------------------------------------------------------------
    // load skybox textures
    glActiveTexture(GL_TEXTURE0);
    unsigned int skyboxMap = LoadGenCubeMap(cubeMapPath);

    // create VAO obj
    unsigned int VAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glBufferData(GL_ARRAY_BUFFER,sizeof(skyboxVertices),skyboxVertices,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    

    Shader skybox_shader(skybox_vertPath,skybox_fragPath);
    skybox_shader.Use();  
    skybox_shader.SetUniformInt("skybox",0);
    glUniformBlockBinding(skybox_shader.ProgramID,glGetUniformBlockIndex(skybox_shader.ProgramID,"Matrices"),0);

	//------------------------------------------------------------------------------------
    Model teapot(utah_teapot);
    Shader teapot_shader(model_vertPath, model_fragPath);
    teapot_shader.Use();
    teapot_shader.SetUniformInt("skybox", 0);
    glUniformBlockBinding(teapot_shader.ProgramID,glGetUniformBlockIndex(teapot_shader.ProgramID,"Matrices"),0);
    //------------------------------------------------------------------------------------
    Model sphere(sphere_path);
    Shader sphere_shader(sphere_vertshader,sphere_fragshader);
    sphere_shader.Use();
    sphere_shader.SetUniformInt("skybox",0);
    glUniformBlockBinding(sphere_shader.ProgramID, glGetUniformBlockIndex(sphere_shader.ProgramID, "Matrices"), 0);


    glm::mat4* sphereOffset;
    int spherenum = 10;
    glm::vec3 scale = glm::vec3(0.1f, 0.1f, 0.1f);
    sphereOffset = new glm::mat4[spherenum];

    float lasttime = 0.0;
    float angles_add = 0.0f;

    glm::mat4 ScaleMatrix = glm::scale(scale);
    glm::mat4 SphereRotation = glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    sphere_shader.SetUniformMat4("Scale", GL_FALSE, glm::value_ptr(ScaleMatrix));
    sphere_shader.SetUniformMat4("Rotation", GL_FALSE, glm::value_ptr(SphereRotation));

    unsigned int InstanceBuffer;
    glGenBuffers(1,&InstanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, InstanceBuffer);
    glBufferData(GL_ARRAY_BUFFER,spherenum*sizeof(glm::mat4),NULL,GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // set up uniform buffer for block "Matrices" across shaders
    unsigned int Matrices_NBO;
    glGenBuffers(1, &Matrices_NBO);
    glBindBuffer(GL_UNIFORM_BUFFER, Matrices_NBO);
    glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW); //size calculated in shaders
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, Matrices_NBO, 0, 128);



    for (int i = 0; i < sphere.meshes.size(); i++)
    {
        unsigned VAO = sphere.meshes[i].GetVAO();
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, InstanceBuffer);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }



    //------------------------------------------------------------------------------------
	while (!glfwWindowShouldClose(window))
	{
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_FRAMEBUFFER_SRGB);
        glClearColor(0.0f,0.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        float currtime = glfwGetTime();
        angles_add += 8.5f * (currtime - lasttime);

        float radius = 1.4;

        for (int i = 0; i < spherenum; i++)
        {
            float angles = i * 360.0f / spherenum + angles_add;
            glm::vec3 displacement = radius * glm::vec3(cos(glm::radians(angles)), 0, sin(glm::radians(angles)));
            sphereOffset[i] = glm::translate(displacement);
        }

        lasttime = currtime;

        glBindBuffer(GL_ARRAY_BUFFER, InstanceBuffer);
        void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(ptr,&sphereOffset[0][0], spherenum * sizeof(glm::mat4));
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindBuffer(GL_UNIFORM_BUFFER, Matrices_NBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(camera->GetViewMatrix()));
        glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(camera->GetProjectionMatrix()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        //------------------------------------------------------------------------------------
        
        /*
        glDepthFunc(GL_LEQUAL);
        skybox_shader.Use();
        //skybox_shader.SetUniformMat4("view",GL_FALSE,glm::value_ptr(camera->GetViewMatrix()));
        //skybox_shader.SetUniformMat4("projection",GL_FALSE,glm::value_ptr(camera->GetProjectionMatrix()));

        glDepthMask(GL_FALSE);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);
        //glBindBuffer(GL_UNIFORM_BUFFER,Matrices_NBO);
        glDrawArrays(GL_TRIANGLES,0,36);
        glDepthMask(GL_TRUE);
        */
        //------------------------------------------------------------------------------------
        teapot_shader.Use();
        glm::mat4 teapot_model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5f, 0.0f));
        teapot_shader.SetUniformVec3("CameraPos",glm::value_ptr(camera->CameraPos));
        teapot_shader.SetUniformMat4("model", GL_FALSE, glm::value_ptr(teapot_model));
        //teapot_shader.SetUniformMat4("view", GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
        //teapot_shader.SetUniformMat4("projection", GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

        teapot.Draw(teapot_shader);

        //------------------------------------------------------------------------------------
        sphere_shader.Use();
        sphere_shader.SetUniformVec3("CameraPos", glm::value_ptr(camera->CameraPos));
        //sphere_shader.SetUniformMat4("view", GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
        //sphere_shader.SetUniformMat4("projection", GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

        for (int i = 0; i < sphere.meshes.size(); i++)
        {
            unsigned VAO = sphere.meshes[i].GetVAO();
            glBindVertexArray(VAO);         
            glDrawElementsInstanced(GL_TRIANGLES,sphere.meshes[i].indices.size(),GL_UNSIGNED_INT,0,spherenum);
            glBindVertexArray(0);
        }

        glDepthFunc(GL_LEQUAL);
        skybox_shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP,skyboxMap);
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glEnable(GL_FRAMEBUFFER_SRGB);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        

        //------------------------------------------------------------------------------------
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

    int width, height,nChannels;
    unsigned char* data;

    //stbi_set_flip_vertically_on_load(true);
    for (int i = 0; i < CubeMapFile.size(); i++)
    {
        data = stbi_load(CubeMapFile[i].c_str(),&width,&height,&nChannels,0);
        if (data == NULL)
        {
            std::cout << "ERROR::TEXTURE::FAILED TO LOAD TEXTURE: " <<CubeMapFile[i] << std::endl;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_SRGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    return CubeTexture;

}