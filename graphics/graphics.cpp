// graphics.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "renderer.h"
#include <string>
#include <iostream>
#include <time.h>
#include "ga.h"

using namespace std;



int main()
{
	srand(time(NULL));

	int count = 500;
	std::vector<robot*> robots;

	renderer* _renderer = new renderer(700, 1400);

	ga * _ga = new ga(100, 60, 3);
	std::vector<chromosome*> population = _ga->newGeneration();
	clock_t t1, t2;
	t1 = clock();
	t2 = clock();
	double total_ticks = 0;
	while (true) {
		for (int i = 0; i < population.size(); i++) {
			robot * r = new robot(population[i]);
			if (r->alive) {
				robots.push_back(r);
			}
			else {
				delete r;
			}
		}
		
		for (int ticks = 0; ticks < 2000; ticks++) {
			_renderer->update(robots);

			for (int subticks = 0; subticks < 20; subticks++) {
				#pragma omp parallel for num_threads(4)
				for (int i = 0; i < robots.size(); i++) {
					robots[i]->tick();
				}
				ticks++;
				total_ticks++;
			}

			float seconds = (clock() - t2) / CLOCKS_PER_SEC;

			Sleep(5);
			if (seconds > 1) {
				t2 = clock();
				seconds = (clock() - t1) / CLOCKS_PER_SEC;
				cout << total_ticks / seconds << "\n";
			}
		}

		population = _ga->breed(population);
		_ga->mutate(&population);

		for (int i = 0; i < robots.size(); i++) {
			robots[i]->dispose();
		}
		robots.clear();
	}
}


