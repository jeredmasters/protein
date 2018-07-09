#pragma once
#include<string>

class result {
public:
	int fitness;
	int millis;
	std::string chromosome;
	result(int m, int f, std::string c) {
		millis = m;
		fitness = f;			
		chromosome = c;
	}
};