#include "timestep.h"

namespace Magma {

	Timestep::Timestep() {
		start = glfwGetTime();
		end = start; 
	}

	void Timestep::onNewFrame() {
		end = glfwGetTime(); 
		frames++;
	}

	double Timestep::getFps() {
		return 1000.0 / getMps();
	}

	double Timestep::getMps() {
		return getElapsed() * 1000.0 / frames; 
	}

	double Timestep::getElapsed() {
		return end - start; 
	}

	void Timestep::reset() {
		start = glfwGetTime();
		end = start; 
		frames = 1; 
	}

}