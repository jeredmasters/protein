#pragma once

#include "stdafx.h"
#include <sstream>
#include <vector>

using namespace std;

class chromosome {
public:
	std::vector<uint16_t> dna;
	long long int fitness;
	long long int weighted_rank;
	chromosome(const char * gene[], int size);
	chromosome(string str);
	chromosome(std::vector<uint16_t> _dna);
	uint8_t * get(int i);
	~chromosome();
	std::string toString();
	chromosome * clone();
};

