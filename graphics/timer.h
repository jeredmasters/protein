#pragma once

#include <time.h>

class timer {
	private:
		int _runningCount;
		float _runningTotal;
		clock_t _start;

	public:
		timer();
		void start();
		void stop();
		float getTotal();
		float getAverage();
		bool running();
		float getDisplay();
		void reset();		
};
