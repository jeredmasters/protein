
#include "stdafx.h"
#include <windows.h>
#include <tchar.h>


using namespace std;

int oneSim(renderer* rendererz, const char * gene[], int size) {
	obstacle * _obstacle = new obstacle(0);
	while (true) {
		chromosome * c = new chromosome(gene, size);
		robot * r = new robot(c);

		std::vector<robot*> robots;
		robots.push_back(r);
		clock_t c_start, c_draw, c_calc;
		float seconds;
		c_start = clock();
		c_draw = clock();
		c_calc = clock();

		int subticks = 0;

		for (int ticks = 0; ticks < (10000); ticks++) {

			Sleep(1);
			
			r->tick(_obstacle);
			ticks++;

			subticks++;
			// seconds = (float)(clock() - c_draw) / (float)CLOCKS_PER_SEC;
			if (subticks > 50) {
				subticks = 0;
				c_draw = clock();
				rendererz->update(&robots, _obstacle);
			}
		}
		delete c;
		delete r;
	}
	return 0;
}

result * multiGenerations(renderer* rendererz, parameters * params)
{
	srand(time(NULL));

	float average_rate = 1;

	std::vector<robot*> robots;

	obstacle * _obstacle = new obstacle(0);
	ga * _ga = new ga(
		params->population,	
		20, 
		params->selection_pressure, 
		params->mutation_rate, 
		params->mutation_variance, 
		params->steepest_decent, 
		params->crossover_rate
	);
	std::vector<chromosome*> population;
	clock_t c_start, c_draw, c_reset;
	c_start = clock();
	c_draw = clock();
	c_reset = clock();
	double total_ticks = 0;
	int generation = 1;
	int resets = 0;


	float seconds;
	long max_fitness;
	string best_chromosome;
	int max_generations = params->generations;

	population = _ga->newGeneration();

	for (int generation = 1; generation <= max_generations; generation++) {
		float gen_ratio = (float)generation / (float)params->generations;
		#pragma omp for
		for (int i = 0; i < population.size(); i++) {
			robot * r = new robot(population[i]);

			#pragma omp critical
			robots.push_back(r);
		}
		
		int sim_duration = params->duration;
		if (params->duration_variance != 0) {
			sim_duration = params->duration * gen_ratio + 1;
		}
		for (int ticks = 0; ticks < sim_duration; ticks++) {
			int subticks_max = 200;
			#pragma omp parallel num_threads(64)
			{
				for (int subticks = 0; subticks < subticks_max; subticks++) {
					#pragma omp for
					for (int i = 0; i < robots.size(); i++) {
						robots[i]->tick(_obstacle);
					}
				}
				
			}

			ticks += subticks_max;
			total_ticks += subticks_max;


			seconds = (float)(clock() - c_draw) / (float)CLOCKS_PER_SEC;
			if (
				generation == 1 ||
				generation == 10 ||
				generation == 50 ||
				generation == 100 ||
				generation == 150 ||
				generation == 200
				) {
				long long fittest = 0;
				int fittext_index = 0;
				for (int i = 0; i < robots.size(); i++) {
					if (robots[i]->gene->fitness > fittest) {
						fittext_index = i;
						fittest = robots[i]->gene->fitness;
					}
				}
				if (seconds > 0.01 && robots.size() > 0) {
					c_draw = clock();
					rendererz->update(robots[fittext_index], _obstacle);
				}
				system("pause");
			}
			else {
				if (seconds > 0.01 && robots.size() > 0) {
					c_draw = clock();
					rendererz->update(&robots, _obstacle);
				}
			}

			if (seconds > 0.01 && robots.size() > 0) {
				c_draw = clock();
				rendererz->update(&robots, _obstacle);
			}

			if (robots.size() == 0) {
				break;
			}
		}

		if (robots.size() > 0) {
			max_fitness = -1;
			double fitness = -1;
			robot* fittest = robots[0];
			int alive = 0;
			for (int i = 0; i < robots.size(); i++) {
				fitness = robots[i]->scaleFitness(sim_duration);
				if (fitness > max_fitness) {
					fittest = robots[i];
					best_chromosome = robots[i]->gene->toString();
					max_fitness = fitness;
				}
				if (robots[i]->alive) {
					alive++;
				}
			}

			seconds = (clock() - c_start) / CLOCKS_PER_SEC;
			float avg = total_ticks / seconds;
			if (average_rate == -1 || isinf(average_rate)) {
				average_rate = avg;
			}
			else {
				average_rate = (average_rate + avg) / 2;
			}

			cout
				<< "G: " << generation << "\t"
				<< "P: " << alive << "\t"
				<< "F: " << fittest->gene->fitness << "\t"
				<< "R: " << average_rate << "\t"
				<< "TR: " << average_rate * alive << "\n";

		}

		_ga->breed(&population);
		if (population.size() == 0) {
			population = _ga->newGeneration();
		}
		else {
			_ga->mutate(&population, gen_ratio);
		}
		
		#pragma omp for
		for (int i = 0; i < robots.size(); i++) {
			delete robots[i];
		}

		robots.clear();
	}
	

	for (int i = 0; i < population.size(); i++) {
		delete population[i];
	}
	population.clear();

	float diff((float)clock() - (float)c_reset);
	long millis = diff / (CLOCKS_PER_SEC / 1000);
	return new result(millis, max_fitness, best_chromosome);
}


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void analyse(uint16_t num, std::vector<int> * count) {
	int bin;
	for (int j = 0; j < 16; j++)
	{
		bin = num % 2;
		if (bin == 1) {
			count[0][j] ++;
		}
		num /= 2;
	}
}

