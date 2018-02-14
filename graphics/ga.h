#pragma once

#include "chromosome.h"

using namespace std;

class ga {
	public:
		std::vector<chromosome*> newGeneration(int size, int length);
		std::vector<chromosome*> breed(std::vector<chromosome*> population);
		void mutate(std::vector<chromosome*> * population);
};
uint8_t rand8() {
	return (uint8_t)(rand() % ((255) + 1));
}
int randVal(int max) {
	return (rand() % (max + 1));
}
std::vector<uint8_t>* choose(std::vector<chromosome*> population, int max) {
	int v = randVal(max);
	for (int i = 0; i < population.size(); i++) {
		v -= population[i]->fittness;
		if (v < 0) {
			return &population[i]->dna;
		}
	}
}

std::vector<uint8_t> crossover(std::vector<uint8_t>* a, std::vector<uint8_t>* b) {
	srand(time(NULL));
	int frequency = 20;
	int total = a->size();
	bool choose_a = randVal(1) == 0;
	std::vector<uint8_t> retval;
	retval.reserve(total);
	for (int i = 0; i < total; i++) {
		if (choose_a) {
			retval.push_back((*a)[i]);
		}
		else {
			retval.push_back((*b)[i]);
		}
		if (randVal(frequency) == 0) {
			choose_a = !choose_a;
		}
	}
	return retval;
}

std::vector<chromosome*> ga::newGeneration(int size, int length) {
	srand(time(NULL));
	std::vector<chromosome*> population;
	population.reserve(size);
	for (int i = 0; i < size; i++) {
		std::vector<uint8_t> dna;
		dna.reserve(length);
		for (int j = 0; j < length; j++) {
			dna.push_back(rand8());
		}
		population.push_back(new chromosome(dna));
	}
	return population;
}

std::vector<chromosome*> ga::breed(std::vector<chromosome*> population) {
	srand(time(NULL));
	int fittest = 0;
	int total = 0;
	for (int i = 0; i < population.size(); i++) {
		int fitness = population[i]->fittness;
		if (fitness > fittest) {
			fittest = fitness;
		}
		total += fitness;
	}

	std::vector<chromosome*> nextGeneration;
	nextGeneration.reserve(population.size());
	for (int i = 0; i < population.size(); i++) {
		std::vector<uint8_t>* a = choose(population, total);
		std::vector<uint8_t>* b = choose(population, total);
		std::vector<uint8_t> c = crossover(a, b);
		chromosome * d = new chromosome(c);
		nextGeneration.push_back(d);
	}
	for (int i = 0; i < population.size(); i++) {
		population[i]->dispose();
	}
	population.clear();
	return nextGeneration;
}

void ga::mutate(std::vector<chromosome*> * population) {
	for (int i = 0; i < population->size(); i++) {
		std::vector<uint8_t> * dna = &population[0][i]->dna;
		for (int j = 0; j < dna->size(); j++) {
			if (randVal(40) == 0) {
				dna[0][j] += (randVal(20) - 10);
			}
		}
	}
}