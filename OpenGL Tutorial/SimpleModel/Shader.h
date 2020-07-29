#pragma once
#include <string>


class Shader
{
public:
	unsigned int ProgramID;
	Shader(std::string vertShader, std::string fragShader);
	void Use();

	void SetUniformInt(std::string name, int n);

private:

	

};