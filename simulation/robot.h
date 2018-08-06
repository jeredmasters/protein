#pragma once

#include "muscle.h"
#include "chromosome.h"
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
		void floor();
		void applyForce();
		int _verticalInfringements;

		
	public:
		std::vector<joint*> joints;
		std::vector<muscle*> muscles;
		robot(chromosome * _gene);
		~robot();
		void tick();
		bool alive;
		chromosome * gene;
		long fittness();
		sf::Color tag;
};
