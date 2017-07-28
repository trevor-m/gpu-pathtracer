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

		//query work group info
		int work_grp_cnt[3];
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
		printf("max global (total) work group size x:%i y:%i z:%i\n", work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);
		int work_grp_size[3];

		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

		printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
			work_grp_size[0], work_grp_size[1], work_grp_size[2]);
		int work_grp_inv;
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
		printf("max local work group invocations %i\n", work_grp_inv);
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