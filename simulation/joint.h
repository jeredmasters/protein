#pragma once

#include "stdafx.h"
#include "point.h"

class joint {
	public:
		point* position;
		point* velocity;
		point* force;
		int weight;
		joint(point* pos, int w);
		~joint();
		bool inf();
};
