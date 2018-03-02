#pragma once

#include "stdafx.h"


joint::joint(point* pos, int w) {
	position = pos;
	velocity = new point(0, 0);
	weight = w + 30;
	force = new point(0, 0);
}

joint::~joint() {
	delete position;
	delete velocity;
	delete force;
}

bool joint::inf() {
	return
		isinf(position->x) ||
		isinf(position->y) ||
		isinf(velocity->x) ||
		isinf(velocity->y) ||
		isinf(force->x) ||
		isinf(force->y);
}