
#include "stdafx.h"
#include <windows.h>
#include <tchar.h>


using namespace std;


result * multiGenerations(renderer* rendererz, parameters * params)
{
	srand(time(NULL));

	// Use no obstacle by default
	obstacle * _obstacle = new obstacle(0);

	// Set up genetic algorithm
	ga * _ga = new ga(
		params->population,	
		20, 
		params->selection_pressure, 
		params->mutation_rate, 
		params->mutation_variance, 
		params->steepest_decent, 
		params->crossover_rate
	);
	
	//
	clock_t c_start, c_draw, c_reset;
	c_start = clock();
	c_draw = clock();
	c_reset = clock();
	double total_ticks = 0;
	int resets = 0;

	float average_rate = 1;
	float seconds;
	long max_fittness;
	string best_chromosome;
	int max_generations = params->generations;

	// Initialize population with random chromosomes
	std::vector<robot*> robots;
	std::vector<chromosome*> population;
	population = _ga->newGeneration();

	for (int generation = 1; generation <= max_generations; generation++) {
		float gen_ratio = (float)generation / (float)params->generations;
		#pragma omp for
		for (int i = 0; i < population.size(); i++) {
			robot * r = new robot(population[i], false);
			if (r->alive) {
				#pragma omp critical
				robots.push_back(r);
			}
			else {
				delete r;
			}
		}
		
		// Calculate simulation run time
		int sim_duration = params->duration;
		if (params->duration_variance == 1) {
			// linearly increase the duration time
			sim_duration = params->duration * gen_ratio + 1;
		}

		// Run simulation on population for entire duration
		for (int ticks = 0; ticks < sim_duration; ) {			
			int subticks_max = 200;
			#pragma omp parallel num_threads(16)
			{
				// execute 200ms of "subticks" using OpenMP multithreading
				for (int subticks = 0; subticks < subticks_max; subticks++) {
					#pragma omp for
					for (int i = 0; i < robots.size(); i++) {
						robots[i]->tick(_obstacle);
					}
				}
			}

			ticks += subticks_max;
			total_ticks += subticks_max;

			// redraw the animation evey 50ms
			seconds = (float)(clock() - c_draw) / (float)CLOCKS_PER_SEC;
			if (seconds > 0.05 && robots.size() > 0) {
				c_draw = clock();
				rendererz->update(&robots, _obstacle);
			}

			// Abort loop if entire population is dead
			if (robots.size() == 0) {
				break;
			}
		}

		// Perform analysis after simulation
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

		}

		// Breed the population
		_ga->breed(&population);
		if (population.size() == 0) {
			// If all the robots have "died" then restart with a fresh generation
			generation = 0;
			population = _ga->newGeneration();
			c_reset = clock();
			resets++;
			if (resets == 10) {
				// Abort a record a failure
				generation = max_generations;
				best_chromosome = "";
				max_fittness = -1;
			}
		}
		else {
			// Mutate the new chromosomes
			_ga->mutate(&population, gen_ratio);
		}
		
		// Clean up robots
		#pragma omp for
		for (int i = 0; i < robots.size(); i++) {
			delete robots[i];
		}
		robots.clear();
	}
	
	// Clean up chromosomes
	for (int i = 0; i < population.size(); i++) {
		delete population[i];
	}
	population.clear();

	// Prepare statistics to report to server
	float diff((float)clock() - (float)c_reset);
	long millis = diff / (CLOCKS_PER_SEC / 1000);
	return new result(millis, max_fittness, best_chromosome);
}

int runDistributedSim(renderer* _renderer) {
	communicator * _communicator = new communicator("http://api.jered.cc/api/");
	parameters * params;
	result * _result;
	while (true) {
		params = _communicator->getParameters();
		if (!params->complete) {
			return 0;
		}

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
	renderer* _renderer = new renderer(700, 1400);	

	if (argc == 1) {
		runDistributedSim(_renderer);
	}
	else if (argc == 2 && argv[1] == "debug") {
		parameters * params = new parameters();
		multiGenerations(_renderer, params);
		delete params;
	}
}


