#include "Game.h"
#include "Model.h"
#include "Utils.h"
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
	programID = LoadShaders( "Shaders/diffuse.vertexshader", "Shaders/diffuse.fragmentshader" );
	
	
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
	
	// Get a handle for our "MVP" uniform
	m_WVPLocation = glGetUniformLocation(programID,"gWVP");
    m_WorldMatrixLocation = glGetUniformLocation(programID,"gWorld");

	m_dirLightLocation.Color = glGetUniformLocation(programID,"gDirectionalLight.Color");
    m_dirLightLocation.AmbientIntensity = glGetUniformLocation(programID,"gDirectionalLight.AmbientIntensity");
    m_dirLightLocation.Direction = glGetUniformLocation(programID,"gDirectionalLight.Direction");
    m_dirLightLocation.DiffuseIntensity = glGetUniformLocation(programID,"gDirectionalLight.DiffuseIntensity");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
    m_directionalLight.AmbientIntensity = 0.5f;
    m_directionalLight.DiffuseIntensity = 0.75f;
    m_directionalLight.Direction = glm::vec3(1.0f, -1.0, 0.0);
		
	return true;
}

void Model::SetDirectionalLight(const DirectionalLight& Light)
{
    glUniform3f(m_dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
    glUniform1f(m_dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
    glm::vec3 Direction = Light.Direction;
	Direction = glm::normalize(Direction);
    glUniform3f(m_dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
    glUniform1f(m_dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
}

void Model::Update(float timeDelta)
{
	Yaw(timeDelta * 100.0f);
	GameComponent::Update(timeDelta);
}

void Model::Draw()
{
	cout << look.x << " " << look.y << " " << look.z << endl;
	glUseProgram(programID);
	glm::mat4 MVP = Game::Instance()->GetCamera()->GetProjection() 
		* Game::Instance()->GetCamera()->GetView() 
		* world; // Remember, matrix multiplication is the other way around
	glUniformMatrix4fv(m_WVPLocation, 1, GL_FALSE, & MVP[0][0]);
	glUniformMatrix4fv(m_WorldMatrixLocation, 1, GL_FALSE, & world[0][0]);
	SetDirectionalLight(m_directionalLight);

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
