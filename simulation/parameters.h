#pragma once
#include <string>

class parameters {
	public:
		bool complete;
		bool benchmark;
		std::string id;
		int population;
		int generations;
		int selection_pressure;
		int duration;
		int crossover_rate;
		int mutation_rate;
		int mutation_variance;
		int duration_variance;
		int gradient_decent;
		parameters();
		bool parse(std::string input);
};

