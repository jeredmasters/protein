// graphics.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "renderer.h"
#include <string>
#include <iostream>
#include <time.h>

using namespace std;

uint8_t rand8() {
	return (uint8_t)(rand() % ((255) + 1));
}
robot* createRobot() {
	int size = 120;
	std::vector<uint8_t> chromosome;
	chromosome.reserve(size);
	for (int i = 0; i < size; i++) {
		chromosome.push_back(rand8());
	}
	return new robot(chromosome);
}

int main()
{
	srand(time(NULL));

	int count = 100;
	std::vector<robot*> robots;
	robots.reserve(10);
	for (int i = 0; i < count; i++) {
		robots.push_back(createRobot());
	}

	renderer* _renderer = new renderer(700, 1000);

	clock_t t1, t2;
	t1 = clock();
	t2 = clock();
	float t = 0;

	while (true) {
		t ++;
		// _renderer->update(robots);

		#pragma omp parallel for
		for (int i = 0; i < count; i++) {
			robots[i]->tick();
		}
		Sleep(5);
		float seconds = (clock() - t2) / CLOCKS_PER_SEC;
		if (seconds > 1) {
			t2 = clock();
			seconds = (clock() - t1) / CLOCKS_PER_SEC;
			cout << t / seconds << "\n";
		}
	}
}


