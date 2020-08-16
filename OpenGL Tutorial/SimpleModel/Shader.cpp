#include "Shader.h"
#include <glad\glad.h>
#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(std::string vertShader, std::string fragShader)
{
	std::ifstream vertStream;
	std::ifstream fragStream;

	std::string vertCode;
	std::string fragCode;

	//set exceptions thrown
	vertStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{   
		// read vertex shader
		vertStream.open(vertShader);	

		std::stringstream vShaderStream;
		std::stringstream fShaderStream;

		vShaderStream << vertStream.rdbuf();
		
		// read shader shader
		fragStream.open(fragShader);

		fShaderStream << fragStream.rdbuf();


		vertStream.close();
		fragStream.close();

		vertCode = vShaderStream.str();
		fragCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	

	// linking and compiling shaders
	unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
	unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);


	const char* vertPointer = vertCode.c_str();
	const char* fragPointer = fragCode.c_str();

	glShaderSource(vert, 1, &vertPointer, NULL);
	glShaderSource(frag, 1, &fragPointer, NULL);

	// compiling
	int success;
	char infoLog[512];

	glCompileShader(vert);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vert, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(frag, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vert);
	glAttachShader(ProgramID, frag);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS,&success);
	if (!success)
	{
		glGetProgramInfoLog(ProgramID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vert);
	glDeleteShader(frag);
}

void Shader::SetUpGeometryShader(std::string geomShader)
{
	std::ifstream geomStream;
	std::string geomCode;

	geomStream.exceptions(std::ifstream::failbit|std::ifstream::badbit);

	try 
	{
		geomStream.open(geomShader);

		std::stringstream geomStringStream;
		geomStringStream << geomStream.rdbuf();

		geomStream.close();
		geomCode = geomStringStream.str();
	}
	catch(std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	unsigned int geom = glCreateShader(GL_GEOMETRY_SHADER);
	const char* geomptr = geomCode.c_str();

	glShaderSource(geom,1,&geomptr,NULL);

	int success;
	char infoLog[512];

	glCompileShader(geom);
	glGetShaderiv(geom,GL_COMPILE_STATUS,&success);
	if (!success)
	{
		glGetShaderInfoLog(geom,512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glAttachShader(ProgramID,geom);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ProgramID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(geom);
}

void Shader::Use()
{
	glUseProgram(ProgramID);
}

void Shader::SetUniformInt(std::string name, int n)
{
	glUniform1i(glGetUniformLocation(ProgramID, name.c_str()),n);
}

void Shader::SetUniformMat4(std::string name, bool transpose, const float* value)
{
	glUniformMatrix4fv(glGetUniformLocation(ProgramID,name.c_str()),1,transpose,value);
}

void Shader::SetUniformVec3(std::string name, const float* value)
{
	glUniform3fv(glGetUniformLocation(ProgramID, name.c_str()),1,value);
}

void Shader::SetUniformf(std::string name, float f)
{
	glUniform1f(glGetUniformLocation(ProgramID, name.c_str()), f);
}