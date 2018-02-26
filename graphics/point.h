#pragma once

class point {
	public:
		float x;
		float y;
		float z;
		point();
		point(float _x, float _y, float _z);
};

point::point() {
}
point::point(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}

