#include "PhysicsFactory.h"
#include "Game.h"
#include "Sphere.h"
#include "Box.h"
#include "Cylinder.h"
#include "CapsuleShape.h"
#include "Ground.h"
#include "Content.h"
#include "PhysicsCamera.h"
#include "Model.h"
#include "dirent.h"
#include "Utils.h"
using namespace BGE;

PhysicsFactory::PhysicsFactory(btDiscreteDynamicsWorld * dynamicsWorld)
{
	this->dynamicsWorld = dynamicsWorld;
}


PhysicsFactory::~PhysicsFactory(void)
{
}

void PhysicsFactory::CreateWall(glm::vec3 startAt, float width, float height, float blockWidth, float blockHeight, float blockDepth)
{
	float z = startAt.z;
	float gap = 1;

	for (int w = 0 ; w < width ; w ++)
	{
		for (int h = 0 ; h < height ; h ++)	
		{
			float x = startAt.x + ((blockWidth + 2) * w);
			float y = ((blockHeight + gap) / 2.0f) + ((blockHeight + gap) * h);
			CreateBox(blockWidth, blockHeight, blockDepth, glm::vec3(x, y, z), glm::quat());
		}
	}
}

shared_ptr<PhysicsController> PhysicsFactory::CreateFromModel(string name, glm::vec3 pos, glm::quat quat, glm::vec3 scale)
{
	shared_ptr<GameComponent> component = make_shared<GameComponent>();
	component->tag = name;
	component->scale = scale;
	Game::Instance()->Attach(component);
	shared_ptr<Model> model = Content::LoadModel(name);
	component->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	model->Initialise();
	component->Attach(model);

	std::vector<glm::vec3>::iterator it = model->vertices.begin(); 	
	btConvexHullShape * tetraShape = new btConvexHullShape();

	while (it != model->vertices.end())
	{
		glm::vec4 point = glm::vec4(* it, 0) * glm::scale(glm::mat4(1), scale);
		tetraShape->addPoint(GLToBtVector(glm::vec3(point)));
		it ++;
	}
	
	btScalar mass = 1;
	btVector3 inertia(0,0,0);
	
	tetraShape->calculateLocalInertia(mass,inertia);
	btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(GLToBtQuat(quat)
		,GLToBtVector(pos)));	
	
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,motionState, tetraShape, inertia);
	btRigidBody * body = new btRigidBody(rigidBodyCI);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	dynamicsWorld->addRigidBody(body);

	shared_ptr<PhysicsController> controller =make_shared<PhysicsController>(tetraShape, body, motionState);	
	body->setUserPointer(controller.get());
	component->Attach(controller);
	
	controller->tag = "Model";	
	return controller;
}

shared_ptr<PhysicsController> PhysicsFactory::CreateSphere(float radius, glm::vec3 pos, glm::quat quat)
{
	shared_ptr<GameComponent> sphere (new Sphere(radius));
	Game::Instance()->Attach(sphere);

	btDefaultMotionState * sphereMotionState = new btDefaultMotionState(btTransform(GLToBtQuat(quat)
		,GLToBtVector(pos)));	

	btScalar mass = 1;
	btVector3 sphereInertia(0,0,0);
	btCollisionShape * sphereShape = new btSphereShape(radius);

	sphereShape->calculateLocalInertia(mass,sphereInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,sphereMotionState, sphereShape, sphereInertia);
	btRigidBody * body = new btRigidBody(fallRigidBodyCI);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	dynamicsWorld->addRigidBody(body);

	shared_ptr<PhysicsController> sphereController (new PhysicsController(sphereShape, body, sphereMotionState));	
	body->setUserPointer(sphereController.get());
	sphere->Attach(sphereController);
	sphereController->tag = "Sphere";	
	return sphereController;
}

