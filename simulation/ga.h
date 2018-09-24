#pragma once

#include "chromosome.h"
#include <random>

using namespace std;

class ga {
	private:
		int _rndCore;
		int _size;
		int _mutationRate;
		int _mutationVariance;
		int _length;
		int _selectionPressure;
		int _crossover_rate;

		std::vector<chromosome*> _temp;
		void crossover(std::vector<uint16_t>* a, std::vector<uint16_t>* b, std::vector<chromosome*>* population, int i);
		std::vector<uint16_t>* choose(long long int max);
		
		std::random_device rd;
		std::default_random_engine generator;
		std::uniform_int_distribution<long long int> distribution;
		
		long randLONG(long max);
		long long int randLLONG(long long int max);
		bool _steepestDecent;
		

	public:
		int size();
		std::vector<uint16_t>* newDna();
		std::vector<chromosome*> newGeneration();
		void breed(std::vector<chromosome*>* population);
		void mutate(std::vector<chromosome*> * population, float gen_ratio);
		ga(int size, int length, int selectionPressure, int mutationRate, int mutationVariance, bool steepestDecent, int crossover);
		uint16_t randBits(float gen_ratio);
		int bitwiseRate(float gen_ratio, int bit_significance);
};
