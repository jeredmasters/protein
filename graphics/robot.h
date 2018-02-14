#pragma once

#include "muscle.h"
#include "chromosome.h"

class robot 
{
	private:
		void fittness();
		void force();
		void gravity();
		void momentum();
		void friction();
		void floor();
		
	public:
		std::vector<joint*> joints;
		std::vector<muscle*> muscles;
		robot(chromosome * _gene);
		void tick();
		bool alive;
		chromosome * gene;
		void dispose();
};

robot::robot(chromosome * _gene)
{
	gene = _gene;
	alive = false;

	for (int i = 0; i < gene->dna.size() - 4; i += 4) {
		uint8_t t = gene->dna[0 + i] % 2;

		if(t == 0) {
			uint8_t a = gene->dna[1 + i];
			uint8_t b = gene->dna[2 + i];
			uint8_t c = gene->dna[2 + i];
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

	for (int i = 0; i < gene->dna.size() - 4; i += 4) {
		uint8_t t = gene->dna[0 + i] % 2;

		if (t == 1) {
			uint8_t a = gene->dna[1 + i];
			uint8_t b = gene->dna[2 + i];
			uint8_t c = gene->dna[2 + i];
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
	if (muscles.size() < joints.size()) {
		return;
	}

	int lowest_joint = INT_MAX;
	for (int i = 0; i < joints.size(); i++) {
		if (joints[i]->position->y < lowest_joint) {
			lowest_joint = joints[i]->position->y;
		}
	}
	for (int i = 0; i < joints.size(); i++) {
		joints[i]->position->y -= lowest_joint;
	}

	
	alive = true;
}


void robot::tick()
{
	if (alive) {
		force();
		gravity();
		momentum();
		friction();
		floor();		
	}
	fittness();
}
void robot::fittness() {
	int max = 0;
	if (alive) {
		for (int i = 0; i < joints.size(); i++) {
			int val = joints[i]->position->x * joints[i]->position->y;
			if (val > max) {
				max = val;
			}
		}
	}
	gene->fittness = max;
}
void robot::force() {
	for (int i = 0; i < muscles.size(); i++) {
		muscle* m = muscles[i];
		float length = m->length();
		float delta = length - m->desiredLength;
		float scaled = (pow(delta / 5, 5) / 10000) *  m->strength;
		if (scaled == scaled && length != 0) { // check for NaN
			if (scaled > 10) {
				cout << "Something went wrong, killing robot for excessive force ";
				alive = false;
			}

			float rX = m->dX() / length;
			float rY = m->dY() / length;
			float cY = rY * scaled;
			float cX = rX * scaled;
			
			m->a->velocity->x -= cX;
			m->a->velocity->y -= cY;

			m->b->velocity->x += cX;
			m->b->velocity->y += cY;
		}
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

void robot::dispose() {
	for (int i = 0; i < joints.size(); i++) {
		delete joints[i];
	}
	joints.clear();
	for (int i = 0; i < muscles.size(); i++) {
		delete muscles[i];
	}
	muscles.clear();
	gene->dispose();
	delete gene;
}