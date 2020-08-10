#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

std::ostream& operator <<(std::ostream& out, const glm::vec3 p)
{
	out << p.x << ", " << p.y << ", " << p.z;
	return out;
}

Camera::Camera(glm::vec3 Pos, glm::vec3 front, glm::vec3 up) :CameraPos(Pos), CameraFront(front), CameraUp(up)
{
	this->WorldCenter = glm::vec3(0.0f);
	this->WorldUp = glm::vec3(0, 1, 0);

	this->yaw = 0.0f;
	this->pitch = 0.0f;

	this->sensitivity = 0.1f;

	this->model = glm::mat4(1.0f);
	this->view = glm::lookAt(CameraPos, CameraPos + front, WorldUp);
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

glm::mat4& Camera::GetModelMatrix()
{
	return this->model;
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


	glm::vec3 pitch_axis = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 yaw_axis = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 pitchRotation = glm::rotate(glm::radians(this->pitch),pitch_axis);
	glm::mat4 yawRotation = glm::rotate(glm::radians(this->yaw),yaw_axis);

	this->view =glm::lookAt(CameraPos,CameraPos+CameraFront,WorldUp)*pitchRotation * yawRotation;
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
	this->RotationCenter = glm::vec3(0.0f);
	this->ModelInner = glm::mat4(1.0f);
	this->model = glm::translate(glm::mat4(1.0f),RotationCenter)*ModelInner*glm::translate(glm::mat4(1.0f),-RotationCenter);
}

void TrackBall::Screen2DRotation(float xlast, float ylast, float xnew, float ynew)
{
	glm::vec3 p1 = Screen2VirtualBall(xlast, ylast);
	glm::vec3 p2 = Screen2VirtualBall(xnew,ynew);

	//std::cout << "1" << std::endl;

	if (p1 == p2)
		return;

	glm::vec3 Raxis = glm::normalize(glm::cross(p1, p2));

	if (glm::dot(p1, p2) > 1.0)
		return;

	float d = glm::acos(glm::dot(p1, p2));
	
	

	this->ModelInner = glm::rotate(d, Raxis) * ModelInner;
	this->model = glm::translate(glm::mat4(1.0f), RotationCenter) * ModelInner * glm::translate(glm::mat4(1.0f), -RotationCenter);
	//std::cout << p1 << "    " << p2 << "   " << d << std::endl;
	//std::cout << this->model[0][0] << std::endl;
	
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

void TrackBall::SetRotationCenter(glm::vec3 r)
{
	this->RotationCenter = r;

}