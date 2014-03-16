# TODO Fix this makefile (it should be possible to make it shorter without a loss of functionality) 
# Although, it does work perfectly, so it's fine for the time being... 

CC=g++
OPTS=-fpermissive -w -std=gnu++0x
START=$(CC) $(OPTS)
DEPS=../Dependancies
LDEPS=$(DEPS)/linux
OBJ=-c -pg -o
PRIMARY=$(START) $(OBJ)

# Include valid header files
IGLM=-I$(DEPS)/glm-0.9.4.3/glm			# Include GLM header files
ISTLTTF=-I$(LDEPS)/SDL2_ttf-2.0.12		# Include STL_ttf header files
ISTL=-I$(LDEPS)/SDL-2.0.2-8236/include		# Include STL header files
IFMOD=-I$(LDEPS)/fmodapi44431linux/api/inc	# Include FMOD header files
IBUL=-I$(LDEPS)/bullet-2.81-rev2613/src		# Include Bullet Physics header files

ALLDEPS=$(IGLM) $(ISTLTTF) $(ISTL) $(IFMOD) $(IBUL)

LSTLTTF=-L$(LDEPS)/SDL2_ttf-2.0.12/.libs
LSTL=-L$(LDEPS)/SDL-2.0.2-8236/build/.libs
LFMOD=-L$(LDEPS)/fmodapi44431linux/api/lib
LBUL=-L$(LDEPS)/bullet-2.81-rev2613/src/.libs

LLSTLTTF=-lSDL2_ttf
LLSTL=-lSDL2
LLFMOD=-lfmodex64
LLBUL=-lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath

# File groupings
PHYSICSCAMERA=physicscamera_cpp
PHYSICSCONTROLLER=physicscontroller_cpp
PHYSICSGAME1=physicsgame1_cpp
TRANSFORM=transform_cpp
CONTENT=content_cpp
STATE=state_cpp 
CAMERA=camera_cpp
BUDDHA=buddha_cpp 
MOTIONSTATE=motionstate_cpp 
PARTICLEEFFECT=particleeffect_cpp
CUBETEST=cubetest_cpp
IDLESTATE=idlestate_cpp
SCENARIO=scenario_cpp
VECTORDRAWER=vectordrawer_cpp
SPHERE=sphere_cpp
FPSCONTROLLER=fpscontroller_cpp 
PHYSICSFACTORY=physicsfactory_cpp 
SNOWEFFECT=snoweffect_cpp 
PATHFOLLOWINGSCENARIO=pathfollowingscenario_cpp
GAME=game_cpp
STEERABLE3DCONTROLLER=steerable3dcontroller_cpp 
XBOXCONTROLLER=xboxcontroller_cpp
MAIN=main_cpp
SCENEGRAPHGAME=scenegraphgame_cpp
BOX=box_cpp 
FOUNTAINEFFECT=fountaineffect_cpp 
EXCEPTION=exception_cpp
TEXTUREDCUBE=texturedcube_cpp 
PARTICLE=particle_cpp 
ROUTE=route_cpp
LAB4=lab4_cpp
FLOCKINGSCENARIO=flockingscenario_cpp
ATTACKINGSTATE=attackingstate_cpp
STEERINGGAME=steeringgame_cpp
LAZERBEAM=lazerbeam_cpp 
KINEMATICMOTIONSTATE=kinematicmotionstate_cpp 
MODEL=model_cpp
TRIANGLETEST=triangletest_cpp
PHYSICSDRAWER=physicsdrawer_cpp
CYLINDER=cylinder_cpp
OBSTACLEAVOIDANCESCENARIO=obstacleavoidancescenario_cpp
GAMECOMPONENT=gamecomponent_cpp 
MODELTEST=modeltest_cpp 
STEERINGCONTROLER=steeringcontroler_cpp
UTILS=utils_cpp
LINEDRAWER=linedrawer_cpp 
SOUNDSYSTEM=soundsystem_cpp
LAB7=lab7_cpp
LAB6=lab6_cpp
LAB5=lab5_cpp 
SHADERFUNCTIONS=shaderfunctions_cpp 
PARAMS=params_cpp
LAB8=lab8_cpp
FBO=fbo_cpp 
GROUND=ground_cpp 

program: compile_0 compile_1 compile_2 compile_3 compile_4 compile_5 compile_6 compile_7 compile_8 compile_9 compile_10 compile_11 
	$(CC) -fpermissive -std=gnu++0x -pg -o bgedemo $(IGLM) *.o $(LSTLTTF) $(LSTL) $(LFMOD) $(LBUL) $(LLSTLTTF) $(LLSTL) $(LLFMOD) $(LLBUL) -lGL -lGLU -lGLEW 

