#include "Ground.h"
#include "Content.h"

using namespace BGE;

Ground::Ground(void):GameComponent()
{
	width = 500;
	height = 500;
}


Ground::~Ground(void)
{
}

bool Ground::Initialise()
{
	const float twidth = 50;
    const float theight = 50;

	if (!GameComponent::Initialise()) {
		return false;
	}

	vertices.push_back(glm::vec3(width, 0, -height));
	texels.push_back(glm::vec2(twidth, 0));
	vertices.push_back(glm::vec3(-width, 0, -height));
	texels.push_back(glm::vec2(0, 0));
	vertices.push_back(glm::vec3(width, 0, height));
	texels.push_back(glm::vec2(twidth, theight));
    vertices.push_back(glm::vec3(width, 0, height));
	texels.push_back(glm::vec2(twidth, theight));
	vertices.push_back(glm::vec3(-width, 0, -height));
	texels.push_back(glm::vec2(0,0));
	vertices.push_back(glm::vec3(-width, 0, height));
	texels.push_back(glm::vec2(0, theight)); 
    
	programID = Content::LoadShaderPair("TexturedCubeShader");

	textureID = Content::LoadTexture("ground");

	glGenBuffers(1, &vertexbuffer); 
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), & vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &texelbuffer); 
	glBindBuffer(GL_ARRAY_BUFFER, texelbuffer);
	glBufferData(GL_ARRAY_BUFFER, texels.size() * sizeof(glm::vec2), & texels[0], GL_STATIC_DRAW);

	MVPID = glGetUniformLocation(programID, "MVP");
	ModelViewID = glGetUniformLocation(programID, "ModelView");

	textureSampler  = glGetUniformLocation(programID, "myTextureSampler");

	return true;
}

void Ground::Draw()
{
	glUseProgram(programID);

	glm::mat4 Model = glm::mat4(1.0f);  
	glm::mat4 ModelView = Game::Instance()->GetCamera()->GetView() 
		* Model; // Remember, matrix multiplication is the other way around	
	glUniformMatrix4fv(ModelViewID, 1, GL_FALSE, &ModelView[0][0]);
	glm::mat4 MVP = Game::Instance()->GetCamera()->GetProjection() 
		* ModelView;
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
	
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

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
	glDrawArrays(GL_TRIANGLES, 0, 6); // 12*3 indices starting at 0 -> 12 triangles

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	// Dont forget to call the superclass draw so that OpenGL Swap Buffers can be called
	GameComponent::Draw();
}
void Ground::Cleanup()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &texelbuffer);
	glDeleteTextures(1, &textureID);
	glDeleteProgram(programID);
}
