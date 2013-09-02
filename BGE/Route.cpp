#include "Route.h"
#include "LineDrawer.h"

using namespace BGE;

Route::Route(void)
{
	worldMode = world_modes::from_self;
	looped = false;
	draw = true;
	next = waypoints.begin();
}

Route::~Route(void)
{
}

void Route::Draw()
{
	list<glm::vec3>::iterator it = waypoints.begin();
	while (it != waypoints.end())
	{
		LineDrawer::DrawLine(* (it ++), * (it ++), glm::vec3(137, 207, 240));
	}
	if (looped)
	{
		LineDrawer::DrawLine(* (-- it), * waypoints.begin(), glm::vec3(137, 207, 240));
	}

}

glm::vec3 Route::NextWaypoint()
{
	return * next;
}

bool Route::IsLast()
{

	return (next == (-- waypoints.end()) );
}

void Route::AdvanceToNext()
{
	if (looped)
	{                
		next ++;
		if (next == waypoints.end())
		{
			next = waypoints.begin();
		}
	}
	else
	{
		if (! IsLast())
		{
			next ++;
		}
	}
}