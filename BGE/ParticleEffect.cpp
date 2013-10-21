#include "ParticleEffect.h"
#include "Content.h"

using namespace BGE;

ParticleEffect::ParticleEffect(void)
{
	textureName = "flare";
	tag = "ParticleEffect";
}


ParticleEffect::~ParticleEffect(void)
{
}

bool BGE::ParticleEffect::Initialise()
{
	if (initialised)
	{
		return true;
	}

	programID = Content::LoadShaderPair( "Particles");
	glUseProgram(programID);


	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);	

	mID = glGetUniformLocation(programID,"m");
	vID = glGetUniformLocation(programID,"v");
	pID = glGetUniformLocation(programID,"p");

	glGenBuffers(1, &colourBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, colourBufferID);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	
	glGenBuffers(1, &sizeBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, sizeBufferID);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	textureSampler  = glGetUniformLocation(programID, "myTextureSampler");
	textureID = Content::LoadTexture(textureName);


	glUseProgram(0);
	initialised = true;
	return GameComponent::Initialise();
}

void BGE::ParticleEffect::Update( float timeDelta )
{
	GameComponent::Update(timeDelta);
	colours.clear();
	vertices.clear();
	sizes.clear();
	list<Particle>::iterator it = particles.begin();
	alive = false;
	while (it != particles.end())
	{
		if (it->alive)
		{
			UpdateParticle(timeDelta, * it);
			if (it->alive)
			{
				// The system remains alive so long as at least one of its particles is alive;
				alive = true;
				colours.push_back(it->diffuse);
				vertices.push_back(it->position);
				sizes.push_back(it->size);
			}			
		}
		it ++;
	}
	GameComponent::Update(timeDelta);	
}

void ParticleEffect::Draw()
{
	if (vertices.size() > 0)
	{
		glUseProgram(programID);

		//Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);	// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(textureSampler, 0);

		glEnable(GL_BLEND);	
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
			4,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
		glBufferSubData(GL_ARRAY_BUFFER, 0, colours.size() * sizeof(glm::vec4), & colours[0]);
		
		
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, sizeBufferID);
		glVertexAttribPointer(
			2,                  // attribute
			1,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizes.size() * sizeof(float), & sizes[0]);

		// The particles are already in world space, so we wont be using this anyway
		
		glUniformMatrix4fv(mID, 1, GL_FALSE, & world[0][0]);
		glUniformMatrix4fv(vID, 1, GL_FALSE, & Game::Instance()->camera->view[0][0]);
		glUniformMatrix4fv(pID, 1, GL_FALSE, & Game::Instance()->camera->projection[0][0]);
		
		glUniform1f(pointSizeID, 20.0f);

		glDrawArrays(GL_POINTS, 0, vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glUseProgram(0);


		colours.clear();
		vertices.clear();
	}

	return GameComponent::Draw();
}
