#pragma once

#include "stdafx.h"
#include <sstream>
#include <vector>

using namespace std;

class chromosome {
public:
	std::vector<uint16_t> dna;
	long fittness;
	long weighted_rank;
	chromosome(string str);
	chromosome(std::vector<uint16_t> _dna);
	~chromosome();
	std::string toString();

};

