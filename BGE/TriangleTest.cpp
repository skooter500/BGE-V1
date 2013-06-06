#include "TriangleTest.h"

using namespace BGE;

// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

TriangleTest::TriangleTest(void) {
}


TriangleTest::~TriangleTest(void) {
}

bool TriangleTest::Initialise() {

	// Initialise OpenGL, GLEW and SDL
	if (!Game::Initialise()) {
		return false;
	}
	programID = LoadShaders( "Shaders/SimpleVertexShader.vertexshader", "Shaders/SimpleFragmentShader.fragmentshader" );

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
 
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
 
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	return true;

}

void TriangleTest::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
	   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
 
	glUseProgram(programID);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
 
	glDisableVertexAttribArray(0);

	// Dont forget to call the superclass draw so that OpenGL Swap Buffers can be called
	Game::Draw();
}
