#include "RiftController.h"
#include "Game.h"

using namespace BGE;
using namespace OVR;
using namespace std;

RiftController::RiftController(void)
{
	worldMode = world_modes::to_parent;
}


RiftController::~RiftController(void)
{
}

glm::quat RiftController::OVRToGLQuat(Quatf q)
{
	return glm::quat(q.w, q.x, q.y, q.z);
}

void RiftController::Update(float timeDelta)
{
	Game * game = Game::Instance();
	//if (game->pSensor)
 //   {        
 //       Quatf    hmdOrient = game->SFusion.GetOrientation();
 //       float    yaw = 0.0f;
	//	orientation = OVRToGLQuat(hmdOrient);

	//	stringstream ss;
	//	ss << "Rift orientation: " << orientation.w << " " << orientation.x << " "  << orientation.y << " " << orientation.z;
	//	game->PrintText(ss.str());
 //       //hmdOrient.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &EyePitch, &EyeRoll);

 //       //EyeYaw += (yaw - LastSensorYaw);
 //       //LastSensorYaw = yaw;    
 //   }    
	GameComponent::Update(timeDelta);
}
