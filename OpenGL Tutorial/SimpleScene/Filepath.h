#pragma once
#include<vector>
#include<string>


std::string skybox_vertPath = "./skybox_vs.vert";
std::string skybox_fragPath = "./skybox_fs.frag";
std::string model_vertPath = "./model_vs.vert";
std::string model_fragPath = "./model_fs.frag";
std::string sphere_vertshader = "./sphere_vs.vert";
std::string sphere_fragshader = "./sphere_fs.frag";
std::string utah_teapot = "D:/LearnOpenGL/3D model/utah_teapot/teapot.obj";
std::string sphere_path = "D:/LearnOpenGL/3D model/sphere/sphere.fbx";


std::vector<std::string> cubeMapPath = { 
	"D:/LearnOpenGL/3D model/skybox/right.jpg",
	"D:/LearnOpenGL/3D model/skybox/left.jpg",
	"D:/LearnOpenGL/3D model/skybox/top.jpg",
	"D:/LearnOpenGL/3D model/skybox/bottom.jpg",
	"D:/LearnOpenGL/3D model/skybox/front.jpg",
	"D:/LearnOpenGL/3D model/skybox/back.jpg" };
