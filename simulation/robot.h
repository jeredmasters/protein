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
		double springForce(float sd);

		int _verticalInfringements;

		float _gravity;
		float _airFriction;
		float _groundFriction;
		float _groundHardness;
		float _muscleElasticity;
		float _inertia;


		
	public:
		std::vector<joint*> joints;
		std::vector<muscle*> muscles;
		robot(chromosome * _gene, bool randomisedPhysics);
		~robot();
		void tick(obstacle * _obstacle);
		bool alive;
		chromosome * gene;
		long fittness();
		sf::Color tag;
};
