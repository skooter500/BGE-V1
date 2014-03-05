#include "LineDrawer.h"
#include "Content.h"
#include "Game.h"
#include "Utils.h"

using namespace BGE;

LineDrawer * LineDrawer::instance = NULL;

LineDrawer::LineDrawer(void)
{
}

LineDrawer * LineDrawer::Instance()
{
	if (instance == NULL)
	{
		instance = new LineDrawer();
	}
	return instance;
}

LineDrawer::~LineDrawer(void)
{
}

bool LineDrawer::Initialise()
{
	programID = Content::LoadShaderPair( "Lines");
	glUseProgram(programID);
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, MAX_LINES * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);	

	vID = glGetUniformLocation(programID,"v");
	pID = glGetUniformLocation(programID,"p");
	
	glGenBuffers(1, &colourBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, colourBufferID);
	glBufferData(GL_ARRAY_BUFFER, MAX_LINES * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);

	glUseProgram(0);

	return true;
}

void LineDrawer::Draw()
{
	if (vertices.size() > 0)
	{
		glUseProgram(programID);

		// Copy the data into the buffers for drawing
		
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), & vertices[0]);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colourBufferID);
		glVertexAttribPointer(
			1,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
		glBufferSubData(GL_ARRAY_BUFFER, 0, colours.size() * sizeof(glm::vec3), & colours[0]);
		glUniformMatrix4fv(vID, 1, GL_FALSE, & Game::Instance()->camera->view[0][0]);
		glUniformMatrix4fv(pID, 1, GL_FALSE, & Game::Instance()->camera->projection[0][0]);

		// Draw the lines !
		glDrawArrays(GL_LINES, 0, vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glUseProgram(0);

		colours.clear();
		vertices.clear();
	}
}

void LineDrawer::CleanUp()
{
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &colourBufferID);
	glDeleteProgram(programID);
}

void LineDrawer::DrawArrowLine(glm::vec3 start, glm::vec3 end, glm::vec3 colour, glm::quat arrowQuat)
{
	LineDrawer * lineDrawer = LineDrawer::Instance();
	lineDrawer->vertices.push_back(start);
	lineDrawer->vertices.push_back(end);
	lineDrawer->colours.push_back(colour);
	lineDrawer->colours.push_back(colour);

	float side = 1;
	float back = 2;
	glm::vec3 points[3];
	points[0] = glm::vec3(-side, 0, back);
	points[1] = glm::vec3(0, 0, 0);
	points[2] = glm::vec3(side, 0, back);
	
	for (int i = 0 ; i < 3 ; i ++)
	{
		points[i] = RotateVector(points[i], arrowQuat) + end;
	}

	lineDrawer->vertices.push_back(points[0]);
	lineDrawer->vertices.push_back(points[1]);
	lineDrawer->vertices.push_back(points[2]);
	lineDrawer->vertices.push_back(points[1]);
	lineDrawer->colours.push_back(colour);
	lineDrawer->colours.push_back(colour);
	lineDrawer->colours.push_back(colour);
	lineDrawer->colours.push_back(colour);
}

void LineDrawer::DrawLine(glm::vec3 start, glm::vec3 end, glm::vec3 colour)
{
	LineDrawer * lineDrawer = LineDrawer::Instance();
	lineDrawer->vertices.push_back(start);
	lineDrawer->vertices.push_back(end);
	lineDrawer->colours.push_back(colour);
	lineDrawer->colours.push_back(colour);
}
