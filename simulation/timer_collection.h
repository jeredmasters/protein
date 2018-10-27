#pragma once
#include "timer.h"

class timer_collection {
	public:	
		timer osc;
		timer reaction;
		timer gravity;
		timer validate;
		timer momentum;
		timer friction;
		timer floor;
		timer fitness;
		void reset();
};

void timer_collection::reset() {
	osc.reset();
	reaction.reset();
	gravity.reset();
	validate.reset();
	momentum.reset();
	friction.reset();
	floor.reset();
	fitness.reset();
}

