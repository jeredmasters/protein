#pragma once


#include <SFML/Graphics.hpp>
#include "windows.h"
#include "stdafx.h"
#include <windows.h>

renderer::renderer() :
	_height(0),
	_width(0)
{
}

renderer::renderer(int height, int width) :
	_window(sf::VideoMode(width, height), "GA"),
	_height(height),
	_width(width)
{
	_offsetY = height;
	_offsetX = -250;
	_zeroA = new point(0, 0);
	_zeroB = new point(0, _height);
	_fitness = 3;
	_generation = 0;
	_mark = 0;
	_redraw = true;
}

void renderer::update(std::vector<robot*>* robots, obstacle * _obstacle) {
	if (_height == 0) {
		return;
	}

	sf::Event event;
	while (_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			_window.close();

		switch (event.type) {
			case sf::Event::Closed:
				_window.close();
			case sf::Event::MouseWheelScrolled:
				_offsetX += event.mouseWheelScroll.delta * 50;
		}
	}


	_window.clear(sf::Color(255, 255, 255, 255));
	long fittest = 0;
	int fittext_index = 0;
	for (int i = 0; i < robots->size(); i++) {
		if (robots[0][i]->gene->fitness > fittest) {
			//fittext_index = i;
			fittest = robots[0][i]->gene->fitness;
		}
	}
	robots[0][0]->tag = sf::Color(50, 50, 255, 255);
	robots[0][fittext_index]->tag = sf::Color(50, 200, 200, 255);

	drawLine(_zeroA, _zeroB, sf::Color(0, 0, 0, 255));

	for (int i = 0; i < robots->size(); i++) {
		if (robots[0][i]->alive) {
			drawRobot(robots[0][i]);
		}
		robots[0][i]->tag = sf::Color(0, 0, 0, 0);
	}

	drawObstacle(_obstacle);

	_window.display();
}
void renderer::writeRobotToFile(int fitnessEval, int generation, int mark, robot* r) {

	ofstream myfile;
	myfile.open("C:\\temp\\f" + to_string(fitnessEval) + "_m" + to_string(mark) + "_g" + to_string(generation) + ".txt", std::ios_base::app);
	myfile << "======================================\n";
	for (int i = 0; i < r->muscles.size(); i++) {
		myfile << "M," << r->muscles[i]->strength << "," << r->muscles[i]->osc_speed << "," << r->muscles[i]->a->position->x << "," << r->muscles[i]->a->position->y << "," << r->muscles[i]->b->position->x << "," << r->muscles[i]->b->position->y << "," << "\n";
	}

	for (int i = 0; i < r->joints.size(); i++) {
		drawJoint(r->joints[i]->position, r->joints[i]->weight);
		myfile << "J," << r->joints[i]->weight << "," << r->joints[i]->position->x << "," << r->joints[i]->position->y << "\n";
	}

	myfile.close();
}
void renderer::update(robot* robot, obstacle * _obstacle) {
	if (_height == 0) {
		return;
	}

	sf::Event event;
	while (_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			_window.close();

		switch (event.type) {
		case sf::Event::Closed:
			_window.close();
		case sf::Event::MouseWheelScrolled:
			_offsetX += event.mouseWheelScroll.delta * 50;
		}
	}


	_window.clear(sf::Color(255, 255, 255, 255));

	drawLine(_zeroA, _zeroB, sf::Color(0, 0, 0, 255));


	drawRobot(robot);


	drawObstacle(_obstacle);

	_window.display();
}

float renderer::modY(float y) {
	return _offsetY - (y/4 + 20);
}

float renderer::modX(float x) {
	return x/4 - _offsetX;
}

void renderer::drawRobot(robot * r) {

	for (int i = 0; i < r->muscles.size(); i++) {
		drawMuscle(r->muscles[i]->a->position, r->muscles[i]->b->position, r->muscles[i]->strength);
		if (r->muscles[i]->hasOsc()) {
			drawOscDot(r->muscles[i]->a->position, r->muscles[i]->b->position, r->muscles[i]->osc_speed);
		}
	}

	point * highest = r->joints[0]->position;
	for (int i = 0; i < r->joints.size(); i++) {
		drawJoint(r->joints[i]->position, r->joints[i]->weight);
		if (r->joints[i]->position->y > highest->y) {
			highest = r->joints[i]->position;
		}
	}
	if (r->tag.r > 0 && r->tag.g > 0 && r->tag.b > 0) {
		//point * top = new point(highest->x, highest->y + 200);
		//drawLine(top, highest, r->tag);
		//delete top;
	}
}

void renderer::drawJoint(point * p, int weight) {
	float radius = 3.f;

	sf::Color c = sf::Color(0, 255 - weight, 0, 255);

	sf::CircleShape shape(radius);
	shape.setFillColor(c);
	shape.setPosition(modX(p->x) - radius, modY(p->y) - radius);
	_window.draw(shape);
}

void renderer::drawMuscle(point * a, point * b, int strength) {

	sf::Color c = sf::Color(255 - strength, 50, 50, 255);
	drawLine(a, b, c);
}

