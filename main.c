#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef unsigned int uint;
typedef unsigned char byte;

char* readFile(const char* filePath)
{
	FILE* fp = fopen(filePath, "r");
	assert(fp);
	assert(fseek(fp, 0, SEEK_END) == 0);
	size_t len = ftell(fp);
	assert(fseek(fp, 0, SEEK_SET) == 0);
	char* buf = (char*)malloc(len + 1);
	assert(buf);
	assert(memset(buf, 0, len + 1));
	fread(buf, 1, len, fp);
	buf[len] = 0;
	assert(!ferror(fp));
	fclose(fp);
	return buf;
}

const float vertices[] = {
	-1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f,  1.0f,	5.0f, 0.0f, 
	 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, -1.0f, 5.0f, 0.0f,
	 0.0f, 1.0f,  0.0f, 2.5f, 5.0f
};

const uint indices[] = {
	0, 1, 2,
	2, 3, 0,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};

int main()
{
	uint vao, vbo, ebo, vert, frag, prog, model, view, proj, tex, sampler;	
	mat4 model_mat = GLM_MAT4_IDENTITY_INIT, view_mat = GLM_MAT4_IDENTITY_INIT, proj_mat = GLM_MAT4_IDENTITY_INIT;
	const char* vertsrc = readFile("a.vert");
	const char* fragsrc = readFile("a.frag");
	int shader_status;
	char shader_infolog[1024];
	vec4 up = {0.0f, 1.0f, 0.0f, 1.0f};
	vec4 back = {0.0f, -1.0f, -3.5f, 1.0f};
	vec4 bigbig = {2.0f, 2.0f, 2.0f, 1.0f};
	int w, h, nrch;
	byte* data;	

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Pyramid", NULL, NULL);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	gladLoadGL();

	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vertsrc, NULL);
	glCompileShader(vert);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &shader_status);
	if(!shader_status) {
		glGetShaderInfoLog(vert , 1024, NULL, shader_infolog);
		printf("%s\n", shader_infolog);
		return -1;
	}

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragsrc, NULL);
	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &shader_status);
	if(!shader_status) {
		glGetShaderInfoLog(frag, 1024, NULL, shader_infolog);
		printf("%s\n", shader_infolog);
		return -1;
	}

	prog = glCreateProgram();
	glUseProgram(prog);
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);	
	glLinkProgram(prog);
	glDeleteShader(vert);
	glDeleteShader(frag);
	free((void*)vertsrc);
	free((void*)fragsrc);
	model = glGetUniformLocation(prog, "model");
	view = glGetUniformLocation(prog, "view");
	proj = glGetUniformLocation(prog, "proj");
	sampler = glGetUniformLocation(prog, "sampler");
	glUniform1i(sampler, 0);

	glm_scale(model_mat, bigbig);
	glm_translate(view_mat, back);
	glm_perspective(glm_rad(120.0f), 1920.0f / 1080.0f, 0.1f, 100.0f, proj_mat);
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (void*)12);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(1);
	data = stbi_load("legend.jpeg", &w, &h, &nrch, 0);
	assert(data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	glViewport(0, 0, 800, 800);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(prog);
	glBindVertexArray(vao);

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.0f, 0.3f, 0.45f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm_rotate(model_mat, glm_rad(glfwGetTime() * 0.1f), up);
		glUniformMatrix4fv(model, 1, GL_FALSE, (GLfloat*)model_mat);
		glUniformMatrix4fv(view, 1, GL_FALSE, (GLfloat*)view_mat);
		glUniformMatrix4fv(proj, 1, GL_FALSE, (GLfloat*)proj_mat);

		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;		
}
