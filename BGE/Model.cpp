#include "Game.h"
#include "Model.h"
#include "Content.h"
#include <gtc/matrix_inverse.hpp>
#include <iostream>
#include <limits>

using namespace BGE;
using namespace std;

Model::Model():GameComponent()
{
	drawMode = draw_modes::materials;
	ambient = glm::vec3(0.5f, 0.5f, 0.5f);
	specular = glm::vec3(1.2f, 1.2f, 1.2f);
	localTransform = glm::mat4(1);
	worldMode = GameComponent::from_parent; // Get the world transform from my parent
	textureID = 0;
}

Model::~Model()
{
}
		

bool Model::Initialise()
{
	GameComponent::Initialise();
	if (!Game::Instance()->initialised)
	{
		return false;
	}

	if (initialised)
	{
		return true;
	}

	switch (drawMode)
	{
	case draw_modes::materials:
	case draw_modes::single_material:
		programID = Content::LoadShaderPair( "standard_materials");
		glGenBuffers(1, &colourbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colourbuffer);
		glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(glm::vec3), &colours[0], GL_STATIC_DRAW);
		break;
	case draw_modes::textured:
		programID = Content::LoadShaderPair( "standard_texture");
		glGenBuffers(1, &texelbuffer); 
		glBindBuffer(GL_ARRAY_BUFFER, texelbuffer);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), & uvs[0], GL_STATIC_DRAW);
		textureID = Content::LoadTexture(textureName);
		textureSampler  = glGetUniformLocation(programID, "myTextureSampler");
		break;
	}
	
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
	//mvpID = glGetUniformLocation(programID,"MVP");

	ambientID = glGetUniformLocation(programID,"ambient");
	specularID = glGetUniformLocation(programID,"specular");
	diffuseID = glGetUniformLocation(programID,"diffuse");
	diffusePerVertexID = glGetUniformLocation(programID,"diffusePerVertex");
	
	CalculateBounds();
	glUseProgram(0);
	initialised = true;
	return true;
}

void Model::CalculateBounds()
{
	boundingBox.min.x = numeric_limits<float>::max();
	boundingBox.min.y = numeric_limits<float>::max();
	boundingBox.min.z = numeric_limits<float>::max();

	boundingBox.max.x = numeric_limits<float>::min();
	boundingBox.max.y = numeric_limits<float>::min();
	boundingBox.max.z = numeric_limits<float>::min();

	std::vector<glm::vec3>::iterator it = vertices.begin();
	while (it != vertices.end())
	{
		if (it->x < boundingBox.min.x)
		{
			boundingBox.min.x = it->x;
		}
		if (it->y < boundingBox.min.y)
		{
			boundingBox.min.y = it->y;
		}
		if (it->z < boundingBox.min.z)
		{
			boundingBox.min.z = it->z;
		}

		if (it->x > boundingBox.max.x)
		{
			boundingBox.max.x = it->x;
		}
		if (it->y > boundingBox.max.y)
		{
			boundingBox.max.y = it->y;
		}
		if (it->z > boundingBox.max.z)
		{
			boundingBox.max.z = it->z;
		}
		it ++;
	}
}



void Model::Draw()
{	
	world = world * localTransform;
	glUseProgram(programID);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Models are singletons, so they share a world transform, so use my parent's world transform instead
	glUniformMatrix4fv(mID, 1, GL_FALSE, & world[0][0]);
	glUniformMatrix4fv(vID, 1, GL_FALSE, & Game::Instance()->camera->view[0][0]);
	glUniformMatrix4fv(pID, 1, GL_FALSE, & Game::Instance()->camera->projection[0][0]);

	switch (drawMode)
	{
	case draw_modes::single_material:
		glUniform3f(diffuseID, diffuse.r, diffuse.g, diffuse.b); 
		glUniform1i(diffusePerVertexID, false);
		break;
	case draw_modes::materials:
		glUniform1i(diffusePerVertexID, true);
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
		break;
	case draw_modes::textured:
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, texelbuffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size : U+V => 2
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);	// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(textureSampler, 0);
		break;		
	}

	glUniform3f(specularID, specular.r, specular.g, specular.b);
	glUniform3f(ambientID, ambient.r, ambient.g, ambient.b);

	glm::mat4 MV = world;
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

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glUseProgram(0);
	// Draw my children
	GameComponent::Draw();
}
