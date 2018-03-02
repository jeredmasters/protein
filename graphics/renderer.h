#pragma once


#include <SFML/Graphics.hpp>
#include "stdafx.h"


using namespace std;

class renderer {
	private:
		sf::RenderWindow _window;
		int _height;
		int _width;
		float modY(float y);
	
	public:
		renderer(int height, int width);
		void update(std::vector<robot*>* robots);
		void drawRobot(robot * r);
		void drawJoint(point * p, int weight);
		void drawLine(point * a, point * b, int strength);
};
