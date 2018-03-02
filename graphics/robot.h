#pragma once

#include "muscle.h"
#include "chromosome.h"
#include "timer_collection.h"

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

		
	public:
		std::vector<joint*> joints;
		std::vector<muscle*> muscles;
		robot(chromosome * _gene);
		void tick();
		void tick(timer_collection* timers);
		bool alive;
		chromosome * gene;
		void dispose();
		double fittness();
};