compile_0: $(PHYSICSCAMERA) $(PHYSICSCONTROLLER) $(PHYSICSGAME1) $(TRANSFORM) $(CONTENT) 
compile_1: $(STATE) $(CAMERA) $(BUDDHA) $(MOTIONSTATE) $(PARTICLEEFFECT) 
compile_2: $(CUBETEST) $(IDLESTATE) $(SCENARIO) $(VECTORDRAWER) $(SPHERE) 
compile_3: $(FPSCONTROLLER) $(PHYSICSFACTORY) $(SNOWEFFECT) $(PATHFOLLOWINGSCENARIO) $(GAME) 
compile_4: $(STEERABLE3DCONTROLLER) $(XBOXCONTROLLER) $(MAIN) $(SCENEGRAPHGAME) $(BOX) 
compile_5: $(FOUNTAINEFFECT) $(EXCEPTION) $(TEXTUREDCUBE) $(PARTICLE) $(ROUTE) 
compile_6: $(LAB4) $(FLOCKINGSCENARIO) $(ATTACKINGSTATE) $(STEERINGGAME) $(LAZERBEAM) 
compile_7: $(KINEMATICMOTIONSTATE) $(MODEL) $(TRIANGLETEST) $(PHYSICSDRAWER) $(CYLINDER) 
compile_8: $(OBSTACLEAVOIDANCESCENARIO) $(GAMECOMPONENT) $(MODELTEST) $(STEERINGCONTROLER) $(UTILS) 
compile_9: $(LINEDRAWER) $(SOUNDSYSTEM) $(OVR_SHADERS) $(LAB7) $(LAB6) 
compile_10: $(LAB5) $(SHADERFUNCTIONS) $(PARAMS) $(LAB8) $(FBO) 
compile_11: $(GROUND) 

physicscamera_cpp:
	$(PRIMARY) physicscamera_cpp.o $(ALLDEPS) ../BGE/PhysicsCamera.cpp

physicscontroller_cpp:
	$(PRIMARY) physicscontroller_cpp.o $(ALLDEPS) ../BGE/PhysicsController.cpp

physicsgame1_cpp:
	$(PRIMARY) physicsgame1_cpp.o $(ALLDEPS) ../BGE/PhysicsGame1.cpp

transform_cpp:
	$(PRIMARY) transform_cpp.o $(ALLDEPS) ../BGE/Transform.cpp

content_cpp:
	$(PRIMARY) content_cpp.o $(ALLDEPS) ../BGE/Content.cpp

state_cpp:
	$(PRIMARY) state_cpp.o $(ALLDEPS) ../BGE/State.cpp

camera_cpp:
	$(PRIMARY) camera_cpp.o $(ALLDEPS) ../BGE/Camera.cpp

buddha_cpp:
	$(PRIMARY) buddha_cpp.o $(ALLDEPS) ../BGE/Buddha.cpp

motionstate_cpp:
	$(PRIMARY) motionstate_cpp.o $(ALLDEPS) ../BGE/MotionState.cpp

particleeffect_cpp:
	$(PRIMARY) particleeffect_cpp.o $(ALLDEPS) ../BGE/ParticleEffect.cpp

cubetest_cpp:
	$(PRIMARY) cubetest_cpp.o $(ALLDEPS) ../BGE/CubeTest.cpp

idlestate_cpp:
	$(PRIMARY) idlestate_cpp.o $(ALLDEPS) ../BGE/IdleState.cpp

scenario_cpp:
	$(PRIMARY) scenario_cpp.o $(ALLDEPS) ../BGE/Scenario.cpp

vectordrawer_cpp:
	$(PRIMARY) vectordrawer_cpp.o $(ALLDEPS) ../BGE/VectorDrawer.cpp

sphere_cpp:
	$(PRIMARY) sphere_cpp.o $(ALLDEPS) ../BGE/Sphere.cpp

fpscontroller_cpp:
	$(PRIMARY) fpscontroller_cpp.o $(ALLDEPS) ../BGE/FPSController.cpp

physicsfactory_cpp:
	$(PRIMARY) physicsfactory_cpp.o $(ALLDEPS) ../BGE/PhysicsFactory.cpp

snoweffect_cpp:
	$(PRIMARY) snoweffect_cpp.o $(ALLDEPS) ../BGE/SnowEffect.cpp

pathfollowingscenario_cpp:
	$(PRIMARY) pathfollowingscenario_cpp.o $(ALLDEPS) ../BGE/PathFollowingScenario.cpp

game_cpp:
	$(PRIMARY) game_cpp.o $(ALLDEPS) ../BGE/Game.cpp

steerable3dcontroller_cpp:
	$(PRIMARY) steerable3dcontroller_cpp.o $(ALLDEPS) ../BGE/Steerable3DController.cpp

xboxcontroller_cpp:
	$(PRIMARY) xboxcontroller_cpp.o $(ALLDEPS) ../BGE/XBoxController.cpp

