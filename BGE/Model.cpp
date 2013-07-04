#include "Game.h"
#include "Model.h"
#include "Utils.h"
#include <gtc/matrix_inverse.hpp>
#include <iostream>

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
	programID = LoadShaders( "Shaders/standard.vertexshader", "Shaders/standard.fragmentshader" );
	
	//GLuint VertexArrayID;
	//glGenVertexArrays(1, &VertexArrayID);
	//glBindVertexArray(VertexArrayID);
	
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
	
	mID = glGetUniformLocation(programID,"m");
	vID = glGetUniformLocation(programID,"v");
	pID = glGetUniformLocation(programID,"p");
	m_3x3_inv_transp =  glGetUniformLocation(programID,"m_3x3_inv_transp");

	return true;
}

void Model::Update(float timeDelta)
{
	Yaw(timeDelta * 100.0f);
	GameComponent::Update(timeDelta);
}

void Model::Draw()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);
	glUniformMatrix4fv(mID, 1, GL_FALSE, & world[0][0]);
	glUniformMatrix4fv(vID, 1, GL_FALSE, & Game::Instance()->GetCamera()->GetView()[0][0]);
	glUniformMatrix4fv(pID, 1, GL_FALSE, & Game::Instance()->GetCamera()->GetProjection()[0][0]);
	
	glm::mat4 MV = Game::Instance()->GetCamera()->GetView() * world;
	glm::mat3 gl_NormalMatrix = glm::inverseTranspose(glm::mat3(MV));
	glUniformMatrix3fv(m_3x3_inv_transp, 1, GL_FALSE, & gl_NormalMatrix[0][0]);
	
	
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
		GL_TRUE,                         // normalized?
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
