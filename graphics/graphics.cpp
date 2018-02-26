// graphics.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "renderer.h"
#include <string>
#include <iostream>
#include <time.h>
#include "ga.h"
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;



int main()
{

	srand(time(NULL));

	int count = 500;
	std::vector<robot*> robots;

	timer_collection* timers = new timer_collection;

	renderer* _renderer = new renderer(700, 1400);

	ga * _ga = new ga(100, 100, 3);
	std::vector<chromosome*> population = _ga->newGeneration();
	clock_t c_start, c_draw, c_log;
	c_start = clock();
	c_draw = clock();
	c_log = clock();
	double total_ticks = 0;
	int generation = 1;
	float average_rate = -1;



	time_t t = time(NULL);
	struct tm buf;
	char str[26];
	gmtime_s(&buf, &t);
	strftime(str, sizeof(str), "%d%m%Y_%I%M%S", &buf);

	std::stringstream ss;
	ss << "C:\\temp\\simulation_" << str << ".csv";

	ofstream myfile;
	myfile.open(ss.str());
	
	
	for(int generation = 1; generation < 100; generation++) {
		for (int i = 0; i < population.size(); i++) {
			robot * r = new robot(population[i]);
			if (r->alive) {
				robots.push_back(r);
			}
			else {
				delete r;
			}
		}
		for (int ticks = 0; ticks < (100 * generation); ticks++) {

			int subticks_max = 20;
			// #pragma omp parallel num_threads(8)
			{
				for (int subticks = 0; subticks < subticks_max; subticks++) {
					// #pragma omp for
					for (int i = 0; i < robots.size(); i++) {
						robots[i]->tick(timers);
					}					
				}
			}
			ticks += subticks_max;
			total_ticks += subticks_max;


			float seconds = (float)(clock() - c_draw) / (float)CLOCKS_PER_SEC;

			if (seconds > 0.05) {
				c_draw = clock();
				_renderer->update(&robots);
			}

			seconds = (clock() - c_log) / CLOCKS_PER_SEC;

			if (seconds > 2) {
				double max_fittness = -1;
				double fittness = -1;
				for (int i = 0; i < robots.size(); i++) {
					fittness = robots[i]->fittness();
					if (fittness > max_fittness) {
						max_fittness = fittness;
					}
				}

				c_log = clock();
				seconds = (clock() - c_start) / CLOCKS_PER_SEC;
				float avg = total_ticks / seconds;
				if (average_rate == -1){
					average_rate = avg;
				}
				else {
					average_rate = (average_rate + avg) / 2;
				}

				printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
					max_fittness,
					avg,
					timers->osc.getDisplay(),
					timers->reaction.getDisplay(),
					timers->gravity.getDisplay(),
					timers->validate.getDisplay(),
					timers->momentum.getDisplay(),
					timers->friction.getDisplay(),
					timers->floor.getDisplay(),
					timers->fittness.getDisplay()
				);

				myfile
					<< max_fittness << ","
					<< total_ticks / seconds << ","
					<< timers->osc.getDisplay() << ","
					<< timers->reaction.getDisplay() << ","
					<< timers->gravity.getDisplay() << ","
					<< timers->validate.getDisplay() << ","
					<< timers->momentum.getDisplay() << ","
					<< timers->friction.getDisplay() << ","
					<< timers->floor.getDisplay() << ","
					<< timers->fittness.getDisplay() << "\n"
					;

				timers->reset();
			}
		}

		for (int i = 0; i < robots.size(); i++) {
			robots[i]->fittness();
		}			

		population = _ga->breed(&population);
		_ga->mutate(&population, generation);

		for (int i = 0; i < robots.size(); i++) {
			robots[i]->dispose();
		}

		robots.clear();
	}
	myfile.close();
	return 0;
}


