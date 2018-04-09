
#include "stdafx.h"
#include <windows.h>
#include <tchar.h>


using namespace std;

int oneSim(renderer* rendererz, const char * gene) {
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
				rendererz->update(&robots);
			}
		}
		delete c;
		delete r;
	}
	return 0;
}
void writeSuccess(int pop, int gen, long fitness) {
	ofstream datfile;
	datfile.open("C:\\Users\\jered\\Documents\\projects\\protein\\results.csv", ios::app);
	datfile << pop << "," << gen << "," << fitness << "\n";
	datfile.close();
}
void writeTransformation(chromosome * start, chromosome * end) {
	ofstream datfile;
	datfile.open("C:\\Users\\jered\\Documents\\projects\\protein\\nn_model\\data\\transform_data.dat", ios::app);
	datfile << start->toString() << end->toString() << "\n";
	datfile.close();
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

long multiGenerations(renderer* rendererz, int pop_size, int generations)
{
	srand(time(NULL));

	int count = 500;
	bool singleRoot = false;
	long total_best = 0;
	int max_rounds = 1;
	std::vector<robot*> robots;

	timer_collection* timers = new timer_collection;

	

	ga * _ga = new ga(pop_size,	20, 3);
	chromosome* root = new chromosome(std::vector<uint16_t>());
	std::vector<chromosome*> population;
	clock_t c_start, c_draw, c_log;
	c_start = clock();
	c_draw = clock();
	double total_ticks = 0;
	int generation = 1;
	float average_rate = -1;

	for(int rounds = 0; rounds < max_rounds; rounds++) {
		if (singleRoot) {
			root = new chromosome(*_ga->newDna());
			population = _ga->newGeneration(root);
		}
		else {
			population = _ga->newGeneration();
		}

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
		int max_generations = generations;

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
					rendererz->update(&robots);
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
					if (fittness > max_fittness) {
						fittest = robots[i];
						best_chromosome = robots[i]->gene->toString();
						max_fittness = fittness;
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


				if (singleRoot) {
					if (generation == max_generations) {
						writeTransformation(root, fittest->gene);
					}
				}
				else {
					if (generation >= 10) {
						writeResults(&robots);
					}
				}
			}

			_ga->breed(&population);
			if (population.size() == 0) {
				generation = 0;
				if (singleRoot) {
					root = new chromosome(*_ga->newDna());
					population = _ga->newGeneration(root);
				}
				else {
					population = _ga->newGeneration();
				}
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
		total_best += max_fittness;
		myfile.open("C:\\temp\\data.csv", ios::app);
		myfile << max_fittness << "," << best_chromosome << "\n";
		myfile.close();
	}

	for (int i = 0; i < population.size(); i++) {
		delete population[i];
	}
	population.clear();

	return total_best / max_rounds;
}


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}


int main(int argc, const char * argv[]) {
	communicator * _communicator = new communicator("http://127.0.0.1:8000/api/");
	renderer* _renderer = new renderer(700, 1400);
	parameters * params = _communicator->getParameters();
	if (argc == 1) {
		clock_t t1, t2;
		t1 = clock();	
		long fitness = 0;
		if (params->benchmark) {
			multiGenerations(_renderer, 20, 20);
		}
		else {
			fitness = multiGenerations(_renderer, params->population, params->generations);
		}	
		t2 = clock();
		float diff((float)t2 - (float)t1);
		long millis = diff / (CLOCKS_PER_SEC / 1000);
		_communicator->pushResults(params->id, fitness, millis);
	}
	else {
		return oneSim(_renderer, argv[1]);
	}
}


