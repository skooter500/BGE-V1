#include "Game.h"
#include "Model.h"
#include "Utils.h"


using namespace BGE;
using namespace std;

Model::Model()
{
}

Model::~Model()
{
}
		

bool Model::Initialise()
{
	GameComponent::Initialise();
	programID = LoadShaders( "Shaders/StandardShading.vertexshader", "Shaders/StandardShading.fragmentshader" );
	
	//programID = LoadShaders( "Shaders/CubeVertexShader.vertexshader", "Shaders/CubeFragmentShader.fragmentshader" );
	
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);	
		
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &colourbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colourbuffer);
	glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(glm::vec3), &colours[0], GL_STATIC_DRAW);
	
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
		
	return true;
}

void Model::Draw()
{
	glUseProgram(programID);
	glm::mat4 MVP = Game::Instance()->GetCamera()->GetProjection() 
		* Game::Instance()->GetCamera()->GetView() 
		* world; // Remember, matrix multiplication is the other way around
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, & MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, & world[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, & Game::Instance()->GetCamera()->GetView()[0][0]);

	glm::vec3 lightPos = glm::vec3(4,4,4);
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

	// 0th  attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 1st attribute buffer : normals
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 2nd attribute buffer : colours
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, colourbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	
	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	
	// Draw my children
	GameComponent::Draw();
}
