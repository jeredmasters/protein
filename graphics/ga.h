#pragma once

#include "chromosome.h"
#include <random>

using namespace std;

class ga {
	private:
		int _size;
		int _length;
		int _selectionPressure;
		std::vector<chromosome*> _temp;
		std::vector<uint8_t> crossover(std::vector<uint8_t>* a, std::vector<uint8_t>* b);
		std::vector<uint8_t>* choose(int max);
		std::vector<uint8_t>* newDna();
		std::random_device rd;
		std::default_random_engine generator;
		std::uniform_int_distribution<long> distribution;
		long randVal(long max);
		bool _forcedSuccess;

	public:
		std::vector<chromosome*> newGeneration();
		void breed(std::vector<chromosome*>* population);
		void mutate(std::vector<chromosome*> * population, int generation);
		ga(int _size, int _length, int _selectionPressure);
};
