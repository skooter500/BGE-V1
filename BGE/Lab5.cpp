#include "Lab5.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "LazerBeam.h"
#include "FountainEffect.h"

using namespace BGE;

Lab5::Lab5(void)
{
	elapsed = 10000;
}


Lab5::~Lab5(void)
{
}

bool Lab5::Initialise()
{
	std::shared_ptr<GameComponent> ground = make_shared<Ground>();
	Attach(ground);	

	ship1 = make_shared<GameComponent>();
	ship1->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	ship1->position = glm::vec3(-10, 2, -10);
	Attach(ship1);

	ship2 = make_shared<GameComponent>();
	ship2->Attach(Content::LoadModel("python", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	ship2->Attach(make_shared<VectorDrawer>(glm::vec3(5,5,5)));
	ship2->position = glm::vec3(10, 2, -10);
	Attach(ship2);


	riftEnabled = false;
	fullscreen = false;
	width = 800;
	height = 600;

	// 500 in the constructor indicates the number of particles in the effect. 
	// You may need to compile in release mode or reduce the number of particles to get an acceptable framerate
	shared_ptr<FountainEffect> centFountain = make_shared<FountainEffect>(500);
	centFountain->position.x = centFountain->position.y = 0;
	centFountain->position.y = FOUNTAIN_HEIGHT;
	centFountain->diffuse = glm::vec3(1,1,0);
	Attach(centFountain);

	// make a circle of fountains		
	fountainTheta = 0.0f; 
	for (int i = 0 ; i < NUM_FOUNTAINS ; i ++)
	{
		// PART 2 of the lab
		// Do something in here to create the circle of fountains.
		// Check the header file for the following defines you can use:
		// NUM_FOUNTAINS - The total number of fountains to create
		// FOUNTAIN_RADIUS - The radius around which to place the fountains. 
		// FOUNTAIN_HEIGHT - The initial Y value for the fountains
		// When you create the fountain attach it to the scene
		// And also add it to the fountains vector (member of the class)
		// Se we can use it later in the Update
		// You can set the colour using the "diffuse" member		
		// Hint! Consider how we can use the Unit Circle to calculate the X and Y positions on circle
	}
	fountainTheta = 0.0f;
	Game::Initialise();

	camera->GetController()->position = glm::vec3(0, 4, 20);
	return true;
}

void Lab5::Update(float timeDelta)
{	
	// Movement of ship2
	if (keyState[SDL_SCANCODE_UP])
	{
		ship2->position += ship2->look * speed * timeDelta;
	}
	if (keyState[SDL_SCANCODE_DOWN])
	{
		ship2->position -= ship2->look * speed * timeDelta;
	}
	if (keyState[SDL_SCANCODE_LEFT])
	{
		ship2->Yaw(timeDelta * speed * speed);
	}
	if (keyState[SDL_SCANCODE_RIGHT])
	{
		ship2->Yaw(-timeDelta * speed * speed);
	}
	
	//Part 2
	for (int i = 0 ; i < fountains.size() ; i ++)
	{		
		// 	Update the fountain Y in here
	}
	fountainTheta += timeDelta;
	// Put code here to check to see if fountainTheta goes above 2 PI and if so reset to 0
	// End Part 2

	Game::Update(timeDelta);


	// Part 1
	float theta = 0.0f;
	// Calculate theta
	// Make a rotation matrix, include it in the world transform of ship 1
	ship1->world = glm::translate(glm::mat4(1), ship1->position);
	// End Part 1
}
