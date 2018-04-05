
#include "stdafx.h"
#include <windows.h>


using namespace std;



int oneSim(const char * gene) {
	renderer* _renderer = new renderer(700, 1400);
	while (true) {
		chromosome * c = new chromosome(string(gene));
		robot * r = new robot(c);

		std::vector<robot*> robots;
		robots.push_back(r);
		clock_t c_start, c_draw, c_calc;
		float seconds;
		c_start = clock();
		c_draw = clock();
		c_calc = clock();

		for (int ticks = 0; ticks < (10000); ticks++) {

			Sleep(1);


			seconds = (float)(clock() - c_calc) / (float)CLOCKS_PER_SEC;
			if (seconds > 0.001) {
				r->tick();
				ticks++;
			}


			seconds = (float)(clock() - c_draw) / (float)CLOCKS_PER_SEC;
			if (seconds > 0.1) {
				c_draw = clock();
				_renderer->update(&robots);
			}
		}
		delete c;
		delete r;
	}
	return 0;
}
void writeResults(vector<robot*> * robots) {
	ofstream datfile;
	datfile.open("C:\\Users\\jered\\Documents\\projects\\protein\\nn_model\\data\\robotic_data.dat", ios::app);
	for (int i = 0; i < robots->size(); i++) {
		if (i != 0 && robots[0][i]->gene->dna.size() > 0 && i % 8 == 0) {
			string gene = robots[0][i]->gene->toString();
			long fitness = robots[0][i]->gene->fittness;

			datfile << gene << fitness << "\n";
		}
	}
	datfile.close();
}
int multiGenerations()
{
	srand(time(NULL));

	int count = 500;
	std::vector<robot*> robots;

	timer_collection* timers = new timer_collection;

	renderer* _renderer = new renderer(700, 1400);

	ga * _ga = new ga(50,	20, 3);
	std::vector<chromosome*> population;
	clock_t c_start, c_draw, c_log;
	c_start = clock();
	c_draw = clock();
	double total_ticks = 0;
	int generation = 1;
	float average_rate = -1;

	while (true) {
		population = _ga->newGeneration();

		time_t t = time(NULL);
		struct tm buf;
		char str[26];
		gmtime_s(&buf, &t);
		strftime(str, sizeof(str), "%d%m%Y_%I%M%S", &buf);

		std::stringstream ss;
		ss << "C:\\temp\\simulation_" << str << ".csv";

		ofstream myfile;		
		myfile.open(ss.str());

		float seconds;
		double max_fittness;
		string best_chromosome;
		int max_generations = 100;

		for (int generation = 1; generation <= max_generations; generation++) {
			for (int i = 0; i < population.size(); i++) {
				robot * r = new robot(population[i]);
				if (r->alive) {
					robots.push_back(r);
				}
				else {
					delete r;
				}
			}
			int sim_duration = 100 * (generation + 20);
			for (int ticks = 0; ticks < sim_duration; ticks++) {

				int subticks_max = 40;
#pragma omp parallel num_threads(16)
				{
					for (int subticks = 0; subticks < subticks_max; subticks++) {
#pragma omp for
						for (int i = 0; i < robots.size(); i++) {
							robots[i]->tick();
						}
					}
				}
				ticks += subticks_max;
				total_ticks += subticks_max;


				seconds = (float)(clock() - c_draw) / (float)CLOCKS_PER_SEC;

				if (seconds > 0.02 && robots.size() > 0) {
					c_draw = clock();
					_renderer->update(&robots);
				}

				if (robots.size() == 0) {
					break;
				}
			}

			if (robots.size() > 0) {
				max_fittness = -1;
				double fittness = -1;
				robot* fittest = robots[0];
				for (int i = 0; i < robots.size(); i++) {
					fittness = robots[i]->fittness();
					if (fittness > fittest->gene->fittness) {
						fittest = robots[i];
						best_chromosome = robots[i]->gene->toString();
					}
				}

				c_log = clock();
				seconds = (clock() - c_start) / CLOCKS_PER_SEC;
				float avg = total_ticks / seconds;
				if (average_rate == -1 || isinf(average_rate)) {
					average_rate = avg;
				}
				else {
					average_rate = (average_rate + avg) / 2;
				}

				cout
					<< generation << "\t"
					<< robots.size() << "\t"
					<< fittest->gene->fittness << "\t"
					<< average_rate * robots.size() << "\t"
					<< average_rate << "\n";

				myfile
					<< generation << ","
					<< fittest->gene->fittness << ","
					<< average_rate << "\n";
			}

			if (generation >= 10) {
				writeResults(&robots);
			}

			_ga->breed(&population);
			if (population.size() == 0) {
				generation = 0;
				population = _ga->newGeneration();
			}
			else {
				_ga->mutate(&population, generation);
			}




			for (int i = 0; i < robots.size(); i++) {
				delete robots[i];
			}

			robots.clear();
		}
		myfile.close();

		myfile.open("C:\\temp\\data.csv", ios::app);
		myfile << max_fittness << "," << best_chromosome << "\n";
		myfile.close();
	}


	return 0;
}


int main(int argc, const char * argv[]) {
	if (argc == 1) {
		return multiGenerations();
	}
	else {
		return oneSim(argv[1]);
	}
}
