#pragma once

#include "stdafx.h"
#include <vector>

using namespace std;

class chromosome {
	public:
		std::vector<uint8_t> dna;
		double fittness;
		chromosome(std::vector<uint8_t> _dna);
		void dispose();
};

chromosome::chromosome(std::vector<uint8_t> _dna) {
	dna = _dna;
	fittness = 0;
}

void chromosome::dispose() {
	dna.clear();
}