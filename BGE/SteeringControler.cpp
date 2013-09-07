#include "SteeringControler.h"
#include "Game.h"
#include <gtx/norm.hpp>
#include <string>
#include "Utils.h"
#include "Params.h"
#include "Sphere.h"
#include <limits>

using namespace BGE;

bool SteeringController::counted = false;
vector<shared_ptr<GameComponent>> SteeringController::obstacles;
vector<shared_ptr<GameComponent>> SteeringController::steerables;


SteeringController::SteeringController(void)
{
	force = glm::vec3(0);
	acceleration = glm::vec3(0);
	mass = 1.0f;
	timeDelta = 0.0f;
	worldMode = world_modes::to_parent;
	calculationMethod = CalculationMethods::WeightedTruncatedRunningSumWithPrioritisation;
	maxSpeed = Params::GetFloat("max_speed");
	maxForce = Params::GetFloat("max_force");

	wanderTarget = RandomPosition(1.0f);
	wanderTarget = glm::normalize(wanderTarget);
	wanderTarget *= Params::GetFloat("wander_radius");
	counted = false;

	route = make_shared<Route>();

	// Start with all behaviours turned off
	TurnOffAll();
}

bool SteeringController::Initialise()
{
	if (initialised)
	{
		return true;
	}
	Attach(route);
	initialised = true;
	return GameComponent::Initialise();
}


SteeringController::~SteeringController(void)
{
}

void SteeringController::Update(float timeDelta)
{
	float smoothRate;
	this->timeDelta = timeDelta;

	if (! SteeringController::counted)
	{
		list<shared_ptr<GameComponent>>::iterator oit = Game::Instance()->children.begin();
		while (oit != Game::Instance()->children.end())
		{
			if ((* oit)->tag == "Obstacle")
			{
				SteeringController::obstacles.push_back(* oit);
			}
			if ((* oit)->tag == "Steerable")
			{
				SteeringController::steerables.push_back(* oit);
			}
			oit ++;
		}
		SteeringController::counted = true;
	}


	force = Calculate();
	CheckNaN(force);
	glm::vec3 newAcceleration = force / mass;

	if (timeDelta > 0)
	{
		smoothRate = Clip(9 * timeDelta, 0.15f, 0.4f) / 2.0f;
		BlendIntoAccumulator(smoothRate, newAcceleration,  acceleration);
	}

	velocity += acceleration * timeDelta;
	float speed = glm::length(velocity);
	if (speed > maxSpeed)
	{

		velocity = glm::normalize(velocity);
		velocity *= maxSpeed;
	}
	position += velocity * timeDelta;            

	// the length of this global-upward-pointing vector controls the vehicle's
	// tendency to right itself as it is rolled over from turning acceleration
	glm::vec3 globalUp = glm::vec3(0, 0.2f, 0);
	// acceleration points toward the center of local path curvature, the
	// length determines how much the vehicle will roll while turning
	glm::vec3 accelUp = acceleration * 0.05f;
	// combined banking, sum of UP due to turning and global UP
	glm::vec3 bankUp = accelUp + globalUp;
	// blend bankUp into vehicle's UP basis vector
	smoothRate = timeDelta * 3;
	glm::vec3 tempUp = up;
	BlendIntoAccumulator(smoothRate, bankUp, tempUp);
	up = tempUp;
	up = glm::normalize(up);

	if (speed > 0.0001f)
	{
		look = velocity;
		look = glm::normalize(look);
		if (look == right)
		{
			right = GameComponent::basisRight;
		}
		else
		{
			right = glm::cross(look, up);
			glm::normalize(right);

			CheckNaN(right, GameComponent::basisRight);
			up = glm::cross(right, look);
			up = glm::normalize(up);
			CheckNaN(up, GameComponent::basisUp);
		}
		// Apply damping
		velocity *= 0.99f;
	}

	if (look != GameComponent::basisLook)
	{
		// Make a quaternion from the vectors
		glm::mat4 rotationMatrix = glm::lookAt(glm::vec3(0),look,up);
		rotationMatrix = glm::transpose(rotationMatrix);
		orientation = glm::quat(rotationMatrix);
	}

	GameComponent::Update(timeDelta);
}

