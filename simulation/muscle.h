#pragma once

#include "joint.h"

class muscle {
	public:
		joint* a;
		joint* b;
		float _desiredLength;
		float osc_range;
		float osc_pos;
		float osc_speed;
		
		float strength;
		float dX();
		float dY();
		double length();
		float angle();
		float desiredLength();
		void oscTick();
		bool inf();
		void setOsc(float _osc_range, float _osc_speed);
		muscle(joint* _a, joint* _b, float osc, float s);
};
