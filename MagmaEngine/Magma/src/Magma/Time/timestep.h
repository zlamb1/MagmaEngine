#pragma once

// should timestep be dependent on GLFW?
#include "GLFW/glfw3.h"

namespace Magma {

	class Timestep {

	public:
		Timestep();

		void onNewFrame();

		double getFps();
		double getMps();
		double getElapsed();

		void reset();

	private:
		double start = 0.0, end = 0.0;
		int frames = 1; 

	};

}