int runDistributedSim(renderer* _renderer) {
	communicator * _communicator = new communicator("http://api.jered.cc/api/");
	parameters * params;
	result * _result;
	while (true) {
		int attempts = 0;
		do {
			cout << "Attempting to contact server." << std::endl;
			cout << "Attempt: " << attempts << std::endl;

			Sleep(10000 * attempts);
			params = _communicator->getParameters();
			if (!params->complete) {
				cout << "Attempt failed. Retry in " << 10 * attempts << " seconds." << std::endl;
			}
			attempts++;			
		} while (!params->complete && attempts <= 10);
		if (!params->complete) {
			cout << "Could not get complete parameter set from server. Check connection." << std::endl;
			cin;
		}
		cin;
		

		if (params->benchmark) {
			_result = multiGenerations(_renderer, params);
		}
		else {
			_result = multiGenerations(_renderer, params);
			_communicator->pushChromosome(params->id, _result->chromosome, _result->fitness, _result->millis);
		}
		_communicator->pushResults(params->id, _result->fitness, _result->millis);
		delete _result;
		delete params;
	}
}

int main(int argc, const char * argv[]) {
	ga * _ga = new ga(1, 1, 1, 1, 2, true, 1);

	for (int i = 0; i < 1000; i++) {
		cout << _ga->randBits(1.9) << "\n";
	}


	

	if (argc == 1) {
		renderer* _renderer = new renderer(700, 1400);
		runDistributedSim(_renderer);
	}
	else if (argc == 2) {
		if (strcmp(argv[1], "norender") == 0) {
			renderer* _renderer = new renderer();
			runDistributedSim(_renderer);
		}
		if (strcmp(argv[1], "debug") == 0) {
			renderer* _renderer = new renderer(700, 1400);
			parameters * params = new parameters();
			multiGenerations(_renderer, params);
			delete params;
		}
	}
	else {
		renderer* _renderer = new renderer(700, 1400);
		for (int i = 0; i < argc; ++i)
			argv[i] = argv[i + 1];
		delete argv[argc - 1];
		return oneSim(_renderer, argv, argc - 1);
	}
}


