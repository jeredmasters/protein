#pragma once

#include "stdafx.h"

robot::robot(chromosome * _gene)
{
	_verticalInfringements = 0;
	gene = _gene;
	alive = false;
	gene->fittness = 0;

	for (int i = 0; i < gene->dna.size(); i++) {
		uint8_t * data = gene->get(i);
		uint8_t t = data[0] / 2;

		if (t >= 0 && t <= 2) {  // 3/8
 			uint8_t a = data[1] * 15;
			uint8_t b = data[2] * 15;
			uint8_t c = data[3] * 15;
			joints.push_back(new joint(
				new point(a, b),
				c
			));
		}

		delete data;
	}
	if (joints.size() == 0) {
		return;
	}

	for (int i = 0; i < gene->dna.size(); i++) {
		uint8_t * data = gene->get(i);
		uint8_t t = data[0] / 2;

		if (t >= 3 && t <= 6) {
			uint8_t a = data[1] + i;
			uint8_t b = data[2] * 15;
			uint8_t c = data[3] * 15;
			int j_a = (muscles.size() + 1) % joints.size();
			int j_b = (a + i) % joints.size();

			muscles.push_back(new muscle(
				joints[j_a],
				joints[j_b],
				c
			));
		}

		delete data;
	}

	if (muscles.size() < joints.size()) {
		return;
	}

	for (int i = 0; i < gene->dna.size(); i++) {
		uint8_t * data = gene->get(i);
		uint8_t t = data[0] / 2;

		if (t == 7) {
			uint8_t a = data[1] * 15;
			uint8_t b = data[2] * 15;
			uint8_t c = data[3] * 15;
			int m_id = (a + i) % muscles.size();

			muscles[m_id]->setOsc(b, c - 122);
		}

		delete data;
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


void robot::tick(obstacle * _obstacle)
{
	if (alive) {
		osc();
		reaction();
		gravity();		
		friction();	
		floor(_obstacle);
		applyForce();
		momentum();
		fittness();		
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
		alive = false;
		return 0;
	}
	else {
		gene->fittness = total / joints.size();
	}
	return gene->fittness;
}
double springForce(float sd) {
	float q = pow(sd / 4, 2);
	if (sd < 0) {
		q *= -1;
	}
	return sd * 4 + q;
}
void robot::osc() {
	for (int i = 0; i < muscles.size(); i++) {
		if (muscles[i]->osc_speed != 0) {
			muscles[i]->oscTick();
		}
	}
}
bool vertical(joint * a, joint * b) {
	return(
		a->velocity->x == 0 &&
		b->velocity->x == 0 &&
		a->position->x == b->position->x
	);
}

void robot::reaction() {
	float desiredlength, delta, scaled, rX, rY, accY, accX;
	double length, force;
	for (int i = 0; i < muscles.size(); i++) {
		muscle* m = muscles[i];
		length = m->length();
		desiredlength = m->desiredLength();
		delta = length - desiredlength;
		scaled = m->strength * delta;
		force = springForce(scaled);

		force = force / TICK_PER_SEC;

		if (force == force && length != 0) { // check for NaN
			if (force > 1e10) {
				cout << "Something went wrong, killing robot for excessive force: " << force << "\n";
				alive = false;
				gene->fittness = 0;
			}


			rX = m->dX() / length;
			rY = m->dY() / length;
			accY = rY * force;
			accX = rX * force;


			m->a->force->x -= accX;
			m->a->force->y -= accY;

			m->b->force->x += accX;
			m->b->force->y += accY;

			if (vertical(m->a, m->b)) {
				_verticalInfringements++;
				if (_verticalInfringements > 3) {
					alive = false;
					cout << "VERT; Killing robot :(\n";
				}
			}
			else {
				_verticalInfringements = 0;
			}

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

void robot::friction()
{
	for (int i = 0; i < joints.size(); i++) {
		joints[i]->velocity->x *= 0.998;
		joints[i]->velocity->y *= 0.998;
	}
}

void robot::applyForce()
{
	for (int i = 0; i < joints.size(); i++) {
		joints[i]->velocity->x += joints[i]->force->x / (joints[i]->weight * 2);
		joints[i]->velocity->y += joints[i]->force->y / (joints[i]->weight * 2);
		joints[i]->force->x = 0;
		joints[i]->force->y = 0;
	}
}

void robot::momentum()
{
	for (int i = 0; i < joints.size(); i++) {
		joints[i]->position->x += joints[i]->velocity->x / TICK_PER_SEC;
		joints[i]->position->y += joints[i]->velocity->y / TICK_PER_SEC;
	}
}

float pi = 3.14159265359;
float _2pi = pi * 2;
float pi_2 = pi / 2;
float pi_8 = pi / 8;



point * findComponent(point * velocity, float c_ang) {
	float v_ang = velocity->angle();
	float v_mag = velocity->magnitude();
	float c_mag = cos(v_ang - c_ang) * v_mag;

	return new point(c_mag * cos(c_ang), c_mag * sin(c_ang));
}
float deceleration_interval = 10.f; // 10 ms
float friction_coefficient = 2;
void robot::floor(obstacle * _obstacle)
{
	float impact;
	float friction;
	for (int i = 0; i < joints.size(); i++) {
		edge * e = _obstacle->edgeAt(joints[i]->position->x);
		if (joints[i]->position->y <= e->y) {

			point * impact_velocity = findComponent(joints[i]->velocity, e->angle - pi_2);
			point * impact_force = findComponent(joints[i]->force, e->angle - pi_2);
			point * tangent_velocity = findComponent(joints[i]->velocity, e->angle);
			point * tangent_force = findComponent(joints[i]->force, e->angle);

			float friction = ((impact_velocity->magnitude() / deceleration_interval) * (float)joints[i]->weight + impact_force->magnitude()) * friction_coefficient;
			float tangent_magnitude = tangent_velocity->magnitude() * (joints[i]->weight * 2) + tangent_force->magnitude();

			// Static friction
			if (tangent_magnitude < friction) {
				joints[i]->force->y -= (tangent_velocity->y * (joints[i]->weight * 2) + tangent_force->y);
				joints[i]->force->x -= (tangent_velocity->x * (joints[i]->weight * 2) + tangent_force->x);
			}
			else {
				joints[i]->force->y -= (tangent_velocity->y * (joints[i]->weight * 2) + tangent_force->y) / 50;
				joints[i]->force->x -= (tangent_velocity->x * (joints[i]->weight * 2) + tangent_force->x) / 50;
			}

			joints[i]->force->y = joints[i]->force->y - impact_velocity->y * (joints[i]->weight * 2) - impact_force->y;
			joints[i]->force->x = joints[i]->force->x - impact_velocity->x * (joints[i]->weight * 2) - impact_force->x;

			joints[i]->position->y = e->y;

			delete impact_force;
			delete impact_velocity;
			delete tangent_velocity;
			delete tangent_force;
		}
		delete e;
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