#pragma once

#include "muscle.h"

class robot 
{
	private:
		void force();
		void gravity();
		void momentum();
		void friction();
		void floor();
		
	public:
		std::vector<joint*> joints;
		std::vector<muscle*> muscles;
		robot();
		void tick();

};

robot::robot()
{
	joints.push_back(new joint(new point(5, 310), new point(1,1), 100));
	joints.push_back(new joint(new point(50, 480), new point(0,0), 50));
	muscles.push_back(new muscle(joints[0], joints[1], 30, 50));
}

void robot::tick()
{
	force();
	gravity();
	momentum();
	friction();
	floor();
}

void robot::force() {
	for (int i = 0; i < muscles.size(); i++) {
		muscle* m = muscles[i];
		float length = m->length();
		float delta = length - m->desiredLength;
		float rX = m->dX() / length;
		float rY = m->dY() / length;
		float cY = (delta / 10000) * rY * m->strength;
		float cX = (delta / 10000) * rX * m->strength;

		m->a->velocity->x -= cX;
		m->a->velocity->y -= cY;

		m->b->velocity->x += cX;
		m->b->velocity->y += cY;
	}
}

void robot::gravity()
{
	for (int i = 0; i < joints.size(); i++) {
		joints[i]->velocity->y -= 0.1;
	}
}

void robot::momentum()
{
	for (int i = 0; i < joints.size(); i++) {
		joints[i]->position->x += joints[i]->velocity->x;
		joints[i]->position->y += joints[i]->velocity->y;
	}
}

void robot::friction()
{
	for (int i = 0; i < joints.size(); i++) {
		joints[i]->velocity->x *= 0.995;
		joints[i]->velocity->y *= 0.995;
	}
}

void robot::floor()
{
	for (int i = 0; i < joints.size(); i++) {
		if (joints[i]->position->y < 0) {
			joints[i]->position->y = 0;
			joints[i]->velocity->y = 0;
			joints[i]->velocity->x *= 0.98;
		}
	}
}