int SteeringController::TagNeighboursSimple(float inRange)
{
	tagged.clear();
	static float inRange2 = inRange * inRange;
	for (int i = 0 ; i < SteeringController::steerables.size() ; i ++)
	{	
		if (SteeringController::steerables[i].get() != parent) 
		{
			shared_ptr<GameComponent> neighbour =  steerables[i];
			float distance = glm::length2(position - neighbour->position);
			if (distance < inRange2)
			{
				tagged.push_back(neighbour);                
			}			
		}
	}
	return tagged.size();
}

bool SteeringController::IsOn(behaviour_type behaviour)
{
	return ((flags & (int)behaviour) == (int)behaviour);
}

void SteeringController::TurnOn(behaviour_type behaviour)
{
	flags |= ((int)behaviour);
}

void SteeringController::TurnOffAll()
{
	flags = (int) SteeringController::behaviour_type::none;
}

glm::vec3 SteeringController::Cohesion()
{
	glm::vec3 steeringForce = glm::vec3(0);
	glm::vec3 centreOfMass = glm::vec3(0);
	int taggedCount = 0;

	for (int i = 0 ; i < tagged.size(); i ++)
	{
		shared_ptr<GameComponent> entity = tagged[i];
		if (entity.get() != parent)
		{
			centreOfMass += entity->position;
			taggedCount++;
		}
	}
	if (taggedCount > 0)
	{
		centreOfMass /= (float) taggedCount;
		if (glm::length2(centreOfMass) == 0)
		{
			steeringForce = glm::vec3(0);
		}
		else
		{
			steeringForce = glm::normalize(Seek(centreOfMass));
		}
	}
	CheckNaN(steeringForce);
	return steeringForce;
}

glm::vec3 SteeringController::Alignment()
{
	glm::vec3 steeringForce = glm::vec3(0);
	int taggedCount = 0;

	for (int i = 0 ; i < tagged.size(); i ++)
	{
		shared_ptr<GameComponent> entity = tagged[i];
		if (entity.get() != parent)
		{
			steeringForce += entity->look;
			taggedCount++;
		}
	}

	if (taggedCount > 0)
	{
		steeringForce /= (float) taggedCount;
		steeringForce -= look;
	}
	return steeringForce;

}

glm::vec3 SteeringController::SphereConstrain(float radius)
{
	float distance = glm::length(position);
	glm::vec3 steeringForce = glm::vec3(0);
	if (distance > radius)
	{
		steeringForce = glm::normalize(position) * (radius - distance);
	}
	return steeringForce;
}


glm::vec3 SteeringController::Separation()
{
	glm::vec3 steeringForce = glm::vec3(0);

	for (int i = 0 ; i < tagged.size(); i ++)
	{
		shared_ptr<GameComponent> entity = tagged[i];
		if (entity.get() != parent)
		{
			glm::vec3 toEntity = position- entity->position;
			steeringForce += (glm::normalize(toEntity) / glm::length(toEntity));
		}
	}

	return steeringForce;
}

glm::vec3  SteeringController::Evade()
{
    float dist = glm::length(target->position - position);
    float lookAhead = (dist / maxSpeed);

    glm::vec3 targetPos = target->position + (lookAhead * target->velocity);
    return Flee(targetPos);
}


