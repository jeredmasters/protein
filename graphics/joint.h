#pragma once

#include "stdafx.h"
#include "point.h"

class joint {
	public:
		point* position;
		point* velocity;
		point* force;
		int weight;
		joint(point* pos, point* vel, int w);
		void dispose();
		bool inf();
};

joint::joint(point* pos, point* vel, int w) {
	position = pos;
	velocity = vel;
	weight = w + 30;
	force = new point(0, 0, 0);
}

void joint::dispose() {
	delete position;
	delete velocity;
	delete force;
}

bool joint::inf() {
	return
		isinf(position->x) ||
		isinf(position->y) ||
		isinf(position->z) ||
		isinf(velocity->x) ||
		isinf(velocity->y) ||
		isinf(velocity->z) ||
		isinf(force->x) ||
		isinf(force->y) ||
		isinf(force->z);
}