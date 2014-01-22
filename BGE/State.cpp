#include "State.h"
#include "Game.h"

using namespace BGE;

// State Stuff
State::State(shared_ptr<StateMachine> owner)
{
	this->owner = owner;
}


State::~State(void)
{
}

std::string BGE::State::Description()
{
	throw std::logic_error("The method or operation is not implemented.");
}

StateMachine::StateMachine(void)
{
	currentState = nullptr;
}

StateMachine::~StateMachine(void)
{
}


// State Machine Stuff
void StateMachine::SwicthState(shared_ptr<State> newState)
{
	if (currentState != nullptr)
	{
		currentState->Exit();
	}

	currentState = newState;
	if (newState != nullptr)
	{
		currentState->Enter();
	}
}

void StateMachine::Update(float timeDelta)
{
	if (currentState != nullptr)
	{
		Game::Instance()->PrintText("State:" + currentState->Description());
		currentState->Update(timeDelta);
	}
	GameComponent::Update(timeDelta);
}