glm::vec3 SteeringController::ObstacleAvoidance()
{
	glm::vec3 force = glm::vec3(0);
    makeFeelers();
	
    vector<shared_ptr<GameComponent>> tagged;
    
	float minBoxLength = 20.0f;
	float boxLength = minBoxLength + ((glm::length(velocity)/maxSpeed) * minBoxLength * 2.0f);
            
    if (glm::isnan(boxLength))
    {
        BGE::LogMessage("NAN");
    }
    // Matt Bucklands Obstacle avoidance
    // First tag obstacles in range
	
	for (int i = 0 ; i < obstacles.size() ; ++i)
	{
		glm::vec3 toCentre = position -  obstacles[i]->position;
		float dist = glm::length(toCentre);
		if (dist < boxLength)
		{
			tagged.push_back(obstacles[i]);
		}
	}
	/*
	vector<shared_ptr<GameComponent>>::iterator it = obstacles.begin();
	while(it != obstacles.end())
	{
		glm::vec3 toCentre = position -  (*it)->position;
        float dist = glm::length(toCentre);
        if (dist < boxLength)
        {
            tagged.push_back(* it);
		}
		++ it;
    }
	*/
    float distToClosestIP = numeric_limits<float>::max();
	shared_ptr<GameComponent> closestIntersectingObstacle = nullptr;
	glm::vec3 localPosOfClosestObstacle = glm::vec3(0);
	glm::vec3 intersection = glm::vec3(0);

    glm::mat4 localTransform = glm::inverse(world);
	for (int i = 0 ; i < tagged.size() ; i ++)
	{
        glm::vec3 localPos = glm::vec3(localTransform * glm::vec4(tagged[i]->position, 1.0f));
        
		// If the local position has a positive Z value then it must lay
		// behind the agent. (in which case it can be ignored)
        if (localPos.z <=0)
		{
			// If the distance from the x axis to the object's position is less
			// than its radius + half the width of the detection box then there
			// is a potential intersection.
			float expandedRadius = scale.z + tagged[i]->scale.z;
			if ((glm::abs(localPos.y) < expandedRadius) && (glm::abs(localPos.x) < expandedRadius))
			{
				// Now to do a ray/sphere intersection test. The center of the				
                SphereGeom tempSphere;
				tempSphere.radius = expandedRadius;
				tempSphere.pos = localPos;				            

				// Create a ray
				RayGeom ray;
				ray.pos = glm::vec3(0, 0, 0);
                ray.look = GameComponent::basisLook;

				// Find the point of intersection
                if (ClosestRayIntersectsSphere(ray, tempSphere, glm::vec3(0), intersection))
				{
					// Now see if its the closest, there may be other intersecting spheres
					float dist = glm::length(intersection);
					if (dist < distToClosestIP)
					{
						dist = distToClosestIP;
						closestIntersectingObstacle = tagged[i];
						localPosOfClosestObstacle = localPos;
					}		
				}
			}	
		}              
		if (closestIntersectingObstacle != nullptr)
		{
			// Now calculate the force
			// Calculate Z Axis braking  force
			float multiplier = 200 * (1.0f + (boxLength - localPosOfClosestObstacle.z) / boxLength);

			//calculate the lateral force
            float expandedRadius = scale.x + closestIntersectingObstacle->scale.x;
			force.x = (expandedRadius - glm::abs(localPosOfClosestObstacle.x))  * multiplier;
            force.y = (expandedRadius - -glm::abs(localPosOfClosestObstacle.y)) * multiplier;

            if (localPosOfClosestObstacle.x > 0)
            {
                force.x = -force.x;
            }
                    
            if (localPosOfClosestObstacle.y > 0)
            {
                force.y = -force.y;
            }                  
            //apply a braking force proportional to the obstacle's distance from
			//the vehicle.
			const float brakingWeight = 40.0f;
            force.z = (closestIntersectingObstacle->scale.x -
                                localPosOfClosestObstacle.z) *
                                brakingWeight;

			//finally, convert the steering vector from local to world space
            force = glm::vec3(world * glm::vec4(force, 1.0f));                    
        }       
    }            
    return force;
}

glm::vec3 SteeringController::OffsetPursuit(glm::vec3 offset)
{
    glm::vec3 target = glm::vec3(0);

    target = glm::vec3(leader->world * glm::vec4(offset, 1));

    float dist = glm::length(target - position);     
      
    float lookAhead = (dist / maxSpeed);

    target = target + (lookAhead * leader->velocity);

    CheckNaN(target);
    return Arrive(target);
}

glm::vec3 SteeringController::Pursue()
{
    float dist = glm::length(target->position - position);

    float lookAhead = (dist / maxSpeed);

    glm::vec3 targetPos = target->position + (lookAhead * target->velocity);
    return Seek(targetPos);
}

glm::vec3 SteeringController::Flee(glm::vec3 targetPos)
{
    float panicDistance = 100.0f;
    glm::vec3 desiredVelocity;
    desiredVelocity = position - targetPos;
    if (glm::length(desiredVelocity)  > panicDistance)
    {
        return glm::vec3(0);
    }

    desiredVelocity = glm::normalize(desiredVelocity);
    desiredVelocity *= maxSpeed;

    return (desiredVelocity - velocity);
}

