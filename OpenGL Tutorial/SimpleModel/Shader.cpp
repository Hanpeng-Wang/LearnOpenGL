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
		std::string line;
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