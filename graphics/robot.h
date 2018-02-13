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
		robot(std::vector<uint8_t> chromosome);
		void tick();
		bool alive;

};

robot::robot(std::vector<uint8_t> chromosome)
{
	alive = false;

	for (int i = 0; i < chromosome.size() - 4; i += 4) {
		uint8_t t = chromosome[0 + i] % 2;

		if(t == 0) {
			uint8_t a = chromosome[1 + i];
			uint8_t b = chromosome[2 + i];
			uint8_t c = chromosome[2 + i];
			joints.push_back(new joint(
				new point(a, b),
				new point(0, 0),
				c
			));
		}
	}
	if (joints.size() == 0) {
		return;
	}

	for (int i = 0; i < chromosome.size() - 4; i += 4) {
		uint8_t t = chromosome[0 + i] % 2;

		if (t == 1) {
			uint8_t a = chromosome[1 + i];
			uint8_t b = chromosome[2 + i];
			uint8_t c = chromosome[2 + i];
			int j_a = (muscles.size() + 1) % joints.size();
			int j_b = a % joints.size();

			muscles.push_back(new muscle(
				joints[j_a],
				joints[j_b],
				b,
				c
			));
		}
	}
	if (muscles.size() == 0) {
		return;
	}
	
	alive = true;
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

