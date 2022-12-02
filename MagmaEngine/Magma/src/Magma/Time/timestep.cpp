#include "timestep.h"

namespace Magma {

	void Timestep::setStart(double start) {
		this->start = start;
	}

	void Timestep::setEnd(double end) {
		this->end = end;
	}

	double Timestep::getFPS() {
		return 1000.0 / getMPS();
	}

	double Timestep::getMPS() {
		return getElapsed() * 1000.0; 
	}

	double Timestep::getElapsed() {
		return end - start; 
	}

}