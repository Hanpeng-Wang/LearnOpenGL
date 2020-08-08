#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>



Camera::Camera(glm::vec3 Pos, glm::vec3 front, glm::vec3 up) :CameraPos(Pos), CameraFront(front), CameraUp(up)
{
	this->WorldCenter = glm::vec3(0.0f);
	this->WorldUp = glm::vec3(0, 1, 0);

	this->yaw = 0.0f;
	this->pitch = 0.0f;

	this->sensitivity = 0.1f;

	this->view = glm::lookAt(CameraPos, WorldCenter, WorldUp);
	this->SetPerspective(45.0f, 800.f, 600.f, 0.1f, 100.0f);
}

void Camera::SetPerspective(float fov, float w, float h, float near, float far)
{
	this->fov = fov;
	this->width = w;
	this->height = h;
	this->near = near;
	this->far = far;

	this->projection = glm::perspective(glm::radians(fov), w/h, near, far);
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

void FlyCamera::Screen2DRotation(float xlast, float ylast, float xnew, float ynew)
{
	float xoffset = xnew - xlast;
	float yoffset = ynew - ylast;

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

	this->projection = glm::perspective(glm::radians(fov),width/height,near,far);
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

void ViewCamera::Screen2DRotation(float xlast, float ylast, float xnew, float ynew)
{
	float xoffset = xnew - xlast;
	float yoffset = ynew - ylast;

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

	this->projection = glm::perspective(glm::radians(fov),width/height, near, far);
}

TrackBall::TrackBall(glm::vec3 Pos, glm::vec3 front, glm::vec3 up) : Camera(Pos, front, up)
{
	this->model = glm::mat4(1.0f);
}

void TrackBall::Screen2DRotation(float xlast, float ylast, float xnew, float ynew)
{
	glm::vec3 p1 = Screen2VirtualBall(xlast, ylast);
	glm::vec3 p2 = Screen2VirtualBall(xnew,ynew);

	glm::vec3 Raxis = glm::normalize(glm::cross(p1, p2));

	float d = glm::acos(glm::dot(p1, p2));

	this->model = glm::rotate(d, Raxis) * model;
	this->view = glm::lookAt(CameraPos, WorldCenter, WorldUp) * model;
}

glm::vec3 TrackBall::Screen2VirtualBall(float x, float y)
{
	float radius = (height < width) ? height/2 : width/2;

	glm::vec2 p;
	p.x = (x - width / 2) / radius;
	p.y = (height / 2 - y) / radius;

	glm::vec3 pMapped;

	if (glm::length(p) >= 1)
	{
		p = glm::normalize(p);
		pMapped = glm::vec3(p, 0);
	}
	else
	{
		float z = sqrt(1 - p.x * p.x - p.y * p.y);
		pMapped = glm::vec3(p, z);
	}

	return pMapped;
}

void TrackBall::ScreenZoom(float yoffset)
{
	this->fov += yoffset;

	if (this->fov < 1.0f)
		this->fov = 1.0f;
	if (this->fov > 60.0f)
		this->fov = 60.0f;

	this->projection = glm::perspective(glm::radians(fov), width / height, near, far);

	
}