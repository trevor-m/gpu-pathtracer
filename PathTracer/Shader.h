#pragma once
#include "GL/glew.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define ERROR_BUFFER_SIZE 512

class Shader
{
private:
	GLuint createShader(GLenum shaderType, std::string path);
public:
	Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
	Shader(std::string computeShaderPath);

	//Get program ID
	GLuint Program;

	//Activate this shader
	void Use();
};