shared_ptr<PhysicsController> PhysicsFactory::CreateBox(float width, float height, float depth, glm::vec3 pos, glm::quat quat)
{
	// Create the shape
	btCollisionShape * boxShape = new btBoxShape(btVector3(width, height, depth) * 0.50);
	btScalar mass = 1;
	btVector3 boxInertia(0,0,0);
	boxShape->calculateLocalInertia(mass,boxInertia);

	// This is a container for the box model
	shared_ptr<Box> box = make_shared<Box>(width, height, depth);
	box->worldMode = GameComponent::from_child;
	box->position = pos;
	Game::Instance()->Attach(box);

	// Create the rigid body
	btDefaultMotionState * boxMotionState = new btDefaultMotionState(btTransform(GLToBtQuat(quat)
		,GLToBtVector(pos)));			
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,  boxMotionState, boxShape, boxInertia);
	btRigidBody * body = new btRigidBody(fallRigidBodyCI);
	body->setFriction(567);
	dynamicsWorld->addRigidBody(body);

	// Create the physics component and add it to the box
	shared_ptr<PhysicsController> boxController = make_shared<PhysicsController>(PhysicsController(boxShape, body, boxMotionState));
	boxController->tag = "Box";
	body->setUserPointer(boxController.get());
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	box->Attach(boxController);

	return boxController;
}

shared_ptr<PhysicsController> PhysicsFactory::CreateCylinder(float radius, float height, glm::vec3 pos, glm::quat quat)
{
	// Create the shape
	btCollisionShape * shape = new btCylinderShape(btVector3(radius, height * 0.5f, radius));
	btScalar mass = 1;
	btVector3 inertia(0,0,0);
	shape->calculateLocalInertia(mass,inertia);

	// This is a container for the box model
	shared_ptr<GameComponent> cyl = make_shared<GameComponent>(Cylinder(radius, height));
	cyl->position = pos;
	Game::Instance()->Attach(cyl);

	// Create the rigid body
	btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(GLToBtQuat(quat)
		,GLToBtVector(pos)));			
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,  motionState, shape, inertia);
	btRigidBody * body = new btRigidBody(rigidBodyCI);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	dynamicsWorld->addRigidBody(body);

	// Create the physics component and add it to the box
	shared_ptr<PhysicsController> component = make_shared<PhysicsController>(PhysicsController(shape, body, motionState));
	body->setUserPointer(component.get());
	component->tag = "Cylinder";
	cyl->Attach(component);

	return component;
}

shared_ptr<PhysicsController> PhysicsFactory::CreateCameraPhysics()
{
	btVector3 inertia;
	// Now add physics to the camera
	btCollisionShape * cameraCyl = new btCylinderShape(btVector3(0.5f, 5.0f, 2.5f));
	cameraCyl->calculateLocalInertia(1, inertia);
	shared_ptr<PhysicsCamera> physicsCamera = make_shared<PhysicsCamera>(this);

	shared_ptr<Camera> camera = Game::Instance()->camera;
	camera->Attach(physicsCamera);

	btRigidBody::btRigidBodyConstructionInfo cameraCI(10,physicsCamera.get(), cameraCyl, inertia);  
	btRigidBody * body = new btRigidBody(cameraCI);
	physicsCamera->SetPhysicsStuff(cameraCyl, body, physicsCamera.get());
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	body->setActivationState(DISABLE_DEACTIVATION);

	dynamicsWorld->addRigidBody(body);
	return physicsCamera;
}

