#pragma once

#include "stdafx.h"
#include "point.h"

class joint {
	public:
		point* position;
		point* velocity;
		int weight;
		joint();		
		joint(point* pos, point* vel, int w);
		int fittness();
};

joint::joint(point* pos, point* vel, int w) {
	position = pos;
	velocity = vel;
	weight = w;
}
