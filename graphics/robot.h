#pragma once

#include "muscle.h"

class robot 
{
	public:
		joint * joints;
		muscle* muscles;
		robot();
};

robot::robot()
{
	joints = {
		new joint(new point(5, 10), new point(1,1), 100)
	};
	muscles = {
		new muscle(&joints[0], &joints[0], 30, 20, 5)
	};
}