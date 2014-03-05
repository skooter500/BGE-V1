#pragma once
#include <stdexcept>
#include "GameComponent.h"

using namespace std;


namespace BGE
{
	class StateMachine;

	class State
	{
	public:
		State(shared_ptr<StateMachine> owner);
		~State(void);

		virtual void Enter() = 0;
		virtual void Exit() = 0;
		virtual void Update(float timeDelta) = 0;
		virtual string Description() = 0;
		shared_ptr<StateMachine> owner;
	};

	class StateMachine :
		public GameComponent
	{
	public:
		StateMachine(void);
		~StateMachine(void);

		shared_ptr<State> currentState;

		void Update(float timeDelta);
		void SwicthState(shared_ptr<State> newState);

	};

	
}

