#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>



Camera::Camera(glm::vec3 Pos, glm::vec3 front, glm::vec3 up) :CameraPos(Pos), CameraFront(front), CameraUp(up)
{
	this->WorldCenter = glm::vec3(0.0f);
	this->WorldUp = glm::vec3(0, 1, 0);

	this->yaw = 0.0f;
	this->pitch = 0.0f;

	this->sensitivity = 0.1f;

	this->view = glm::lookAt(CameraPos, WorldCenter, WorldUp);
	this->SetPerspective(45.0f, 1.0f, 0.1f, 100.0f);
}

void Camera::SetPerspective(float fov, float aspect, float near, float far)
{
	this->fov = fov;
	this->aspect = aspect;
	this->near = near;
	this->far = far;

	this->projection = glm::perspective(glm::radians(fov), aspect, near, far);
}

void Camera::SetSensitivity(float s)
{
	this->sensitivity = s;
}

glm::mat4& Camera::GetViewMatrix()
{
	return this->view;
}

glm::mat4& Camera::GetProjectionMatrix()
{
	return this->projection;
}



FlyCamera::FlyCamera(glm::vec3 Pos, glm::vec3 front, glm::vec3 up): Camera(Pos, front, up)
{
	this->sensitivity = 0.1f;
	this->yaw = -90.f;
}

void FlyCamera::Screen2DRotation(float xoffset, float yoffset)
{
	float pitch_update = -(this->sensitivity * yoffset);
	float yaw_update = this->sensitivity * xoffset;

	UpdateEulerAngles(pitch_update,yaw_update);
}

void FlyCamera::ScreenZoom(float yoffset)
{
	this->fov += yoffset;

	if(this->fov < 1.0f)
		this->fov = 1.0f;
	if (this->fov > 60.0f)
		this->fov = 60.0f;

	this->projection = glm::perspective(glm::radians(fov),aspect,near,far);
}

void FlyCamera::UpdateEulerAngles(float pitch_update, float yaw_update)
{
	this->pitch += pitch_update;
	this->yaw += yaw_update;

	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	this->CameraFront = glm::normalize(front);

	glm::vec3 right = glm::cross(CameraFront,WorldUp);
	this->CameraUp = glm::normalize(glm::cross(right,CameraFront));

	this->view = glm::lookAt(CameraPos,(CameraPos+CameraFront),CameraUp);

	
}

ViewCamera::ViewCamera(glm::vec3 Pos, glm::vec3 front, glm::vec3 up) : Camera(Pos, front, up)
{
	this->sensitivity = 0.5f;
	this->yaw = 90.0f;
}

void ViewCamera::Screen2DRotation(float xoffset, float yoffset)
{
	float pitch_update = this->sensitivity * yoffset;
	float yaw_update = this->sensitivity * xoffset;

	UpdateEulerAngles(pitch_update, yaw_update);
}

void ViewCamera::UpdateEulerAngles(float pitch_update, float yaw_update)
{
	this->pitch += pitch_update;
	this->yaw += yaw_update;

	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;

	glm::vec3 new_pos;
	new_pos.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	new_pos.y = sin(glm::radians(pitch));
	new_pos.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	this->CameraPos = glm::length(CameraPos) * glm::normalize(new_pos);
	CameraFront = glm::normalize(-CameraPos);

	glm::vec3 right = glm::cross(CameraFront, WorldUp);
	this->CameraUp = glm::normalize(glm::cross(right, CameraFront));

	this->view = glm::lookAt(CameraPos, WorldCenter, CameraUp);
}

void ViewCamera::ScreenZoom(float yoffset)
{
	this->fov += yoffset;

	if (this->fov < 1.0f)
		this->fov = 1.0f;
	if (this->fov > 60.0f)
		this->fov = 60.0f;

	this->projection = glm::perspective(glm::radians(fov), aspect, near, far);
}

