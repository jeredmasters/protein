#pragma once

#include "stdafx.h"
#include "joint.h"

class muscle {
	public:
		joint* a;
		joint* b;
		float desiredLength;
		float currentLength;
		float strength;
		muscle();
		muscle(joint* _a, joint* _b, float dl, float cl, float s);
};

muscle::muscle(joint* _a, joint* _b, float dl, float cl, float s) {
	a = _a;
	b = _b;
	desiredLength = dl;
	currentLength = cl;
	strength = s;
}