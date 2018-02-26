#pragma once

#include "chromosome.h"

using namespace std;

class ga {
	private:
		int size;
		int length;
		int selectionPressure;
		std::vector<uint8_t> crossover(std::vector<uint8_t>* a, std::vector<uint8_t>* b);
		std::vector<uint8_t>* choose(std::vector<chromosome*>* population, int max);
		std::vector<uint8_t>* newDna();

	public:
		std::vector<chromosome*> newGeneration();
		std::vector<chromosome*> breed(std::vector<chromosome*>* population);
		void mutate(std::vector<chromosome*> * population, int generation);
		ga(int _size, int _length, int _selectionPressure);
};
ga::ga(int _size, int _length, int _selectionPressure) {
	size = _size;
	length = _length;
	selectionPressure = _selectionPressure;
}
uint8_t rand8() {
	return (uint8_t)(rand() % ((255) + 1));
}
int randVal(int max) {
	return (rand() % (max + 1));
}
std::vector<uint8_t>* ga::choose(std::vector<chromosome*>* population, int max) {
	int v = randVal(max);
	for (int i = 0; i < population->size(); i++) {
		v -= population[0][i]->fittness;
		if (v < 0) {
			return &population[0][i]->dna;
		}
	}
	return newDna();
}

std::vector<uint8_t> ga::crossover(std::vector<uint8_t>* a, std::vector<uint8_t>* b) {
	srand(time(NULL));
	int frequency = 20;
	bool choose_a = randVal(1) == 0;
	std::vector<uint8_t> retval;
	retval.reserve(length);
	for (int i = 0; i < length; i++) {
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

std::vector<uint8_t>* ga::newDna(){
	std::vector<uint8_t>* dna = new std::vector<uint8_t>;
	dna->reserve(length);
	for (int j = 0; j < length; j++) {
		dna->push_back(rand8());
	}
	return dna;
}

std::vector<chromosome*> ga::newGeneration() {
	srand(time(NULL));
	std::vector<chromosome*> population;
	population.reserve(size);
	for (int i = 0; i < size; i++) {
		
		population.push_back(new chromosome(*newDna()));
	}
	return population;
}

std::vector<chromosome*> ga::breed(std::vector<chromosome*>* population) {
	srand(time(NULL));
	int fittest = 0;
	int total = 0;
	for (int i = 0; i < size; i++) {
		population[0][i]->fittness = pow(population[0][i]->fittness / 1000, selectionPressure);
		int fitness = population[0][i]->fittness;
		if (fitness > fittest) {
			fittest = fitness;
		}
		total += fitness;
	}

	if (fittest <= 0) {

		return newGeneration();
	}

	std::vector<chromosome*> nextGeneration;
	nextGeneration.reserve(size);
	for (int i = 0; i < size; i++) {
		std::vector<uint8_t>* a = choose(population, total);
		std::vector<uint8_t>* b = choose(population, total);
		std::vector<uint8_t> c = crossover(a, b);
		chromosome * d = new chromosome(c);
		nextGeneration.push_back(d);
	}
	for (int i = 0; i < population->size(); i++) {
		population[0][i]->dispose();
	}
	population->clear();
	return nextGeneration;
}

void ga::mutate(std::vector<chromosome*> * population, int generation) {
	for (int i = 0; i < size; i++) {
		std::vector<uint8_t> * dna = &population[0][i]->dna;
		for (int j = 0; j < length; j++) {
			if ((int)sqrt(randVal(generation + 10)) == 0) {
				float r = (randVal(100) - 50);
				r = sqrt(r);
				dna[0][j] += (int)r % 255;
			}
		}
	}
}