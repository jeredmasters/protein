#pragma once

class point {
	public:
		float x;
		float y;
		point();
		point(float _x, float _y);
};

point::point() {
}
point::point(float _x, float _y) {
	x = _x;
	y = _y;
}

