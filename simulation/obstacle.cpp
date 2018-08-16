
#include "stdafx.h"

obstacle::obstacle(int type) {
	_type = type;

	switch (_type) {
	case 0:
		_points.push_back(new point(-500, 0));
		_points.push_back(new point(10000, 0));
		break;
	case 1:
		_points.push_back(new point(-500, 0));
		_points.push_back(new point(800, 0));
		_points.push_back(new point(800, 0));
		_points.push_back(new point(10000, 1150));
		break;
	case 2:
		_points.push_back(new point(-500, 0));
		_points.push_back(new point(780, 0));

		_points.push_back(new point(780, 0));
		_points.push_back(new point(800, 30));

		_points.push_back(new point(800, 30));
		_points.push_back(new point(10000, 30));
		break;
	case 3:
		_points.push_back(new point(-500, 0));
		_points.push_back(new point(770, 0));

		_points.push_back(new point(770, 0));
		_points.push_back(new point(800, 30));

		_points.push_back(new point(800, 30));
		_points.push_back(new point(900, 30));

		_points.push_back(new point(900, 30));
		_points.push_back(new point(930, 0));

		_points.push_back(new point(930, 0));
		_points.push_back(new point(10000, 0));
		break;
	}
}

std::vector<point *> * obstacle::points() {
	return &_points;
}


edge * obstacle::edgeAt(float x) {
	switch (_type) {
	case 0:
		return new edge(x, 0, 0);
	case 1:
		if (x < 800) {
			return new edge(x, 0, 0);
		}
		else {
			int incline = 8;
			return new edge(x, (x - 800) / incline, 3.14159 / (incline * 4));
		}
	case 2:
		if (x < 800) {
			return new edge(x, 0, 0);
		}
		else {
			return new edge(x, 30, 0);
		}
	case 3:
		if (x < 770 || x > 930) {
			return new edge(x, 0, 0);
		}
		else if (x >= 800 && x <= 900) {
			return new edge(x, 30, 0);
		}
		else if (x < 800) {
			return new edge(x, x - 770, 3.14159 / 4);
		}
		else {
			return new edge(x, x - 900, -3.14159 / 4);
		}
	}
	return new edge(x, 0, 0);
}