#pragma once

namespace Magma {

	class Timestep {

	public:
		Timestep() = default;

		void setStart(double start);
		void setEnd(double end);

		// returns frames per second
		double getFPS();
		// returns milliseconds per frame
		double getMPS();

		double getElapsed();

	private:
		float start, end;

	};

}

