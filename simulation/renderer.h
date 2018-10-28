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
		int _offsetY;
		float modY(float y);
		float modX(float x);
		point* _zeroA;
		point* _zeroB;
		int _fitness;
		int _generation;
		int _mark;
		bool _redraw;
	
	public:
		renderer();
		renderer(int height, int width);
		void update(std::vector<robot*>* robots, obstacle * _obstacle);
		void update(robot* robot, obstacle * _obstacle);
		void drawRobot(robot * r);
		void drawRobot(robot * r, sf::Color tag);
		void renderFile(int fitness, int mark, int generation);
		void renderFromFile();
		void writeRobotToFile(int fitness, int generation, int mark, robot* robot);
		void drawJoint(point * p, int weight);
		void drawLine(point * a, point * b, sf::Color c);
		void drawMuscle(point * a, point * b, int strength);
		void drawOscDot(point * a, point * b, int osc_speed);
		void drawObstacle(obstacle * _obstacle);
};
