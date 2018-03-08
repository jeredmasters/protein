#pragma once

#include "stdafx.h"


robot::robot(chromosome * _gene)
{
	gene = _gene;
	alive = false;
	gene->fittness = 0;

	for (int i = 0; i < gene->dna.size() - 4; i += 4) {
		uint8_t t = gene->dna[0 + i] % 5;

		if (t == 0 || t == 1) {
			uint8_t a = gene->dna[1 + i];
			uint8_t b = gene->dna[2 + i];
			uint8_t c = gene->dna[3 + i];
			joints.push_back(new joint(
				new point(a, b),
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
			uint8_t c = gene->dna[3 + i];
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
			uint8_t c = gene->dna[3 + i];
			int m_id = a % muscles.size();

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
	for (int i = 0; i < muscles.size(); i++) {
		if (muscles[i]->a == muscles[i]->b) {
			return;
		}
	}

	validate();

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
		fittness();
	}

}
void robot::tick(timer_collection* timers)
{
	if (alive) {
		timers->osc.start();
		osc();
		timers->osc.stop();


		timers->reaction.start();
		reaction();
		timers->reaction.stop();


		timers->gravity.start();
		gravity();
		timers->gravity.stop();


		timers->momentum.start();
		momentum();
		timers->momentum.stop();


		timers->friction.start();
		friction();
		timers->friction.stop();


		timers->floor.start();
		floor();
		timers->floor.stop();
	}

}
void robot::validate() {
	for (int i = 0; i < joints.size(); i++) {
		if (joints[i]->inf()) {
			cout << "INF; Killing robot :(\n";
			alive = false;
			gene->fittness = 0;
		}
	}
	for (int i = 0; i < muscles.size(); i++) {
		if (muscles[i]->inf()) {
			cout << "INF; Killing robot :(\n";
			alive = false;
			gene->fittness = 0;
		}
	}
}
long robot::fittness() {
	long total = 0;
	if (alive) {
		for (int i = 0; i < joints.size(); i++) {
			total += (joints[i]->position->x * joints[i]->position->y) ;
		}
	}
	if (total <= 0) {
		gene->fittness = 0;
	}
	else {
		gene->fittness = total / joints.size();
	}
	return gene->fittness;
}
double springForce(float sd) {
	float q = pow(sd / 3, 2);
	if (sd < 0) {
		q *= -1;
	}
	return sd * 5 + q * 2;
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
		double length = m->length();
		float delta = length - m->desiredLength();
		float scaled = m->strength * delta;
		double force = springForce(scaled);

		force = force / TICK_PER_SEC;

		if (force == force && length != 0) { // check for NaN
			if (force > 1e10) {
				cout << "Something went wrong, killing robot for excessive force: " << force << "\n";
				alive = false;
				gene->fittness = 0;
			}


			float rX = m->dX() / length;
			float rY = m->dY() / length;
			float accY = rY * force;
			float accX = rX * force;


			m->a->velocity->x -= accX / (m->a->weight * 2);
			m->a->velocity->y -= accY / (m->a->weight * 2);

			m->b->velocity->x += accX / (m->b->weight * 2);
			m->b->velocity->y += accY / (m->b->weight * 2);

			if (
				m->inf() ||
				m->a->inf() ||
				m->b->inf()
				) {
				cout << "INF; Killing robot :(\n";
				alive = false;
				gene->fittness = 0;
			}
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
		joints[i]->velocity->x *= 0.998;
		joints[i]->velocity->y *= 0.998;
	}
}

void robot::floor()
{
	float impact;
	float friction;
	for (int i = 0; i < joints.size(); i++) {
		if (joints[i]->position->y <= 0) {
			impact = 1 - (joints[i]->position->y + 1);
			friction = pow(impact * 5, 2);
			joints[i]->velocity->x = joints[i]->velocity->x / (friction + 1);

			joints[i]->position->y = 0;
			joints[i]->velocity->y = abs(joints[i]->velocity->y) * 0.3;
		}
	}
}

robot::~robot() {
	for (int i = 0; i < joints.size(); i++) {
		delete joints[i];
	}
	joints.clear();
	for (int i = 0; i < muscles.size(); i++) {
		delete muscles[i];
	}
	muscles.clear();
}