void renderer::drawOscDot(point * a, point * b, int osc_speed) {
	int x = (a->x + b->x) / 2;
	int y = (a->y + b->y) / 2;

	float radius = 3.f;

	sf::Color c = sf::Color(0, 0, osc_speed * 5, 255);

	sf::CircleShape shape(radius);
	shape.setFillColor(c);
	shape.setPosition(modX(x) - radius, modY(y) - radius);
	_window.draw(shape);
}

void renderer::drawLine(point * a, point * b, sf::Color c) {
	
	sf::Vertex line[] =
	{
		sf::Vertex(sf::Vector2f(modX(a->x), modY(a->y))),
		sf::Vertex(sf::Vector2f(modX(b->x), modY(b->y)))
	};

	line[0].color = c;
	line[1].color = c;

	_window.draw(line, 2, sf::Lines);
}

void renderer::drawObstacle(obstacle * _obstacle) {
	std::vector<point *> * points = _obstacle->points();

	sf::Vertex * line = (sf::Vertex *)malloc(points->size() * sizeof(sf::Vertex));

	for (int i = 0; i < points->size(); i++) {
		line[i] = sf::Vertex(sf::Vector2f(modX(points[0][i]->x), modY(points[0][i]->y)));
		line[i].color = sf::Color(0, 0, 0, 255);
	}

	_window.draw(line, points->size(), sf::Lines);	

	free(line);
}

void renderer::renderFromFile() {


	sf::Event event;
	while (_window.pollEvent(event)){
		switch (event.type) {
			case sf::Event::Closed:
				_window.close();
				break;
			case sf::Event::KeyPressed:

				

				_redraw = true;
				if (event.key.code == 72) {
					_generation++;
					if (_generation > 4) {
						_generation = 0;
						_mark++;
						if (_mark > 4) {
							_mark = 0;
							_fitness++;
							if (_fitness > 3) {
								_fitness = 1;
							}
						}
					}
				}
				if (event.key.code == 71) {
					_generation--;
					if (_generation < 0) {
						_generation = 4;
						_mark--;
						if (_mark < 0) {
							_mark = 4;
							_fitness--;
							if (_fitness < 1) {
								_fitness = 1;
							}
						}
					}
				}
				break;
		}
	}

	if (_redraw) {
		vector<int> gens;
		gens.push_back(1);
		gens.push_back(10);
		gens.push_back(50);
		gens.push_back(150);
		gens.push_back(200);
		renderFile(_fitness, _mark, gens[_generation]);
		_redraw = false;
	}
}

void renderer::renderFile(int fitnessEval, int mark, int generation) {

	ifstream myReadFile;
	myReadFile.open("C:\\temp\\f" + to_string(fitnessEval) + "_m" + to_string(mark) + "_g" + to_string(generation) + ".txt");
	char output[100];

	_window.clear(sf::Color(255, 255, 255, 255));
	_offsetY = 0;
	sf::Font font;
	font.loadFromFile("C:\\temp\\arial.ttf");

	sf::Text text;

	string m = "Generation: " + to_string(generation);
	text.setFont(font);
	text.setString(m);
	text.setCharacterSize(18);
	text.setFillColor(sf::Color::Black);
	text.setPosition(modX(100), modY(-100));
	_window.draw(text);

	int time = 0;
	
	if (myReadFile.is_open()) {
		while (!myReadFile.eof()) {
			myReadFile >> output;

			vector<string> strings;
			istringstream f(output);
			point * a;
			point * b;
			string s;
			while (getline(f, s, ',')) {
				strings.push_back(s);
			}
			if (strings.size() > 0) {
				if (strings[0] == "M") {
					float strength = std::stof(strings[1]);
					float osc_speed = std::stof(strings[2]);
					a = new point(std::stof(strings[3]), std::stof(strings[4]));
					b = new point(std::stof(strings[5]), std::stof(strings[6]));
					drawMuscle(a, b, strength);
					if (osc_speed > 0) {
						drawOscDot(a, b, osc_speed);
					}
					delete a;
					delete b;
				}
				else if (strings[0] == "J") {
					float weight = std::stof(strings[1]);
					a = new point(std::stof(strings[2]), std::stof(strings[3]));
					drawJoint(a, weight);
					delete a;
				}
				else if (strings[0] == "======================================") {
					_offsetY = _offsetY + 100;

					sf::Text text;

					text.setFont(font);
					// text.setString("Time: " + to_string(time * 5) + "s");
					text.setCharacterSize(14);
					text.setFillColor(sf::Color::Black);
					text.setPosition(modX(-50), modY(100));
					_window.draw(text);
					time++;
				}
			}
			a = new point(-200, 0);
			b = new point(2000, 0);
			drawLine(a, b, sf::Color(0, 0, 0, 255));
			delete a;
			delete b;

			a = new point(0, 0);
			b = new point(0, 255);
			drawLine(a, b, sf::Color(0, 0, 0, 255));
			delete a;
			delete b;
		}
	}
	
	_window.display();
	myReadFile.close();
}