shared_ptr<PhysicsController> PhysicsFactory::CreateVehicle(glm::vec3 position)
{
	float width = 15;
	float height = 2;
	float length = 5;
	float wheelWidth = 1;
	float wheelRadius = 2;
	float wheelOffset = 2.0f;

	shared_ptr<PhysicsController> chassis = CreateBox(width, height, length, position, glm::quat());

	shared_ptr<PhysicsController> wheel;
	glm::quat q =  glm::angleAxis(glm::half_pi<float>(), glm::vec3(1, 0, 0));

	glm::vec3 offset;
	btHingeConstraint * hinge;

	offset = glm::vec3(- (width / 2 - wheelRadius), 0, - (length / 2 + wheelOffset));
	wheel = CreateCylinder(wheelRadius, wheelWidth, position + offset, q);	 
	hinge = new btHingeConstraint(* chassis->rigidBody, * wheel->rigidBody, GLToBtVector(offset),btVector3(0,0, 0), btVector3(0,0,1), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);

	offset = glm::vec3(+ (width / 2 - wheelRadius), 0, - (length / 2 + wheelOffset));
	wheel = CreateCylinder(wheelRadius, wheelWidth, glm::vec3(position.x + (width / 2) - wheelRadius, position.y, position.z - (length / 2) - wheelWidth), q);
	hinge = new btHingeConstraint(* chassis->rigidBody, * wheel->rigidBody, GLToBtVector(offset),btVector3(0,0, 0), btVector3(0,0,1), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);

	offset = glm::vec3(- (width / 2 - wheelRadius), 0, + (length / 2 + wheelOffset));
	wheel = CreateCylinder(wheelRadius, wheelWidth, position + offset, q);	 
	hinge = new btHingeConstraint(* chassis->rigidBody, * wheel->rigidBody, GLToBtVector(offset),btVector3(0,0, 0), btVector3(0,0,1), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);

	offset = glm::vec3(+ (width / 2 - wheelRadius), 0, + (length / 2 + wheelOffset));
	wheel = CreateCylinder(wheelRadius, wheelWidth, position + offset, q);	 
	hinge = new btHingeConstraint(* chassis->rigidBody, * wheel->rigidBody, GLToBtVector(offset),btVector3(0,0, 0), btVector3(0,0,1), btVector3(0,1,0), true);
	dynamicsWorld->addConstraint(hinge);

	return chassis;
}

shared_ptr<PhysicsController> PhysicsFactory::CreateGroundPhysics()
{
	shared_ptr<Ground> ground = make_shared<Ground>();

	btCollisionShape * groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
	btDefaultMotionState * groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));

	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
	btRigidBody* body = new btRigidBody(groundRigidBodyCI);
	body->setFriction(100);
	dynamicsWorld->addRigidBody(body);
	body->setUserPointer(ground.get());
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	shared_ptr<PhysicsController> groundComponent (new PhysicsController(groundShape, body, groundMotionState));
	groundComponent->tag = "Ground";
	ground->Attach(groundComponent);	
	Game::Instance()->SetGround(ground);
	return groundComponent;
}

shared_ptr<PhysicsController> PhysicsFactory::CreateRandomObject(glm::vec3 point, glm::quat q)
{
	vector<string> names;
	DIR * dir;
	struct dirent * ent;
	dir = opendir (Content::prefix.c_str());
	if (dir != NULL) 
	{
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) 
		{
			string fname = string(ent->d_name);
			int fpos = fname.find("objm");
			if (fpos != string::npos)
			{
				if ((fname.find("cube") == string::npos) && (fname.find("cyl") == string::npos) && (fname.find("sphere") == string::npos))
				{
					names.push_back(fname.substr(0, fpos - 1));
				}
			}
		}
		closedir (dir);
	} 
	else 
	{
		throw BGE::Exception("Could not list obj files in content folder");
	}

	int which = rand() % names.size();
	string name = names[which];
	return CreateFromModel(name, point, q, glm::vec3(3,3,3));
}

