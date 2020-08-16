#pragma once
#include <string>


class Shader
{
public:
	unsigned int ProgramID;
	Shader(std::string vertShader, std::string fragShader);
	void Use();

	void SetUniformInt(std::string name, int n);
	void SetUniformMat4(std::string name, bool transpose, const float* value);
	void SetUniformVec3(std::string name, const float* value);
	void SetUniformf(std::string name, float f);

private:

	

};