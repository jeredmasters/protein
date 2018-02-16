#pragma once

#include "stdafx.h"
#include "joint.h"
#include <math.h> 

class muscle {
	public:
		joint* a;
		joint* b;
		float desiredLength;
		float osc_range;
		float osc_pos;
		bool osc_dir;
		
		float strength;
		muscle();
		float dX();
		float dY();
		float length();
		float angle();
		float force();
		muscle(joint* _a, joint* _b, float osc, float s);
};

muscle::muscle(joint* _a, joint* _b, float osc, float s) {
	a = _a;
	b = _b;
	desiredLength = length();
	strength = s;
}

float muscle::dX() {
	return a->position->x - b->position->x;
}

float muscle::dY() {
	return a->position->y - b->position->y;
}

float muscle::length() {
	return sqrt(pow(dX(), 2) + pow(dY(), 2));
}

float muscle::angle() {
	return atan(dY() / dX());
}