glm::vec3 SteeringController::RandomWalk()
{
    float dist = glm::length(position - randomWalkTarget);
    if (dist < 50)
    {
        randomWalkTarget.x = RandomClamped() * Params::GetFloat("world_range");
        randomWalkTarget.y = RandomClamped() * Params::GetFloat("world_range");
        randomWalkTarget.z = RandomClamped() * Params::GetFloat("world_range");
    }
    return Seek(randomWalkTarget);
}

glm::vec3 SteeringController::Seek(glm::vec3 targetPos)
{           
    glm::vec3 desiredVelocity;

    desiredVelocity = targetPos - position;
    
	desiredVelocity = glm::normalize(desiredVelocity);
    desiredVelocity *= maxSpeed;

    return (desiredVelocity - velocity);
}


glm::vec3 SteeringController::Wander()
{
    float jitterTimeSlice = Params::GetFloat("wander_jitter") * timeDelta;

    glm::vec3 toAdd = glm::vec3(RandomClamped(), RandomClamped(), RandomClamped()) * jitterTimeSlice;
    wanderTarget += toAdd;
    wanderTarget = glm::normalize(wanderTarget);
    wanderTarget *= Params::GetFloat("wander_radius");

    glm::vec3 worldTarget = glm::vec3(world * glm::vec4(wanderTarget + (GameComponent::basisLook * Params::GetFloat("wander_distance")), 1.0f) );

    return (worldTarget - position);
            
}

glm::vec3 SteeringController::WallAvoidance()
{
    makeFeelers();


    glm::vec3 force = glm::vec3(0);

	/*
	Plane worldPlane = new Plane(new glm::vec3(0, 1, 0), 0);

    foreach (glm::vec3 feeler in fighter.Feelers)
    {
        float dot = worldPlane.DotCoordinate(feeler);
        if (dot < 0)
        {
            float distance = Math.Abs(dot - worldPlane.D);
            force += worldPlane.Normal * distance;
        }           
    }

    if (force.Length() > 0.0)
    {
        fighter.DrawFeelers = true;
    }
    else
    {
        fighter.DrawFeelers = false;
    }
    fighter.DrawAxis = false;
	*/
    return force;
}

void SteeringController::makeFeelers()
{
 	feelers.clear();
    float feelerDistance = 20.0f;
    // Make the forward feeler
    glm::vec3 newFeeler = GameComponent::basisLook * feelerDistance;
    newFeeler = glm::vec3(world * glm::vec4(newFeeler, 1));
    feelers.push_back(newFeeler);

	newFeeler = GameComponent::basisLook * feelerDistance;
	newFeeler = glm::vec3(glm::rotate(glm::mat4(1), 45.0f, GameComponent::basisUp) * glm::vec4(newFeeler, 1));
	newFeeler = glm::vec3(world * glm::vec4(newFeeler, 1));
	feelers.push_back(newFeeler);

	newFeeler = GameComponent::basisLook * feelerDistance;
	newFeeler = glm::vec3(glm::rotate(glm::mat4(1), -45.0f, GameComponent::basisUp) * glm::vec4(newFeeler, 1));
	newFeeler = glm::vec3(world * glm::vec4(newFeeler, 1));
	feelers.push_back(newFeeler);

	newFeeler = GameComponent::basisLook * feelerDistance;
	newFeeler = glm::vec3(glm::rotate(glm::mat4(1), 45.0f, GameComponent::basisRight) * glm::vec4(newFeeler, 1));
	newFeeler = glm::vec3(world * glm::vec4(newFeeler, 1));
	feelers.push_back(newFeeler);

	newFeeler = GameComponent::basisLook * feelerDistance;
	newFeeler = glm::vec3(glm::rotate(glm::mat4(1), -45.0f, GameComponent::basisRight) * glm::vec4(newFeeler, 1));
	newFeeler = glm::vec3(world * glm::vec4(newFeeler, 1));
	feelers.push_back(newFeeler);

}


