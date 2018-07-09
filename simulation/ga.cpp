#pragma once

#include "stdafx.h"
#include <algorithm>


ga::ga(int size, int length, int selectionPressure, int mutationRate, int mutationVariance, bool gradientDecent) :
	generator(rd()),
	distribution(0, LONG_MAX)
{
	_rndCore = 0;
	_size = size;
	_length = length;
	_selectionPressure = selectionPressure;
	_mutationRate = mutationRate;
	_mutationVariance = mutationVariance;
	_gradientDecent = gradientDecent;
	_temp.reserve(_size);
	_gradientDecent = true;
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
		dna->push_back(randVal(65536));
	}
	return dna;
}

std::vector<chromosome*> ga::newGeneration() {	
	std::vector<chromosome*> population;
	population.reserve(_size);
	#pragma omp for
	for (int i = 0; i < _size; i++) {
		chromosome * c = new chromosome(*newDna());
		#pragma omp critical
		population.push_back(c);
	}
	return population;
}
std::vector<uint16_t>* ga::choose(int max) {
	int v = randVal(max);
	for (int i = 0; i < _size; i++) {
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
	long total = 0;

	#pragma omp parallel for reduction(+:total)
	for (int i = 0; i < _size; i++) {
		if (_temp[i]->fittness > 0) {
			_temp[i]->weighted_rank = pow((float)_temp[i]->fittness / 1000, _selectionPressure) + _temp[i]->fittness;
			total += _temp[i]->weighted_rank;			
		}
		else {
			_temp[i]->weighted_rank = 0;
		}
	}


	if (total <= 0) {
		#pragma omp for
		for (int i = 0; i < _size; i++) {
			if (_temp[i]->fittness > 0) {
				cout << "FALSE REGRESSION!!\n";
			}
		}
		#pragma omp for
		for (int i = 0; i < _size; i++) {
			delete _temp[i];
		}
		_temp.clear();
		population->clear();
		cout << "REGRESSION!!\n";
		return;
	}
	if (_gradientDecent) {
		population[0][0] = _temp[0];
	}
	#pragma omp for
	for (int i = (_gradientDecent ? 1 : 0); i < _size; i++) {
		std::vector<uint16_t>* a = choose(total);
		std::vector<uint16_t>* b = choose(total);
		std::vector<uint16_t> c = crossover(a, b);
		population[0][i] = new chromosome(c);
	}
	#pragma omp for
	for (int i = (_gradientDecent ? 1 : 0); i < population->size(); i++) {
		delete _temp[i];
	}
	_temp.clear();
}

int ga::bitwiseRate(float gen_ratio, int bit_significance) {
	float y = gen_ratio;
	float x = bit_significance;	
	
	float bit_ratio = (-2.0 / 3.0)*x + (-2.0 / 2.0)*y + (4.0 / 6.0)*x*y + 3.0;

	return _size / (_mutationRate * bit_ratio);
}

uint16_t ga::randBits(float gen_ratio) {
	uint16_t retval = 0x00;
	switch (_mutationVariance) {
	case 0:
		for (int i = 0; i < 16; i++) {
			if (randVal(_size / _mutationRate + 1) == 0) {
				retval += pow(2, i);
			}
		}
		break;
	case 1:
		for (int i = 0; i < 16; i++) {
			if (randVal(_size / (_mutationRate / gen_ratio) + 1) == 0) {
				retval += pow(2, i);
			}
		}
		break;
	case 2:
		for (int a = 0; a < 4; a++) {
			for (int bit = 0; bit < 4; bit++) {
				int i = a * 4 + bit;
				if (randVal(bitwiseRate(gen_ratio, bit)) == 0) {
					retval += pow(2, i);
				}
			}
		}
		break;
	}	
	
	return retval;
}

void ga::mutate(std::vector<chromosome*> * population, float gen_ratio) {
	#pragma omp for
	for (int i = (_gradientDecent ? 1 : 0); i < _size; i++) {
		std::vector<uint16_t> * dna = &population[0][i]->dna;
		for (int j = 0; j < _length; j++) {
			dna[0][j] = dna[0][j] ^ randBits(gen_ratio * 2);
		}
	}
}

int ga::size() {
	return _size;
}