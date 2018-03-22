#pragma once


#include <SFML/Graphics.hpp>
#include "windows.h"
#include "stdafx.h"
#include <windows.h>

renderer::renderer(int height, int width) :
	_window(sf::VideoMode(width, height), "GA"),
	_height(height),
	_width(width)
{
	_offsetX = -250;
	_zeroA = new point(0, 0);
	_zeroB = new point(0, _height);
}

void renderer::update(std::vector<robot*>* robots) {


	sf::Event event;
	while (_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			_window.close();

		switch (event.type) {
			case sf::Event::Closed:
				_window.close();
			case sf::Event::MouseWheelScrolled:
				_offsetX += event.mouseWheelScroll.delta * 5;
		}
	}


	_window.clear();
	long fittest = 0;
	int fittext_index = 0;
	for (int i = 0; i < robots->size(); i++) {
		if (robots[0][i]->gene->fittness > fittest) {
			fittext_index = i;
			fittest = robots[0][i]->gene->fittness;
		}
		
	}
	robots[0][0]->tag = sf::Color(50, 50, 255, 255);
	robots[0][fittext_index]->tag = sf::Color(50, 200, 200, 255);

	drawLine(_zeroA, _zeroB, sf::Color(255, 255, 255, 255));

	for (int i = 0; i < robots->size(); i++) {
		if (robots[0][i]->alive) {
			drawRobot(robots[0][i]);
		}
		robots[0][i]->tag = sf::Color(0, 0, 0, 0);
	}

	_window.display();
}

float renderer::modY(float y) {
	return _height - (y + 20);
}

float renderer::modX(float x) {
	return x - _offsetX;
}

void renderer::drawRobot(robot * r) {

	for (int i = 0; i < r->muscles.size(); i++) {
		drawMuscle(r->muscles[i]->a->position, r->muscles[i]->b->position, r->muscles[i]->strength);
	}

	point * highest = r->joints[0]->position;
	for (int i = 0; i < r->joints.size(); i++) {
		drawJoint(r->joints[i]->position, r->joints[i]->weight);
		if (r->joints[i]->position->y > highest->y) {
			highest = r->joints[i]->position;
		}
	}
	if (r->tag.r > 0 && r->tag.g > 0 && r->tag.b > 0) {
		drawLine(new point(highest->x, highest->y + 200), highest, r->tag);
	}
}

void renderer::drawJoint(point * p, int weight) {
	float radius = 4.f;

	sf::Color c = sf::Color(50, 255 - weight, 50, 255);

	sf::CircleShape shape(radius);
	shape.setFillColor(c);
	shape.setPosition(modX(p->x - radius), modY(p->y) - radius);
	_window.draw(shape);
}

void renderer::drawMuscle(point * a, point * b, int strength) {

	sf::Color c = sf::Color(255 - strength, 50, 50, 255);
	drawLine(a, b, c);
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