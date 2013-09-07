#pragma once
#include "State.h"

namespace BGE
{
	class AttackingState :
		public State
	{
	public:
		AttackingState(shared_ptr<StateMachine> owner, shared_ptr<GameComponent> enemy);
		~AttackingState(void);

		virtual void Enter();
		virtual void Exit();
		virtual void Update(float timeDelta);

		virtual string Description();

		shared_ptr<GameComponent> enemy;
		float timeShot;
	};
}

