#include "Lab4.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "LazerBeam.h"

using namespace BGE;

Lab4::Lab4(void)
{
	
}


Lab4::~Lab4(void)
{
}

bool Lab4::Initialise()
{
	std::shared_ptr<GameComponent> ground = make_shared<Ground>();
	Attach(ground);	

	ship1 = make_shared<GameComponent>();
	ship1->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	ship1->Attach(make_shared<VectorDrawer>(glm::vec3(5,5,5)));
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

	Game::Initialise();

	camera->GetController()->position = glm::vec3(0, 4, 20);
	return true;
}

void Lab4::Update(float timeDelta)
{
	// Movement of ship2
	if (keyState[SDL_SCANCODE_UP])
	{
		ship2->position += ship2->look * speed * timeDelta;
	}
	if (keyState[SDL_SCANCODE_LEFT])
	{
		ship2->Yaw(timeDelta * speed * speed);
	}
	Game::Update(timeDelta);
}
