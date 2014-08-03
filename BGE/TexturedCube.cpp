#include "TexturedCube.h"
#include "Content.h"
#include<iostream>

using namespace BGE;
using namespace std;

static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
};

static const GLfloat g_uv_buffer_data[] = {
    0.000059f, 0.000004f,
    0.000103f, 0.336048f,
    0.335973f, 0.335903f,
    1.000023f, 0.000013f,
    0.667979f, 0.335851f,
    0.999958f, 0.336064f,
    0.667979f, 0.335851f,
    0.336024f, 0.671877f,
    0.667969f, 0.671889f,
    1.000023f, 0.000013f,
    0.668104f, 0.000013f,
    0.667979f, 0.335851f,
    0.000059f, 0.000004f,
    0.335973f, 0.335903f,
    0.336098f, 0.000071f,
    0.667979f, 0.335851f,
    0.335973f, 0.335903f,
    0.336024f, 0.671877f,
    1.000004f, 0.671847f,
    0.999958f, 0.336064f,
    0.667979f, 0.335851f,
    0.668104f, 0.000013f,
    0.335973f, 0.335903f,
    0.667979f, 0.335851f,
    0.335973f, 0.335903f,
    0.668104f, 0.000013f,
    0.336098f, 0.000071f,
    0.000103f, 0.336048f,
    0.000004f, 0.671870f,
    0.336024f, 0.671877f,
    0.000103f, 0.336048f,
    0.336024f, 0.671877f,
    0.335973f, 0.335903f,
    0.667969f, 0.671889f,
    1.000004f, 0.671847f,
    0.667979f, 0.335851f
};

TexturedCube::TexturedCube(void) {
}


TexturedCube::~TexturedCube(void) {
}

bool TexturedCube::Initialise() {
	// Initialise OpenGL, GLEW and SDL
	if (!Game::Initialise()) {
		return false;
	}
	programID = Content::LoadShaderPair("TexturedCubeShader");

	texture = Content::LoadTexture("numberedfaces");

	glGenBuffers(1, &vertexbuffer); 
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &texelbuffer); 
	glBindBuffer(GL_ARRAY_BUFFER, texelbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	textureSampler  = glGetUniformLocation(programID, "myTextureSampler");

	return true;
}

void TexturedCube::Draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);

	glm::mat4 Model = glm::mat4(1.0f);  
	glm::mat4 MVP = Game::Instance()->camera->projection 
		* Game::Instance()->camera->view 
		* Model; // Remember, matrix multiplication is the other way around
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(textureSampler, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : texels
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, texelbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Dont forget to call the superclass draw so that OpenGL Swap Buffers can be called
	Game::Draw();
}

void TexturedCube::Cleanup()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &texelbuffer);
	glDeleteTextures(1, &texture);
	glDeleteProgram(programID);
}