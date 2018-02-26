#pragma once

#include <time.h>
#include "stdafx.h"

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

timer::timer() {
	_runningCount = 0;
	_runningTotal = 0;
	_start = -1;

}

void timer::start() {
	_start = clock();
}

void timer::stop() {
	_runningCount++;
	clock_t _end = clock();
	float diff = (_end - _start);
	_runningTotal += diff;
	_start = -1;
}

bool timer::running() {
	return _start != -1;
}

float timer::getTotal() {
	return _runningTotal;
}

float timer::getAverage() {
	return _runningTotal / _runningCount;
}

float timer::getDisplay() {
	return getTotal() / CLOCKS_PER_SEC;
}

void timer::reset() {
	_runningCount = 0;
	_runningTotal = 0;
}