#pragma once

#include "muscle.h"
#include "chromosome.h"
#include "obstacle.h"
#include <SFML/Graphics.hpp>

class robot 
{
	private:
		void validate();
		
		void osc();
		void reaction();
		void gravity();
		void momentum();
		void friction();
		void floor(obstacle * _obstacle);
		void applyForce();
		void calcFitness();
		int _verticalInfringements;
		int _fitnessEval;
		
	public:
		std::vector<joint*> joints;
		std::vector<muscle*> muscles;
		robot(chromosome * _gene, int fitnessEval);
		~robot();
		void tick(obstacle * _obstacle);
		bool alive;
		chromosome * gene;
		long scaleFitness(int total_ticks);
		sf::Color tag;
};
