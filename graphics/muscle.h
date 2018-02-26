#pragma once

#include "stdafx.h"
#include "joint.h"
#include <math.h>
#include <stdexcept>

class muscle {
	public:
		joint* a;
		joint* b;
		float _desiredLength;
		float osc_range;
		float osc_pos;
		float osc_speed;
		
		float strength;
		float dX();
		float dY();
		float dZ();
		double length();
		float desiredLength();
		void oscTick();
		bool inf();
		void setOsc(float _osc_range, float _osc_speed);
		muscle(joint* _a, joint* _b, float osc, float s);
};

muscle::muscle(joint* _a, joint* _b, float osc, float s) {
	
	a = _a;
	b = _b;
	_desiredLength = length();
	strength = s + 30;

	osc_speed = 0;
	osc_range = 0;
}

void muscle::setOsc(float _osc_range, float _osc_speed) {
	osc_pos = 0;
	osc_speed = _osc_speed;
	osc_range = _osc_range;
}

void muscle::oscTick() {
	osc_pos += (5 * osc_speed) / TICK_PER_SEC;
	if (abs(osc_pos) > osc_range) {
		osc_speed *= -1;
	}
}

float muscle::desiredLength() {
	return _desiredLength + osc_pos;
}

float muscle::dX() {
	return a->position->x - b->position->x;
}

float muscle::dY() {
	return a->position->y - b->position->y;
}

float muscle::dZ() {
	return a->position->z - b->position->z;
}

double muscle::length() {
	float h = sqrt(pow(dX(), 2) + pow(dY(), 2));
	return sqrt(pow(dZ(), 2) + pow(h, 2));
}

bool muscle::inf() {
	return isinf(dX()) || isinf(dY()) || isinf(dZ());
}