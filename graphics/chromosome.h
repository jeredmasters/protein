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

