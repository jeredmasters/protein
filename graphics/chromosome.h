#pragma once

#include "stdafx.h"
#include <sstream>

#include <vector>

using namespace std;

class chromosome {
	public:
		std::vector<uint8_t> dna;
		double fittness;
		chromosome(std::vector<uint8_t> _dna);
		void dispose();
		std::string toString();
};

chromosome::chromosome(std::vector<uint8_t> _dna) {
	dna = _dna;
	fittness = 0;
}

std::string chromosome::toString() {
	std::stringstream ss;

	for (int i = 0; i < dna.size(); i++) {
		ss << (int)dna[i] << "|";
	}

	return ss.str();
}

void chromosome::dispose() {
	dna.clear();
}

