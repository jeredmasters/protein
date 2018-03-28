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
uint16_t rand16() {
	uint16_t r = (uint16_t)(rand() % 65536);
	return r;
}
long ga::randVal(long max) {
	return (distribution(generator) % (max + 1));
}


std::vector<uint16_t> ga::crossover(std::vector<uint16_t>* a, std::vector<uint16_t>* b) {
	srand(time(NULL));
	int frequency = 20;
	bool choose_a = randVal(1) == 0;
	std::vector<uint16_t> retval;
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

std::vector<uint16_t>* ga::newDna() {
	std::vector<uint16_t>* dna = new std::vector<uint16_t>;
	dna->reserve(_length);
	for (int j = 0; j < _length; j++) {
		dna->push_back(rand16());
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
std::vector<uint16_t>* ga::choose(int max) {
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
			_temp[i]->weighted_rank = pow(_temp[i]->fittness / 1000, _selectionPressure) + _temp[i]->fittness;
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
		std::vector<uint16_t>* a = choose(total);
		std::vector<uint16_t>* b = choose(total);
		std::vector<uint16_t> c = crossover(a, b);
		population[0][i] = new chromosome(c);
	}
	for (int i = (_forcedSuccess ? 1 : 0); i < population->size(); i++) {
		delete _temp[i];
	}
	_temp.clear();
}

uint16_t ga::randBits(int generation) {
	uint16_t retval = 0x00;
	for (int i = 0; i < 16; i++) {
		if (randVal(generation + 5) == 0) {
			retval += pow(2, i);
		}
	}
	return retval;
}

void ga::mutate(std::vector<chromosome*> * population, int generation) {
	for (int i = (_forcedSuccess ? 1 : 0); i < _size; i++) {
		std::vector<uint16_t> * dna = &population[0][i]->dna;
		for (int j = 0; j < _length; j++) {
			dna[0][j] = dna[0][j] ^ randBits(generation);
		}
	}
}