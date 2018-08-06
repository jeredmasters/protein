
#include "stdafx.h"

point::point() {
}
point::point(float _x, float _y) {
	x = _x;
	y = _y;
}

float point::magnitude() {
	return sqrt(pow(x, 2) + pow(y, 2));
}

float point::angle() {
	return atan2(y, x);
}