#pragma once
#include "GameComponent.h"
#include <vector>
#include <iostream>
#include "Route.h"

using namespace std;

namespace BGE
{

	class SteeringController :
		public GameComponent
	{
	private:
		void makeFeelers();
		static vector<shared_ptr<GameComponent>> steerables;
		static vector<shared_ptr<GameComponent>> obstacles;
		static bool counted;
		int deleteMe; 
	public:
		SteeringController(void);
		~SteeringController(void);

		enum CalculationMethods { WeightedTruncatedSum, WeightedTruncatedRunningSumWithPrioritisation, PrioritisedDithering };
		enum behaviour_type
		{
			none = 0x00000,
			seek = 0x00002,
			flee = 0x00004,
			arrive = 0x00008,
			wander = 0x00010,
			cohesion = 0x00020,
			separation = 0x00040,
			alignment = 0x00080,
			obstacle_avoidance = 0x00100,
			wall_avoidance = 0x00200,
			follow_path = 0x00400,
			pursuit = 0x00800,
			evade = 0x01000,
			interpose = 0x02000,
			hide = 0x04000,
			flock = 0x08000,
			offset_pursuit = 0x10000,
			sphere_constrain = 0x20000,
			random_walk = 0x40000,
		};
		CalculationMethods calculationMethod;
		vector<shared_ptr<GameComponent>> tagged;
		vector<glm::vec3> feelers;

		// From Game Component
		void Update(float timeDelta);
		bool Initialise();

		// Support stuff
		int TagNeighboursSimple(float inRange);
		bool IsOn(behaviour_type behaviour);
		void TurnOn(behaviour_type behaviour);
		void TurnOffAll();

		// Stuff needed to implement the behaviours
		shared_ptr<GameComponent> target;
		shared_ptr<GameComponent> leader;
		float maxSpeed;
		float maxForce;
		glm::vec3 force;
		glm::vec3 acceleration;
		float mass;
		glm::vec3 targetPos;
		glm::vec3 wanderTarget;
		glm::vec3 velocity;
		glm::vec3 randomWalkTarget;
		glm::vec3 offset;
		float timeDelta;

		shared_ptr<Route> route;

		// Behaviours
		glm::vec3 Cohesion();
		glm::vec3 Alignment();
		glm::vec3 SphereConstrain(float radius);
		glm::vec3 Separation();
		glm::vec3 Evade();
		glm::vec3 ObstacleAvoidance();
		glm::vec3 OffsetPursuit(glm::vec3 offset);
		glm::vec3 Pursue();
		glm::vec3 Flee(glm::vec3 targetPos);
		glm::vec3 RandomWalk();
		glm::vec3 Seek(glm::vec3 targetPos);
		glm::vec3 Wander();
		glm::vec3 WallAvoidance();
		glm::vec3 Arrive(glm::vec3 target);
		glm::vec3 FollowPath();

		// Calculation Methods
		glm::vec3 Calculate();
		glm::vec3 CalculateWeightedPrioritised();
		bool AccumulateForce(glm::vec3 & runningTotal, glm::vec3 force);

		int flags;
	};
}

