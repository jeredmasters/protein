#pragma once

#include "stdafx.h"
#include <algorithm>


ga::ga(int size, int length, int selectionPressure) :
	generator(rd()),
	distribution(0, LONG_MAX)
{
	_size = size;
	_length = length;
	_selectionPressure = selectionPressure;
	_temp.reserve(_size);
	_forcedSuccess = true;
}
uint8_t rand8() {
	return (uint8_t)(rand() % ((255) + 1));
}
long ga::randVal(long max) {
	return (distribution(generator) % (max + 1));
}


std::vector<uint8_t> ga::crossover(std::vector<uint8_t>* a, std::vector<uint8_t>* b) {
	srand(time(NULL));
	int frequency = 20;
	bool choose_a = randVal(1) == 0;
	std::vector<uint8_t> retval;
	retval.reserve(_length);
	for (int i = 0; i < _length; i++) {
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

std::vector<uint8_t>* ga::newDna() {
	std::vector<uint8_t>* dna = new std::vector<uint8_t>;
	dna->reserve(_length);
	for (int j = 0; j < _length; j++) {
		dna->push_back(rand8());
	}
	return dna;
}

std::vector<chromosome*> ga::newGeneration() {
	srand(time(NULL));
	std::vector<chromosome*> population;
	population.reserve(_size);
	for (int i = 0; i < _size; i++) {

		population.push_back(new chromosome(*newDna()));
	}
	return population;
}
std::vector<uint8_t>* ga::choose(int max) {
	int v = randVal(max);
	for (int i = 0; i < _length; i++) {
		v -= _temp[i]->weighted_rank;
		if (v < 0) {
			return &_temp[i]->dna;
		}
	}
	return newDna();
}
bool sortGene(chromosome* a, chromosome* b) { return (a->fittness > b->fittness); }
void ga::breed(std::vector<chromosome*>* population) {
	std::sort(population->begin(), population->end(), sortGene);
	for (int i = 0; i < _size; i++) {
		_temp.push_back(population[0][i]);
	}

	srand(time(NULL));
	long fittest = 0;
	long total = 0;
	for (int i = 0; i < _size; i++) {
		if (_temp[i]->fittness > 0) {
			_temp[i]->weighted_rank = pow(_size - i, _selectionPressure);
			//_temp[i]->fittness = 100;
			long fitness = _temp[i]->weighted_rank;
			if (fitness > fittest) {
				fittest = fitness;
			}
			total += fitness;
		}
		else {
			_temp[i]->weighted_rank = 0;
		}
	}

	if (fittest <= 0) {
		for (int i = 0; i < _size; i++) {
			if (_temp[i]->fittness > 0) {
				cout << "FALSE REGRESSION!!\n";
			}
		}
		_temp = newGeneration();
		for (int i = 0; i < _size; i++) {
			population[0][i] = _temp[i];
		}
		cout << "REGRESSION!!\n";
		return;
	}
	if (_forcedSuccess) {
		population[0][0] = _temp[0];
	}
	for (int i = (_forcedSuccess ? 1 : 0); i < _size; i++) {
		std::vector<uint8_t>* a = choose(total);
		std::vector<uint8_t>* b = choose(total);
		std::vector<uint8_t> c = crossover(a, b);
		population[0][i] = new chromosome(c);
	}
	for (int i = (_forcedSuccess ? 1 : 0); i < population->size(); i++) {
		delete _temp[i];
	}
	_temp.clear();
}

void ga::mutate(std::vector<chromosome*> * population, int generation) {
	for (int i = (_forcedSuccess ? 1 : 0); i < _size; i++) {
		std::vector<uint8_t> * dna = &population[0][i]->dna;
		for (int j = 0; j < _length; j++) {
			if (randVal(generation) == 0) {
				dna[0][j] += (dna[0][j] + randVal(100) + (255 - 50)) % 255;
			}
		}
	}
}