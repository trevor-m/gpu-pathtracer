#include "Shader.h"

Shader::Shader(std::string vertexShaderPath, std::string fragmentShaderPath) {
	//create shaders
	GLuint vertex = createShader(GL_VERTEX_SHADER, vertexShaderPath);
	GLuint fragment = createShader(GL_FRAGMENT_SHADER, fragmentShaderPath);

	//create program and attach shaders
	Program = glCreateProgram();
	glAttachShader(Program, vertex);
	glAttachShader(Program, fragment);
	//link shaders
	glLinkProgram(Program);
	//check for link errors
	GLint success = 0;
	glGetProgramiv(Program, GL_LINK_STATUS, &success);
	if (!success) {
		//print errors
		GLchar buffer[ERROR_BUFFER_SIZE];
		glGetShaderInfoLog(Program, ERROR_BUFFER_SIZE, NULL, buffer);
		std::cout << "Error linking shader:" << std::endl << buffer << std::endl;
	}
	//free shaders now that they are compiled and linked into program
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::Shader(std::string computeShaderPath) {
	//create shaders
	GLuint compute = createShader(GL_COMPUTE_SHADER, computeShaderPath);

	//create program and attach shaders
	Program = glCreateProgram();
	glAttachShader(Program, compute);
	//link shaders
	glLinkProgram(Program);
	//check for link errors
	GLint success = 0;
	glGetProgramiv(Program, GL_LINK_STATUS, &success);
	if (!success) {
		//print errors
		GLchar buffer[ERROR_BUFFER_SIZE];
		glGetShaderInfoLog(Program, ERROR_BUFFER_SIZE, NULL, buffer);
		std::cout << "Error linking shader:" << std::endl << buffer << std::endl;
	}
	//free shaders now that they are compiled and linked into program
	glDeleteShader(compute);
}

GLuint Shader::createShader(GLenum shaderType, std::string path) {
	//create shader
	GLuint shader = glCreateShader(shaderType);
	//load file
	std::string shaderSource;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::badbit);
	try {
		shaderFile.open(path);
		std::stringstream stream;
		stream << shaderFile.rdbuf();
		shaderFile.close();
		shaderSource = stream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "Error reading file \"" << path << "\"" << std::endl;
	}
	const GLchar* source = shaderSource.c_str();
	//compile
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	//check for compile errors
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar buffer[ERROR_BUFFER_SIZE];
		glGetShaderInfoLog(shader, ERROR_BUFFER_SIZE, NULL, buffer);
		std::cout << "Error compiling \'" << path << "\': " << std::endl << buffer << std::endl;
	}
	return shader;
}

void Shader::Use() {
	glUseProgram(Program);
}