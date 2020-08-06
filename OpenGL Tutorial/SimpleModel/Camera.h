#pragma once
#include<glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// camera interface
class Camera
{
public:
	glm::vec3 CameraPos;
	glm::vec3 CameraFront;
	glm::vec3 CameraUp;

	glm::vec3 WorldCenter;
	glm::vec3 WorldUp;

	float yaw;
	float pitch;

	float fov;
	float aspect;
	float near;
	float far;
	float sensitivity;

	glm::mat4 view;
	glm::mat4 projection;

	Camera(glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 front = glm::vec3(0.0,0.0,-1.0), 
		glm::vec3 up = glm::vec3(0,1,0));
	void SetPerspective(float fov, float aspect, float near, float far);
	void SetSensitivity(float s);
	virtual void Screen2DRotation(float xoffset,float yoffset) = 0;
	virtual void ScreenZoom(float yoffset) = 0;

    glm::mat4& GetViewMatrix();
    glm::mat4& GetProjectionMatrix();
};

class FlyCamera: public Camera
{
public:
	FlyCamera(glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 front = glm::vec3(0, 0, -1),
		glm::vec3 up = glm::vec3(0, 1, 0));
	
	void Screen2DRotation(float xoffset, float yoffset);
	void UpdateEulerAngles(float pitch_update, float yaw_update);
    void ScreenZoom(float yoffset);
};

class ViewCamera: public Camera
{
public:
	ViewCamera(glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 front = glm::vec3(0, 0, -1),
		glm::vec3 up = glm::vec3(0, 1, 0));

	void Screen2DRotation(float xoffset, float yoffset);
	void UpdateEulerAngles(float pitch_update, float yaw_update);
	void ScreenZoom(float yoffset);
};

