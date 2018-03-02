#pragma once

#include "stdafx.h"
#include <sstream>

#include <vector>

using namespace std;

class chromosome {
public:
	std::vector<uint8_t> dna;
	double fittness;
	chromosome(string str);
	chromosome(std::vector<uint8_t> _dna);
	void dispose();
	std::string toString();
};


chromosome::chromosome(std::vector<uint8_t> _dna) {
	dna = _dna;
	fittness = 0;
}

chromosome::chromosome(string str) :
	dna()
{
	fittness = 0;
	int index = 0;
	while (str.length() > 0) {
		index = str.find('|');
		string num = str.substr(0, index);
		index++;
		str = str.substr(index, str.length() - index);
		dna.push_back(atoi(num.c_str()));
	}
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