#pragma once

#include "muscle.h"
#include "chromosome.h"
#include "stdafx.h"

class robot 
{
	private:
		void fittness();
		void osc();
		void reaction();
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
		uint8_t t = gene->dna[0 + i] % 5;

		if(t == 0 || t == 1) {
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
		uint8_t t = gene->dna[0 + i] % 5;

		if (t == 2 || t == 3) {
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

	for (int i = 0; i < gene->dna.size() - 4; i += 4) {
		uint8_t t = gene->dna[0 + i] % 5;

		if (t == 4) {
			uint8_t a = gene->dna[1 + i];
			uint8_t b = gene->dna[2 + i];
			uint8_t c = gene->dna[2 + i];
			int m_id  = a % muscles.size();

			muscles[m_id]->setOsc(b, c - 122);
		}
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
		osc();
		reaction();
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
float springForce(float sd){
	float q = pow(sd / 5, 2);
	if (sd < 0) {
		q *= -1;
	}
	return sd * 5 + q ;
}
void robot::osc() {
	for (int i = 0; i < muscles.size(); i++) {
		if (muscles[i]->osc_speed != 0) {
			muscles[i]->oscTick();
		}
	}
}
void robot::reaction() {
	for (int i = 0; i < muscles.size(); i++) {
		muscle* m = muscles[i];
		float length = m->length();
		float delta = length - m->desiredLength();
		float scaled = m->strength * delta;
		float force = springForce(scaled);

		force = force / TICK_PER_SEC;

		if (force == force && length != 0) { // check for NaN
			if (force > 1000000) {
				cout << "Something went wrong, killing robot for excessive force ";
				alive = false;
			}
			

			float rX = m->dX() / length;
			float rY = m->dY() / length;
			float accY = rY * force;
			float accX = rX * force;

			
			m->a->velocity->x -= (accX / 1000) / m->a->weight;
			m->a->velocity->y -= accY / m->a->weight;

			m->b->velocity->x += accX / m->a->weight;
			m->b->velocity->y += accY / m->a->weight;
		}
	}
}

void robot::gravity()
{
	for (int i = 0; i < joints.size(); i++) {
		joints[i]->velocity->y -= 9800 / TICK_PER_SEC;
	}
}

void robot::momentum()
{
	for (int i = 0; i < joints.size(); i++) {
		joints[i]->position->x += joints[i]->velocity->x / TICK_PER_SEC;
		joints[i]->position->y += joints[i]->velocity->y / TICK_PER_SEC;
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
		if (joints[i]->position->y < -1) {
			joints[i]->position->y = 0;
			joints[i]->velocity->y = abs(joints[i]->velocity->y) * 0.3;
		}
		if (joints[i]->position->y <= 1) {
			float friction = (joints[i]->position->y + 1);
			joints[i]->velocity->x = joints[i]->velocity->x / (pow(friction, 2) + 1);
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