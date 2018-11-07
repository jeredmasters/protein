#pragma once

#include "stdafx.h"
#include <algorithm>


ga::ga(int size, int length, int selectionPressure, int mutationRate, int mutationVariance, bool steepestDecent, int crossover) :
	generator(rd()),
	distribution(0, LONG_MAX)
{
	_rndCore = 0;
	_size = size;
	_length = length;
	_selectionPressure = selectionPressure;
	_mutationRate = mutationRate;
	_mutationVariance = mutationVariance;
	_steepestDecent = steepestDecent;
	_crossover_rate = crossover;
	_temp.reserve(_size);
}

/* Generate a random long less then max */
long ga::randLONG(long max) {
	return (distribution(generator) % (max + 1));
}

/* Generate a random long long less than max */
long long int ga::randLLONG(long long int max) {
	long long int r1 = distribution(generator);
	long long int r2 = distribution(generator);
	long long int r3 = distribution(generator);
	return ((r1 * r2 * r3) % (max + 1));
}

/* Crossover two chromosomes and insert them back into the population */
void ga::crossover(std::vector<uint16_t>* a, std::vector<uint16_t>* b, std::vector<chromosome*>* population, int i) {
	// initialize random time
	srand(time(NULL));
	
	bool slice = randLONG(1) == 0;
	std::vector<uint16_t> c;
	std::vector<uint16_t> d;
	c.reserve(_length);
	d.reserve(_length);

	// if zero crossover, copy the first chromosome verbatim 
	if (_crossover_rate <= 0) {
		for (int i = 0; i < _length; i++) {
			c.push_back((*a)[i]);
		}
		for (int i = 0; i < _length; i++) {
			d.push_back((*b)[i]);
		}
	}
	else {
		// Choose chrossover points
		int * slices = (int*)malloc(sizeof(int) * _crossover_rate);
		for (int i = 0; i < _crossover_rate; i++) {
			slices[i] = (int)randLONG(_length);
		}

		// loop through each segment of the new chromosome
		for (int i = 0; i < _length; i++) {

			// if choose_a, then copy the respective segment from a, other wise copy from b
			if (slice) {
				c.push_back((*a)[i]);
				d.push_back((*b)[i]);
			}
			else {
				d.push_back((*a)[i]);
				c.push_back((*b)[i]);
			}

			for (int j = 0; j < _crossover_rate; j++) {
				if (slices[j] == i) {
					slice = !slice;
					break;
				}
			}
		}
		free(slices);
	}

	// Put both new chromosomes into the population
	// (unless steepest descent is on and this is the last chromosome to go in)
	population[0][i] = new chromosome(c);	
	if (i + 1 < _size) { // Occurs in the case of steepest descent
		population[0][i + 1] = new chromosome(d);
	}
}

/* Create an entirely new chromosome */
std::vector<uint16_t>* ga::newDna() {
	std::vector<uint16_t>* dna = new std::vector<uint16_t>;
	dna->reserve(_length);
	for (int j = 0; j < _length; j++) {
		dna->push_back(randLONG(65536));
	}
	return dna;
}

/* Create an entirely new population */
std::vector<chromosome*> ga::newGeneration() {	
	std::vector<chromosome*> population;
	population.reserve(_size);
	for (int i = 0; i < _size; i++) {
		chromosome * c = new chromosome(*newDna());

		robot * r = new robot(c, false);	
		if (!r->alive) {
			delete c;
			i--;
		}
		else {
			population.push_back(c);
		}
		delete r;
	}
	return population;
}

/* Choose a chromosome based on the roulette wheel */
std::vector<uint16_t>* ga::choose(long long int max) {
	long long int v = randLLONG(max);
	for (int i = 0; i < _size; i++) {
		v -= _temp[i]->weighted_rank;
		if (v < 0) {
			return &_temp[i]->dna;
		}
	}
	return newDna();
}

/* Sort evaluation */
bool sortGene(chromosome* a, chromosome* b) { return (a->fittness > b->fittness); }

/* Breed the population */
void ga::breed(std::vector<chromosome*>* population) {
	std::sort(population->begin(), population->end(), sortGene);
	for (int i = 0; i < _size; i++) {
		_temp.push_back(population[0][i]);
	}

	srand(time(NULL));
	// Handle very large values for fitness (becomes an issue when selection pressure == 4)
	long long int total = 0;

	#pragma omp parallel for reduction(+:total)
	for (int i = 0; i < _size; i++) {
		long long int weighted = _temp[i]->fittness;
		if (weighted > 0) {
			weighted = pow(weighted, _selectionPressure);		
			
			total += weighted;
			_temp[i]->weighted_rank = weighted;
		}
		else {
			_temp[i]->weighted_rank = 0;
		}
	}

	if (_steepestDecent) {
		// Copy the top performing chromosome into the next generation
		population[0][0] = _temp[0];
	}
	#pragma omp for
	for (int i = (_steepestDecent ? 1 : 0); i < _size; i+= 2) {
		std::vector<uint16_t>* a = choose(total);
		std::vector<uint16_t>* b = choose(total);
		crossover(a, b, population, i);
	}
	#pragma omp for
	for (int i = (_steepestDecent ? 1 : 0); i < population->size(); i++) {
		delete _temp[i];
	}
	_temp.clear();
}

/* Calculation the bitwise mutation scaling */
float ga::bitwiseRate(float gen_ratio, int bit_significance) {
	float y = gen_ratio;
	float x = 3-bit_significance;	
	
	float bit_ratio = (-2.0 / 3.0)*x - y + (4.0 / 6.0)*x*y + 2.0;

	return _size / (_mutationRate * bit_ratio);
}

/* Get random bits for mutation */
uint16_t ga::randBits(float gen_ratio) {
	uint16_t retval = 0x00;
	switch (_mutationVariance) {
	case 0: // No mutation variance
		for (int i = 0; i < 16; i++) {
			if (randLONG(_size / _mutationRate + 1) == 0) {
				retval += pow(2, i);
			}
		}
		break;
	case 1: // Linear mutation variance
		for (int i = 0; i < 16; i++) {
			if (randLONG(_size / (_mutationRate / gen_ratio) + 1) == 0) {
				retval += pow(2, i);
			}
		}
		break;
	case 2: // Bitwise mutation variance
		for (int a = 0; a < 4; a++) {
			for (int bit = 0; bit < 4; bit++) {
				int i = a * 4 + bit;
				if (randLONG(bitwiseRate(gen_ratio, bit)) == 0) {
					retval += pow(2, i);
				}
			}
		}
		break;
	}
	return retval;
}

/* Mutate an entire population */
void ga::mutate(std::vector<chromosome*> * population, float gen_ratio) {
	if (_mutationRate == 0) {
		return;
	}
	#pragma omp for
	for (int i = (_steepestDecent ? 1 : 0); i < _size; i++) {
		std::vector<uint16_t> * dna = &population[0][i]->dna;
		for (int j = 0; j < _length; j++) {
			dna[0][j] = dna[0][j] ^ randBits(gen_ratio * 2);
		}
	}
}

/* Get size */
int ga::size() {
	return _size;
}