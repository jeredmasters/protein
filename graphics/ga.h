#pragma once

#include "chromosome.h"

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

	public:
		std::vector<chromosome*> newGeneration();
		void breed(std::vector<chromosome*>* population);
		void mutate(std::vector<chromosome*> * population, int generation);
		ga(int _size, int _length, int _selectionPressure);
};