main_cpp:
	$(PRIMARY) main_cpp.o $(ALLDEPS) ../BGE/main.cpp

scenegraphgame_cpp:
	$(PRIMARY) scenegraphgame_cpp.o $(ALLDEPS) ../BGE/SceneGraphGame.cpp

box_cpp:
	$(PRIMARY) box_cpp.o $(ALLDEPS) ../BGE/Box.cpp

fountaineffect_cpp:
	$(PRIMARY) fountaineffect_cpp.o $(ALLDEPS) ../BGE/FountainEffect.cpp

exception_cpp:
	$(PRIMARY) exception_cpp.o $(ALLDEPS) ../BGE/Exception.cpp

texturedcube_cpp:
	$(PRIMARY) texturedcube_cpp.o $(ALLDEPS) ../BGE/TexturedCube.cpp

particle_cpp:
	$(PRIMARY) particle_cpp.o $(ALLDEPS) ../BGE/Particle.cpp

route_cpp:
	$(PRIMARY) route_cpp.o $(ALLDEPS) ../BGE/Route.cpp

lab4_cpp:
	$(PRIMARY) lab4_cpp.o $(ALLDEPS) ../BGE/Lab4.cpp

flockingscenario_cpp:
	$(PRIMARY) flockingscenario_cpp.o $(ALLDEPS) ../BGE/FlockingScenario.cpp

attackingstate_cpp:
	$(PRIMARY) attackingstate_cpp.o $(ALLDEPS) ../BGE/AttackingState.cpp

steeringgame_cpp:
	$(PRIMARY) steeringgame_cpp.o $(ALLDEPS) ../BGE/SteeringGame.cpp

lazerbeam_cpp:
	$(PRIMARY) lazerbeam_cpp.o $(ALLDEPS) ../BGE/LazerBeam.cpp

kinematicmotionstate_cpp:
	$(PRIMARY) kinematicmotionstate_cpp.o $(ALLDEPS) ../BGE/KinematicMotionState.cpp

model_cpp:
	$(PRIMARY) model_cpp.o $(ALLDEPS) ../BGE/Model.cpp

triangletest_cpp:
	$(PRIMARY) triangletest_cpp.o $(ALLDEPS) ../BGE/TriangleTest.cpp

physicsdrawer_cpp:
	$(PRIMARY) physicsdrawer_cpp.o $(ALLDEPS) ../BGE/PhysicsDrawer.cpp

cylinder_cpp:
	$(PRIMARY) cylinder_cpp.o $(ALLDEPS) ../BGE/Cylinder.cpp

obstacleavoidancescenario_cpp:
	$(PRIMARY) obstacleavoidancescenario_cpp.o $(ALLDEPS) ../BGE/ObstacleAvoidanceScenario.cpp

gamecomponent_cpp:
	$(PRIMARY) gamecomponent_cpp.o $(ALLDEPS) ../BGE/GameComponent.cpp

modeltest_cpp:
	$(PRIMARY) modeltest_cpp.o $(ALLDEPS) ../BGE/ModelTest.cpp

steeringcontroler_cpp:
	$(PRIMARY) steeringcontroler_cpp.o $(ALLDEPS) ../BGE/SteeringControler.cpp

utils_cpp:
	$(PRIMARY) utils_cpp.o $(ALLDEPS) ../BGE/Utils.cpp

linedrawer_cpp:
	$(PRIMARY) linedrawer_cpp.o $(ALLDEPS) ../BGE/LineDrawer.cpp

soundsystem_cpp:
	$(PRIMARY) soundsystem_cpp.o $(ALLDEPS) ../BGE/SoundSystem.cpp

lab7_cpp:
	$(PRIMARY) lab7_cpp.o $(ALLDEPS) ../BGE/Lab7.cpp

lab6_cpp:
	$(PRIMARY) lab6_cpp.o $(ALLDEPS) ../BGE/Lab6.cpp

lab5_cpp:
	$(PRIMARY) lab5_cpp.o $(ALLDEPS) ../BGE/Lab5.cpp

shaderfunctions_cpp:
	$(PRIMARY) shaderfunctions_cpp.o $(ALLDEPS) ../BGE/ShaderFunctions.cpp

params_cpp:
	$(PRIMARY) params_cpp.o $(ALLDEPS) ../BGE/Params.cpp

lab8_cpp:
	$(PRIMARY) lab8_cpp.o $(ALLDEPS) ../BGE/Lab8.cpp

fbo_cpp:
	$(PRIMARY) fbo_cpp.o $(ALLDEPS) ../BGE/FBO.cpp

ground_cpp:
	$(PRIMARY) ground_cpp.o $(ALLDEPS) ../BGE/Ground.cpp


clean:
	rm *.o
