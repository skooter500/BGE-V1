#pragma once
#include "State.h"

namespace BGE
{
	class IdleState :
		public State
	{
	public:
		IdleState(shared_ptr<StateMachine> owner, shared_ptr<GameComponent> enemy);
		~IdleState(void);

		virtual void Enter();
		virtual void Exit();
		virtual void Update(float timeDelta);

		virtual string Description();

		shared_ptr<GameComponent> enemy;		
		static glm::vec3 initialPos;
	};
}

