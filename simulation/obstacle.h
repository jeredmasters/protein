#pragma once

#include "edge.h"
#include "point.h"
#include <vector>


class obstacle {
	private:
		int _type;
		std::vector<point *> _points;
		
	
	public:
		obstacle(int type);
		edge * edgeAt(float x);
		std::vector<point *> * points();
};