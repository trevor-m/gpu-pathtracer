#pragma once
#include <GL/glew.h>
#include <stdio.h>
#include "Shader.h"

class PathTracerBuffer {
private:
	GLuint frameTexture;
public:
	PathTracerBuffer(int frameWidth, int frameHeight) {
		// create texture to render to
		glGenTextures(1, &frameTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, frameWidth, frameHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
		glBindImageTexture(0, frameTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}

	void BindTextureWrite() {
		glBindImageTexture(0, frameTexture, 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}

	void BindTextureRead(Shader shader, GLuint unit) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameTexture);
		glUniform1i(glGetUniformLocation(shader.Program, "tex"), 0);
	}
};