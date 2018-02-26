#pragma once


#ifdef _DEBUG
#pragma comment(lib,"sfml-graphics-d.lib")
#pragma comment(lib,"sfml-audio-d.lib")
#pragma comment(lib,"sfml-system-d.lib")
#pragma comment(lib,"sfml-window-d.lib")
#pragma comment(lib,"sfml-network-d.lib")
#else
#pragma comment(lib,"sfml-graphics.lib")
#pragma comment(lib,"sfml-audio.lib")
#pragma comment(lib,"sfml-system.lib")
#pragma comment(lib,"sfml-window.lib")
#pragma comment(lib,"sfml-network.lib")
#endif
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"
#include <GL/glu.h>


#include <SFML/Graphics.hpp>
#include "windows.h"
#include "stdafx.h"
#include <string>
#include <iostream>
#include "robot.h"

using namespace std;

class renderer {
	private:
		sf::RenderWindow _window;
		int _height;
		int _width;
		float modY(float y);
		bool render;
		float angle;
		sf::Clock Clock;
		float track_length;
		int downX, downY;
		int downTheta, downPhi, downRow;
		float stheta, sphi, srow, sdepth, mX, mY;
		bool mouseLeft, mouseRight;

	public:
		renderer(int height, int width);
		void update(std::vector<robot*>* robots);
		void drawRobot(robot * r);
		void drawJoint(point * p, int weight);
		void drawLine(point * a, point * b, int strength);
};




renderer::renderer(int height, int width) : 
	_window(sf::VideoMode(width, height, 32), "SFML OpenGL"),
	_height(height),
	_width(width)
{
	angle = 0;
	render = true;
	track_length = 1000;

	// Create a clock for measuring time elapsed
	// sf::Clock Clock;

	_window.setActive(true);

	//prepare OpenGL surface for HSR
	glClearDepth(1.f);
	glClearColor(0.3f, 0.3f, 0.3f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	//// Setup a perspective projection & Camera position
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120.f, 1.4, 1.f, 2000.0f);//fov, aspect, zNear, zFar

	stheta = 22;
	sphi = 1;
	srow = -1;
	sdepth = 420;
	mouseLeft = false;
	mouseRight = false;
	downX = -1;
	downY = -1;
	mX = -49;
	mY = -87;


}

void renderer::update(std::vector<robot*>* robots) {
	

	sf::Event Event;
	while (_window.pollEvent(Event))
	{
		// Close window : exit
		if (Event.type == sf::Event::Closed)
			_window.close();

		// Escape key : exit
		if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape))
			_window.close();

		if (Event.type == sf::Event::MouseMoved) {

			if (mouseLeft) {

				float ratio = (downPhi % 180) / 90;
				stheta -= (1 - ratio) * (downY - Event.mouseMove.y) / 5;
				srow -= ratio * (downY - Event.mouseMove.y) / 5;
				
				

				sphi -= (downX - Event.mouseMove.x) / 5;
				

				stheta = ((int)stheta + 360) % 360;
				sphi = ((int)sphi + 360) % 360;

				cout << stheta << "\t" << sphi << "\n";
			}
			if (mouseRight) {
				mX += (downX - Event.mouseMove.x) / 5;
				mY += (downY - Event.mouseMove.y) / 5;
			}
			downX = Event.mouseMove.x;
			downY = Event.mouseMove.y;
		}

		if (Event.type == sf::Event::MouseButtonPressed && Event.mouseButton.button == sf::Mouse::Button::Left) {
			mouseLeft = true;
			downTheta = stheta;
			downPhi = sphi;
		}

		if (Event.type == sf::Event::MouseButtonReleased && Event.mouseButton.button == sf::Mouse::Button::Left) {
			mouseLeft = false;
		}

		if (Event.type == sf::Event::MouseButtonPressed && Event.mouseButton.button == sf::Mouse::Button::Right) {
			mouseRight = true;
		}

		if (Event.type == sf::Event::MouseButtonReleased && Event.mouseButton.button == sf::Mouse::Button::Right) {
			mouseRight = false;
		}

		if (Event.type == sf::Event::MouseWheelScrolled) {
			sdepth -= Event.mouseWheelScroll.delta *5;
		}

		if (Event.type == sf::Event::KeyPressed) {
			switch (Event.key.code) {
				case sf::Keyboard::Up:
					angle++;
					break;
				case sf::Keyboard::Down:
					angle--;
					break;
				case sf::Keyboard::Space:
					render = !render;
					break;
			}
		}

	}

	if (!render) {
		return;
	}

	_window.setActive(true);
	//Prepare for drawing
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Apply some transformations for the cube
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-mX, mY, -sdepth);

	glRotatef(sphi, 0.f, 1.f, 0.f);
	glRotatef(stheta, 1.f, 0.f, 0.f);	
	glRotatef(srow, 0.f, 0.f, 1.f);

	glBegin(GL_QUADS);//draw some squares
	glColor3f(0.1, 0.1, 0.1);
	glVertex3f(-track_length / 2, 0, -250.f);
	glVertex3f(-track_length / 2, 0, 250.f);
	glVertex3f(track_length / 2, 0, 250.f);
	glVertex3f(track_length / 2, 0, -250.f);

	glColor3f(1, 1, 1);
	glVertex3f(track_length / 2 - 100, 0, -250.f);
	glVertex3f(track_length / 2 - 100, 0, 250.f);
	glVertex3f(track_length / 2 - 50, 0, 250.f);
	glVertex3f(track_length / 2 - 50, 0, -250.f);

	glEnd();

	for (int i = 0; i < robots->size(); i++) {
		if (robots[0][i]->alive) {
			drawRobot(robots[0][i]);
		}
	}


	
	_window.display();
}

float renderer::modY(float y) {
	return _height - (y + 20);
}

void renderer::drawRobot(robot * r) {

	for (int i = 0; i < r->muscles.size(); i++) {
		drawLine(r->muscles[i]->a->position, r->muscles[i]->b->position, r->muscles[i]->strength);
	}
	for (int i = 0; i < r->joints.size(); i++) {
		drawJoint(r->joints[i]->position, r->joints[i]->weight);
	}

}

void renderer::drawJoint(point * p, int weight) {
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glColor3f(weight/255, 0.3, 0.3);
	glVertex3f(p->x - (track_length / 2), p->y, p->z);
	glEnd();
}

void renderer::drawLine(point * a, point * b, int strength) {
	glBegin(GL_LINES);
	glColor3f(0.3, strength/255, 0.3);
	glVertex3f(a->x - (track_length / 2), a->y, a->z);
	glVertex3f(b->x - (track_length / 2), b->y, b->z);
	glEnd();
}