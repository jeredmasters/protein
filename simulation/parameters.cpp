#include "stdafx.h"


parameters::parameters() {
	this->complete = true;
	this->benchmark = true;
	this->population = 50;
	this->generations = 50;
	this->selection_pressure = 2;
	this->duration = 20000;
	this->crossover_rate = 5;
	this->mutation_rate = 6;
	this->mutation_variance = 0;
	this->duration_variance = 0;
	this->gradient_decent = true;
}

bool parameters::parse(std::string input) {
	this->complete = false;
	if (input == "benchmark") {
		this->complete = true;
		this->benchmark = true;
		this->id = "benchmark";
	}
	else {
		this->benchmark = false;
		vector<string> strings;
		istringstream f(input);
		string s;
		while (getline(f, s, ',')) {
			strings.push_back(s);
		}

		if (strings.size() < 10) {
			this->complete = false;
			return false;
		}

		this->complete = true;
		this->id = strings[0];
		this->population = std::stoi(strings[1]);
		this->generations = std::stoi(strings[2]);
		this->selection_pressure = std::stoi(strings[3]);
		this->duration = std::stoi(strings[4]);
		this->crossover_rate = std::stoi(strings[5]);
		this->mutation_rate = std::stoi(strings[6]);
		this->mutation_variance = std::stoi(strings[7]);
		this->duration_variance = std::stoi(strings[8]);
		this->gradient_decent = std::stoi(strings[9]);
	}

	return this->complete;
}