shared_ptr<PhysicsController> PhysicsFactory::CreateCapsuleShape(float radius, float height, glm::vec3 pos, glm::quat quat)
{
	btCollisionShape* capsuleShape = new btCapsuleShape(btScalar(radius), btScalar(height));
	btScalar mass = 1;
	btVector3 inertia(0,0,0);
	capsuleShape->calculateLocalInertia(mass,inertia);

	// This is a container for the box model
	shared_ptr<GameComponent> cap = make_shared<GameComponent>(CapsuleShape(radius, height));
	cap->position = pos;
	Game::Instance()->Attach(cap);

	// Create the rigid body
	btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(GLToBtQuat(quat)
		,GLToBtVector(pos)));			
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,  motionState, capsuleShape, inertia);
	btRigidBody * body = new btRigidBody(rigidBodyCI);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	dynamicsWorld->addRigidBody(body);

	// Create the physics component and add it to the box
	shared_ptr<PhysicsController> component = make_shared<PhysicsController>(PhysicsController(capsuleShape, body, motionState));
	body->setUserPointer(component.get());
	component->tag = "Capluse";
	cap->Attach(component);

	return component;
}

/*
*example how to use each joins
*			
*physicsFactory->right_upper_leg_right_lower_leg->setLimit(btScalar(num), btScalar(num));
*physicsFactory->right_upper_leg_right_lower_leg->enableAngularMotor(bool, targetVelocity, maxMotorImpulse);
*/
shared_ptr<PhysicsController> PhysicsFactory::CreateCapsuleRagdoll(glm::vec3 position)
{

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4     0.785398163397448309616
#endif

	shared_ptr<PhysicsController> bodypart_head  = CreateCapsuleShape(1.0, 0.8, glm::vec3(position.x, position.y+20, position.z), glm::quat());
	shared_ptr<PhysicsController> bodypart_spine  = CreateCapsuleShape(1.8, 1.4, glm::vec3(position.x, position.y+15, position.z), glm::quat());
	shared_ptr<PhysicsController> bodypart_pelvis  = CreateCapsuleShape(1.8, 0.8, glm::vec3(position.x, position.y+10, position.z), glm::quat());

	shared_ptr<PhysicsController> bodypart_left_upper_leg  = CreateCapsuleShape(0.8, 1.4, glm::vec3(position.x-1.5, position.y+6, position.z), glm::quat());
	shared_ptr<PhysicsController> bodypart_left_lower_leg  = CreateCapsuleShape(0.6, 1.4, glm::vec3(position.x-1.5, position.y, position.z), glm::quat());
	shared_ptr<PhysicsController> bodypart_right_upper_leg  = CreateCapsuleShape(0.8, 1.4, glm::vec3(position.x+1.5, position.y+6, position.z), glm::quat());
	shared_ptr<PhysicsController> bodypart_right_lower_leg  = CreateCapsuleShape(0.6, 1.4, glm::vec3(position.x+1.5, position.y, position.z), glm::quat());

	shared_ptr<PhysicsController> bodypart_left_upper_arm  = CreateCapsuleShape(0.6, 1.2, glm::vec3(position.x-3, position.y+14, position.z), glm::quat());
	shared_ptr<PhysicsController> bodypart_left_lower_arm  = CreateCapsuleShape(0.5, 1.2, glm::vec3(position.x-3, position.y+9, position.z), glm::quat());
	shared_ptr<PhysicsController> bodypart_right_upper_arm  = CreateCapsuleShape(0.6, 1.2, glm::vec3(position.x+3, position.y+14, position.z), glm::quat());
	shared_ptr<PhysicsController> bodypart_right_lower_arm  = CreateCapsuleShape(0.5, 1.2, glm::vec3(position.x+3, position.y+9, position.z), glm::quat());

	btTransform localA, localB;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(2.7), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-2.7), btScalar(0.)));
	spine_pelvis =  new btHingeConstraint(*bodypart_pelvis->rigidBody, *bodypart_spine->rigidBody, localA, localB);
	spine_pelvis->setLimit(btScalar(-M_PI_2), btScalar(M_PI_2));
	dynamicsWorld->addConstraint(spine_pelvis);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,M_PI_2); localA.setOrigin(btVector3(btScalar(0.), btScalar(2.7), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-2.7), btScalar(0.)));
	head_spine = new btConeTwistConstraint(*bodypart_spine->rigidBody, *bodypart_head->rigidBody, localA, localB);
	head_spine->setLimit(M_PI_4, M_PI_4, M_PI_2);
	dynamicsWorld->addConstraint(head_spine);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,-M_PI_4*5); localA.setOrigin(btVector3(btScalar(-1.5), btScalar(-2.5), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,-M_PI_4*5); localB.setOrigin(btVector3(btScalar(0.), btScalar(2.5), btScalar(0.)));
	pelvis_left_upper_leg = new btConeTwistConstraint(*bodypart_pelvis->rigidBody, *bodypart_left_upper_leg->rigidBody, localA, localB);
	pelvis_left_upper_leg->setLimit(M_PI_4, M_PI_2, 0);
	dynamicsWorld->addConstraint(pelvis_left_upper_leg);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(-2.7), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(2.7), btScalar(0.)));
	left_upper_leg_left_lower_leg =  new btHingeConstraint(*bodypart_left_upper_leg->rigidBody, *bodypart_left_lower_leg->rigidBody, localA, localB);
	left_upper_leg_left_lower_leg->setLimit(btScalar(-M_PI_2), btScalar(0));
	dynamicsWorld->addConstraint(left_upper_leg_left_lower_leg);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,M_PI_4); localA.setOrigin(btVector3(btScalar(1.5), btScalar(-2.5), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_4); localB.setOrigin(btVector3(btScalar(0.), btScalar(2.5), btScalar(0.)));
	pelvis_right_upper_leg = new btConeTwistConstraint(*bodypart_pelvis->rigidBody, *bodypart_right_upper_leg->rigidBody, localA, localB);
	pelvis_right_upper_leg->setLimit(M_PI_4, M_PI_2, 0);
	dynamicsWorld->addConstraint(pelvis_right_upper_leg);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(-2.7), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(2.7), btScalar(0.)));
	right_upper_leg_right_lower_leg =  new btHingeConstraint(*bodypart_right_upper_leg->rigidBody, *bodypart_right_lower_leg->rigidBody, localA, localB);
	right_upper_leg_right_lower_leg->setLimit(btScalar(-M_PI_2), btScalar(0));
	dynamicsWorld->addConstraint(right_upper_leg_right_lower_leg);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,M_PI); localA.setOrigin(btVector3(btScalar(-2.5), btScalar(2.5), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-2.5), btScalar(0.)));
	spine_left_upper_arm = new btConeTwistConstraint(*bodypart_spine->rigidBody, *bodypart_left_upper_arm->rigidBody, localA, localB);
	spine_left_upper_arm->setLimit(M_PI, M_PI, 0);
	dynamicsWorld->addConstraint(spine_left_upper_arm);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(2.5), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-2.5), btScalar(0.)));
	left_upper_arm_left_lower_arm =  new btHingeConstraint(*bodypart_left_upper_arm->rigidBody, *bodypart_left_lower_arm->rigidBody, localA, localB);
	left_upper_arm_left_lower_arm->setLimit(btScalar(-M_PI_2), btScalar(0));
	dynamicsWorld->addConstraint(left_upper_arm_left_lower_arm);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,0); localA.setOrigin(btVector3(btScalar(2.5), btScalar(2.5), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-2.5), btScalar(0.)));
	spine_right_upper_arm = new btConeTwistConstraint(*bodypart_spine->rigidBody, *bodypart_right_upper_arm->rigidBody, localA, localB);
	spine_right_upper_arm->setLimit(M_PI, M_PI, 0);
	dynamicsWorld->addConstraint(spine_right_upper_arm);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(2.5), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-2.5), btScalar(0.)));
	right_upper_arm_right_lower_arm =  new btHingeConstraint(*bodypart_right_upper_arm->rigidBody, *bodypart_right_lower_arm->rigidBody, localA, localB);
	right_upper_arm_right_lower_arm->setLimit(btScalar(-M_PI_2), btScalar(0));
	dynamicsWorld->addConstraint(right_upper_arm_right_lower_arm);

	return bodypart_spine;
}