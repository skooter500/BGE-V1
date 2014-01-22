#include "Ground.h"
#include "Content.h"
#include <gtc/matrix_inverse.hpp>

using namespace BGE;

Ground::Ground(void):GameComponent()
{
	width = 5000;
	height = 5000;
}

bool Ground::rayIntersectsWorldPlane(glm::vec3 origin, glm::vec3 look, glm::vec3 & point)
{
	// Calculate t
	float t;
	glm::vec3 planeNormal = Transform::basisUp;
	float d = 0.0f;


	t = - d - glm::dot(planeNormal, origin) / (glm::dot(planeNormal, look));
	if (t > 0)
	{
		point = origin + (look * t);
		return true;
	}
	return false;
}


Ground::~Ground(void)
{
}

bool Ground::Initialise()
{
	if (initialised)
	{
		return true;
	}

	// Diffuse will come from the texture
	transform->ambient = glm::vec3(0.2f, 0.2, 0.2f);

	const float twidth = 500;
    const float theight = 500;

	if (!GameComponent::Initialise()) {
		return false;
	}


	vertices.push_back(glm::vec3(width, 0, -height));
	normals.push_back(Transform::basisUp);
	texels.push_back(glm::vec2(twidth, 0));

	vertices.push_back(glm::vec3(-width, 0, -height));
	normals.push_back(Transform::basisUp);
	texels.push_back(glm::vec2(0, 0));

	vertices.push_back(glm::vec3(width, 0, height));
	normals.push_back(Transform::basisUp);
	texels.push_back(glm::vec2(twidth, theight));
    
	vertices.push_back(glm::vec3(width, 0, height));
	normals.push_back(Transform::basisUp);
	texels.push_back(glm::vec2(twidth, theight));
	
	vertices.push_back(glm::vec3(-width, 0, -height));
	normals.push_back(Transform::basisUp);
	texels.push_back(glm::vec2(0,0));
	
	vertices.push_back(glm::vec3(-width, 0, height));
	normals.push_back(Transform::basisUp);
	texels.push_back(glm::vec2(0, theight)); 
    
	programID = Content::LoadShaderPair("standard_texture");

	textureID = Content::LoadTexture("BGE_Logo_01");

	glGenBuffers(1, &vertexbuffer); 
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), & vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &texelbuffer); 
	glBindBuffer(GL_ARRAY_BUFFER, texelbuffer);
	glBufferData(GL_ARRAY_BUFFER, texels.size() * sizeof(glm::vec2), & texels[0], GL_STATIC_DRAW);

	mID = glGetUniformLocation(programID,"m");
	vID = glGetUniformLocation(programID,"v");
	pID = glGetUniformLocation(programID,"p");
	nID = glGetUniformLocation(programID,"n");
	ambientID = glGetUniformLocation(programID,"ambient");
	specularID = glGetUniformLocation(programID,"specular");
	
	textureSampler  = glGetUniformLocation(programID, "myTextureSampler");

	initialised = true;

	return true;
}

void Ground::Draw()
{
	glUseProgram(programID);

	glUniformMatrix4fv(mID, 1, GL_FALSE, & transform->world[0][0]);
	glUniformMatrix4fv(vID, 1, GL_FALSE, & Game::Instance()->camera->view[0][0]);
	glUniformMatrix4fv(pID, 1, GL_FALSE, & Game::Instance()->camera->projection[0][0]);
	glUniform3f(specularID, transform->specular.r, transform->specular.g, transform->specular.b);
	glUniform3f(ambientID, transform->ambient.r, transform->ambient.g, transform->ambient.b);


	glm::mat4 MV = transform->world;
	glm::mat3 gl_NormalMatrix = glm::inverseTranspose(glm::mat3(MV));
	glUniformMatrix3fv(nID, 1, GL_FALSE, & gl_NormalMatrix[0][0]);
	
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(textureSampler, 0);

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

	// 2nd attribute buffer : texels
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

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6); 
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glUseProgram(0);	
	GameComponent::Draw();
}
void Ground::Cleanup()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &texelbuffer);
	glDeleteTextures(1, &textureID);
	glDeleteProgram(programID);
}
