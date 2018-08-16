
#include "stdafx.h"
#include <windows.h>
#include <tchar.h>


using namespace std;

int oneSim(renderer* rendererz, const char * gene[], int size) {
	obstacle * _obstacle = new obstacle(3);
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

result * multiGenerations(renderer* rendererz, parameters * params)
{
	srand(time(NULL));

	float average_rate = 1;

	std::vector<robot*> robots;

	obstacle * _obstacle = new obstacle(3);
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
	long max_fittness;
	string best_chromosome;
	int max_generations = params->generations;

	population = _ga->newGeneration();

	for (int generation = 1; generation <= max_generations; generation++) {
		float gen_ratio = (float)generation / (float)params->generations;
		#pragma omp for
		for (int i = 0; i < population.size(); i++) {
			robot * r = new robot(population[i]);
			if (r->alive) {
				#pragma omp critical
				robots.push_back(r);
			}
			else {
				delete r;
			}
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

			if (seconds > 0.01 && robots.size() > 0) {
				c_draw = clock();
				rendererz->update(&robots, _obstacle);
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

		_ga->breed(&population);
		if (population.size() == 0) {
			generation = 0;
			population = _ga->newGeneration();
			c_reset = clock();
			resets++;
			if (resets == 1000) {
				// Bomb out
				generation = max_generations;
				best_chromosome = "";
				max_fittness = -1;
			}
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
	return new result(millis, max_fittness, best_chromosome);
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
	else if (argc == 2) {
		parameters * params = new parameters();
		multiGenerations(_renderer, params);
		delete params;
	}
	else {
		for (int i = 0; i < argc; ++i)
			argv[i] = argv[i + 1];
		delete argv[argc - 1];
		return oneSim(_renderer, argv, argc - 1);
	}
}


