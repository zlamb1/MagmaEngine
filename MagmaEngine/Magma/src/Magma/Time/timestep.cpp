#include "timestep.h"

namespace Magma {

	Timestep::Timestep() {
		m_StartTime = glfwGetTime();
		m_EndTime = m_StartTime; 
	}

	void Timestep::onNewFrame() {
		m_EndTime = glfwGetTime(); 
		m_FramesElapsed++;
	}

	double Timestep::getFps() {
		return 1000.0 / getMps();
	}

	double Timestep::getMps() {
		return getElapsed() * 1000.0 / m_FramesElapsed; 
	}

	double Timestep::getElapsed() {
		return m_EndTime - m_StartTime; 
	}

	void Timestep::reset() {
		m_StartTime = glfwGetTime();
		m_EndTime = m_StartTime; 
		m_FramesElapsed = 1; 
	}

}