glm::vec3 SteeringController::Arrive(glm::vec3 target)
{
    glm::vec3 toTarget = target - position;
            
    float slowingDistance = 8.0f;
    float distance = glm::length(toTarget);
    if (distance == 0.0f)
    {
        return glm::vec3(0);
    }
    const float DecelerationTweaker = 10.3f;
    float ramped = maxSpeed * (distance / (slowingDistance * DecelerationTweaker));

    float clamped = glm::min<float>(ramped, maxSpeed);
    glm::vec3 desired = clamped * (toTarget / distance);

    CheckNaN(desired);
          
    return desired - velocity;
}

glm::vec3 SteeringController::Calculate()
{            
    if (calculationMethod == CalculationMethods::WeightedTruncatedRunningSumWithPrioritisation)
    {
        return CalculateWeightedPrioritised();
    }

    return glm::vec3(0);            
}

glm::vec3 SteeringController::CalculateWeightedPrioritised()
{
    glm::vec3 force = glm::vec3(0);
    glm::vec3 steeringForce = glm::vec3(0);
            
    if (IsOn(behaviour_type::obstacle_avoidance))
    {
        force = ObstacleAvoidance() * Params::GetWeight("obstacle_avoidance_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
    CheckNaN(force);
    if (IsOn(behaviour_type::wall_avoidance))
    {
        force = WallAvoidance() * Params::GetWeight("wall_avoidance_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    if (IsOn(behaviour_type::sphere_constrain))
    {
        force = SphereConstrain(Params::GetFloat("world_range")) * Params::GetWeight("sphere_constrain_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    if (IsOn(behaviour_type::evade))
    {
        force = Evade() * Params::GetWeight("evade_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    int tagged = 0;
    if (IsOn(behaviour_type::separation) || IsOn(behaviour_type::cohesion) || IsOn(behaviour_type::alignment))
    {
        //tagged = TagNeighboursSimple(Params::GetFloat("tag_range"));
    }
	tagged = TagNeighboursSimple(Params::GetFloat("tag_range"));
            
    if (IsOn(behaviour_type::separation) && (tagged > 0) )
    {
        force = Separation() * Params::GetWeight("separation_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    if (IsOn(behaviour_type::alignment) && (tagged > 0))
    {
        force = Alignment() * Params::GetWeight("alignment_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    if (IsOn(behaviour_type::cohesion) && (tagged > 0))
    {
        force = Cohesion() * Params::GetWeight("cohesion_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    if (IsOn(behaviour_type::seek))
    {
        force = Seek(targetPos) * Params::GetWeight("seek_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    if (IsOn(behaviour_type::arrive))
    {
        force = Arrive(targetPos) * Params::GetWeight("arrive_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    if (IsOn(behaviour_type::wander))
    {
        force = Wander() * Params::GetWeight("wander_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    if (IsOn(behaviour_type::pursuit))
    {
        force = Pursue() * Params::GetWeight("pursuit_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    if (IsOn(behaviour_type::offset_pursuit))
    {
        force = OffsetPursuit(offset) * Params::GetWeight("offset_pursuit_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    if (IsOn(behaviour_type::follow_path))
    {
        force = FollowPath() * Params::GetWeight("follow_path_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    if (IsOn(behaviour_type::random_walk))
    {
        force = RandomWalk() * Params::GetWeight("random_walk_weight");
        if (!AccumulateForce(steeringForce, force))
        {
            return steeringForce;
        }
    }
            
    return steeringForce;
}

glm::vec3 SteeringController::FollowPath()
{
    float epsilon = 5.0f;
    float dist = glm::length(position - route->NextWaypoint());
    if (dist < epsilon)
    {
        route->AdvanceToNext();
    }
    if ((! route->looped) && route->IsLast())
    {
        return Arrive(route->NextWaypoint());
    }
    else
    {
        return Seek(route->NextWaypoint());
    }
}

bool SteeringController::AccumulateForce(glm::vec3 & runningTotal, glm::vec3 force)
{
    float soFar = glm::length(runningTotal);

    float remaining = maxForce - soFar;
    if (remaining <= 0)
    {
        return false;
    }

    float toAdd = glm::length(force);
           

    if (toAdd < remaining)
    {
        runningTotal += force;
    }
    else
    {
        runningTotal += glm::normalize(force) * remaining;
    }
    return true;
}