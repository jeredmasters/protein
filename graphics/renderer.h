#pragma once


#include <SFML/Graphics.hpp>
#include "stdafx.h"


using namespace std;

class renderer {
	private:
		sf::RenderWindow _window;
		int _height;
		int _width;
		int _offsetX;
		float modY(float y);
		float modX(float x);
	
	public:
		renderer(int height, int width);
		void update(std::vector<robot*>* robots);
		void drawRobot(robot * r);
		void drawRobot(robot * r, sf::Color tag);
		void drawJoint(point * p, int weight);
		void drawLine(point * a, point * b, sf::Color c);
		void drawMuscle(point * a, point * b, int strength);
};
