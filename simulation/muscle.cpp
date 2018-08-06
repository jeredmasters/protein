#pragma once

#include "stdafx.h"

muscle::muscle(joint* _a, joint* _b, float s) {

	a = _a;
	b = _b;
	_desiredLength = length();
	strength = s;

	osc_speed = 0;
	osc_range = 0;
}

bool muscle::hasOsc() {
	return osc_speed != 0;
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

double muscle::length() {
	return sqrt(pow(dX(), 2) + pow(dY(), 2));
}

float muscle::angle() {
	return atan(dY() / dX());
}

bool muscle::inf() {
	return isinf(dX()) || isinf(dY());
}