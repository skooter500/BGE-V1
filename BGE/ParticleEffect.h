#pragma once
#include "GameComponent.h"
#include "Particle.h"
#include<list>
#include<vector>

#define MAX_PARTICLES 1000

using namespace std;

namespace BGE
{
	
	class ParticleEffect :
		public GameComponent
	{
	public:
		ParticleEffect(void);
		~ParticleEffect(void);

		bool Initialise();
		void Update(float timeDelta);
		void Draw();

		virtual void InitParticle(Particle & particle) = 0;
		virtual void UpdateParticle(float timeDelta, Particle & particle) = 0;

		list<Particle> particles;
		vector<glm::vec3> vertices;
		vector<glm::vec4> colours;
		vector<float> sizes;
		string textureName;
	private:
		GLuint vertexbuffer;
		GLuint programID;
		GLuint colourBufferID;
		GLuint vertexBufferID;
		GLuint sizeBufferID;
		GLuint mID, vID, pID;
		GLuint pointSizeID;
		GLuint textureID;
		GLuint textureSampler;
	};
}

