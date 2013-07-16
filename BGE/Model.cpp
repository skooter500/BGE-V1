#include "Game.h"
#include "Model.h"
#include "Content.h"
#include <gtc/matrix_inverse.hpp>
#include <iostream>

using namespace BGE;
using namespace std;

Model::Model():GameComponent()
{
	drawMode = draw_modes::materials;
	ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	specular = glm::vec3(1.2f, 1.2f, 1.2f);
}

Model::~Model()
{
}
		

bool Model::Initialise()
{

	GameComponent::Initialise();
	programID = Content::LoadShaderPair( "standard_materials");
	
	//GLuint VertexArrayID;
	//glGenVertexArrays(1, &VertexArrayID);
	//glBindVertexArray(VertexArrayID);
	
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);	
		
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	
	glUseProgram(programID);
	
	mID = glGetUniformLocation(programID,"m");
	vID = glGetUniformLocation(programID,"v");
	pID = glGetUniformLocation(programID,"p");
	nID = glGetUniformLocation(programID,"n");
	ambientID = glGetUniformLocation(programID,"ambient");
	specularID = glGetUniformLocation(programID,"specular");
	diffuseID = glGetUniformLocation(programID,"diffuse");
	diffusePerVertexID = glGetUniformLocation(programID,"diffusePerVertex");
	if (drawMode == draw_modes::materials)
	{
		
		glGenBuffers(1, &colourbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colourbuffer);
		glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(glm::vec3), &colours[0], GL_STATIC_DRAW);
	}

	return true;
}

void Model::Draw()
{
	glUseProgram(programID);
	glUniformMatrix4fv(mID, 1, GL_FALSE, & world[0][0]);
	glUniformMatrix4fv(vID, 1, GL_FALSE, & Game::Instance()->GetCamera()->GetView()[0][0]);
	glUniformMatrix4fv(pID, 1, GL_FALSE, & Game::Instance()->GetCamera()->GetProjection()[0][0]);

	if (drawMode == draw_modes::materials)
	{
		glUniform3f(diffuseID, ambient.r, ambient.g, ambient.b); 
		glUniform1i(diffusePerVertexID, false);
	}
	else
	{
		glUniform1i(diffusePerVertexID, true);
	}
	glUniform3f(specularID, specular.r, specular.g, specular.b);
	glUniform3f(diffuseID, diffuse.r, diffuse.g, diffuse.b);


	glm::mat4 MV = Game::Instance()->GetCamera()->GetView() * world;
	glm::mat3 gl_NormalMatrix = glm::inverseTranspose(glm::mat3(MV));
	glUniformMatrix3fv(nID, 1, GL_FALSE, & gl_NormalMatrix[0][0]);
	
	
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
