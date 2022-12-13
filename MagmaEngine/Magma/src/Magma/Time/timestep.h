#pragma once

// TODO: make time function abstract
// it currently relies upon GLFW
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
		double m_StartTime = 0.0, m_EndTime = 0.0;
		int m_FramesElapsed = 1; 

	};

}

