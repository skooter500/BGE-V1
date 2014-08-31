#include "LeapSampleListener.h"


LeapSampleListener::LeapSampleListener()
{
}


LeapSampleListener::~LeapSampleListener()
{
}

void LeapSampleListener::onInit(const Controller& controller) 
{
	std::cout << "Initialized" << std::endl;
}

void LeapSampleListener::onConnect(const Controller& controller) 
{
	std::cout << "Connected" << std::endl;
}

void LeapSampleListener::onDisconnect(const Controller& controller) 
{
	// Note: not dispatched when running in a debugger.
	std::cout << "Disconnected" << std::endl;
}

void LeapSampleListener::onExit(const Controller& controller) 
{
	std::cout << "Exited" << std::endl;
}

void LeapSampleListener::onFrame(const Controller& controller)
{

}
